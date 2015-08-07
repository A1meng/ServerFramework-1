#include <unistd.h>

#include "GameProtocol.hpp"
#include "LogAdapter.hpp"
#include "NowTime.hpp"
#include "TimeUtility.hpp"
#include "HandlerFactory.hpp"
#include "AppDef.hpp"
#include "AppLoop.hpp"
#include "ShmObjectCreator.hpp"
#include "AppUtility.hpp"
#include "ConfigHelper.hpp"
#include "ZoneOssLog.hpp"
#include "ZoneMsgHelper.hpp"
#include "FreqCtrl.hpp"
#include "ZoneObjectHelper.hpp"
#include "GMCommandManager.hpp"

class CGameRoleObj;

using namespace ServerLib;

//��ʼ��AppCmd
int CAppLoop::ms_iAppCmd = APPCMD_NOTHING_TODO;
CTimeValue CAppLoop::m_stLotusMsgMaxProsessTime;
CTimeValue CAppLoop::m_stWorldMsgMaxProcessTime;
CTimeValue CAppLoop::m_stTickMaxProcessTime;


#ifdef _DEBUG_
const int  APP_ZONE_MAX_SLEEP_USEC = 5 * 1000;
#else
const int  APP_ZONE_MAX_SLEEP_USEC = 5 * 1000;
#endif

CAppLoop::CAppLoop()
{
    m_bResumeMode = false;
}

void CAppLoop::SetAppCmd(int iAppCmd)
{
    ms_iAppCmd = iAppCmd;
}

int CAppLoop::ReloadConfig()
{
    //m_stPinDrawLucky.WriteData();

    // GameZoneServer.tcm
    CAppUtility::LoadLogConfig(APP_CONFIG_FILE, "GameZoneServer");

    CModuleHelper::LoadModuleConfig();

    // GMList.ini
    CModuleHelper::GetConfigManager()->Initialize(true);

    // OssLogConfig.xml
    CZoneOssLog::Initialize();

    TRACESVR("Reload Config Success.\n");

    return 0;
}

int CAppLoop::LoadConfig()
{
    // GameZoneServer.tcm
    CAppUtility::LoadLogConfig(APP_CONFIG_FILE, "GameZoneServer");

    CModuleHelper::LoadModuleConfig();

    // OssLogConfig.xml
    CZoneOssLog::Initialize();

    TRACESVR("Load Config Success.\n");

    return 0;
}

int CAppLoop::Initialize(bool bResume, int iWorldID, int iZoneID, int iInstanceID)
{
    if (bResume)
    {
        SetServerStatus(GAME_SERVER_STATUS_RESUME);
    }
    else
    {
        SetServerStatus(GAME_SERVER_STATUS_INIT);
    }

    //��ȡ����
    LoadConfig();

    m_bResumeMode = bResume;

    int iRet = -1;

    // ģ��ע��
    CModuleHelper::RegisterServerID(iWorldID, iZoneID, iInstanceID);
    CModuleHelper::RegisterUnitEventManager(&m_stUnitEventManager);
    CModuleHelper::RegisterConfigManager(&m_stConfigManager);
    CModuleHelper::RegisterZoneProtocolEngine(&m_stZoneProtocolEngine);
    CModuleHelper::RegisterSessionManager(&m_stSessionManager);
    CModuleHelper::RegisterZoneMsgTransceiver(&m_stZoneMsgTransceiver);

    m_stHandlerList.RegisterAllHandler();
    m_stZoneProtocolEngine.SetService(this);

    CGMCommandManager::Instance()->Init();

    // ��Ϣ�շ���
    iRet = m_stZoneMsgTransceiver.Initialize(m_bResumeMode);
    if (iRet < 0)
    {
        TRACESVR("ZoneBus initialized failed: iRet = %d\n", iRet);
        exit(3);
    }

    // Э������
    iRet = m_stZoneProtocolEngine.Initialize(m_bResumeMode);
    if (iRet < 0)
    {
        TRACESVR("Protocol Engine initialize failed: iRet = %d\n", iRet);
        exit(4);
    }

    // Excel������ü���
    iRet = m_stConfigManager.Initialize(m_bResumeMode);
    if (iRet < 0)
    {
        TRACESVR("ConfigManager initialize failed %d\n", iRet);
        exit(5);
    }

    // ����س�ʼ��
    iRet = m_stAllocator.Initialize(m_bResumeMode);
    if(iRet < 0)
    {
        TRACESVR("Allocator initialize failed, iRet %d\n", iRet);
        exit(6);
    }

    // ��¼Session
    iRet = m_stSessionManager.Initialize(m_bResumeMode);
    if (iRet < 0)
    {
        TRACESVR("SessionManager initialize failed\n");
        exit(7);
    }

    // ��ɫ��־
    TLogConfig& rstPlayerLogConfig = PlayerLogSingleton::Instance()->GetLogConfig();
    rstPlayerLogConfig.m_iLogLevel = LOG_LEVEL_ANY;

    //TLogConfig stServerLogConfig = ServerLogSingleton::Instance()->GetLogConfig();
    //stServerLogConfig.m_iMaxFileExistDays = 7;
    //ServerLogSingleton::Instance()->ReloadLogConfig(stServerLogConfig);

    //TLogConfig stPlayerLogConfig = PlayerLogSingleton::Instance()->GetLogConfig();
    //stPlayerLogConfig.m_iMaxFileExistDays = 3;
    //PlayerLogSingleton::Instance()->ReloadLogConfig(stPlayerLogConfig);

    // Ƶ�ʿ�����
    CFreqencyCtroller<0>::Instance()->SetParameters(1, 100, 100);
    CFreqencyCtroller<0>::Instance()->CheckFrequency(0);

    // Tick����
    iRet = m_stAppTick.Initialize(bResume);
    if (iRet < 0)
    {
        TRACESVR("AppTick initialize failed!\n");
        exit(18);
    }

    //Lua�ű�ϵͳ����
    //LuaScriptFramework oLuaFramework;
    //iRet = oLuaFramework.Init();
    //if(iRet)
    //{
    //    TRACESVR("fail to init lua framework, ret %d!\n", iRet);
    //    return iRet;
    //}

    // �������
    SetServerStatus(GAME_SERVER_STATUS_IDLE);

    return 0;
}

void CAppLoop::OnTick()
{
    m_stAppTick.OnTick();
}

int CAppLoop::Run()
{
    int iRet = -1;

    TRACESVR("Run, run, run, never stop!\n");
    TRACESVR("WorldID: %d, ZoneID = %d\n", CModuleHelper::GetWorldID(), CModuleHelper::GetZoneID());
    TRACESVR("Mode: %s\n", m_bResumeMode ? "Resume" : "Initialize");
    TRACESVR("==============================================================================\n");

    static char szBuffer[MAX_MSGBUFFER_SIZE];
    int iLength = sizeof(szBuffer);
    int iBusyCount = 0;

    //init tick time
    NowTimeSingleton::Instance()->EnablePseudoTime();
    CTimeUtility::m_uiTimeTick = time(NULL);
    CTimeUtility::m_stTimeValueTick.RefreshTime();

    timeval stTimeLoopStart;
    timeval stTimeLoopEnd;

    while(true)
    {
        NowTimeSingleton::Instance()->RefreshNowTime();
        NowTimeSingleton::Instance()->RefreshNowTimeVal();

        stTimeLoopStart = NowTimeSingleton::Instance()->GetNowTimeVal();
        CTimeUtility::m_stTimeValueTick = CTimeValue(stTimeLoopStart);
        CTimeUtility::m_uiTimeTick = stTimeLoopStart.tv_sec;

        if (ms_iAppCmd == APPCMD_STOP_SERVICE)
        {
            bool bStop = Stop();
            if (bStop)
            {
                break;
            }
        }

        if (ms_iAppCmd == APPCMD_RELOAD_CONFIG)
        {
            TRACESVR("Receive Command: APPCMD_RELOAD_CONFIG\n");
            ReloadConfig();
            ms_iAppCmd = APPCMD_NOTHING_TODO;
        }

        if (ms_iAppCmd == APPCMD_QUIT_SERVICE)
        {
            TRACESVR("Receive Command: APPCMD_QUIT_SERVICE\n");
            break;
        }

        // �������е���Ϣ

        int iNewMsgCount = 0;
        int riMsgLength=0;

        CTimeValue astTimeVal[4];
        CTimeValue stDiff;
        astTimeVal[0].RefreshTime();
        // ͣ���ڼ�, ������Lotus��Ϣ
        if (GetServerStatus() != GAME_SERVER_STATUS_STOP)
        {
            for (int i = 0; i < m_stZoneMsgTransceiver.GetCodeQueueNum(); i++)
            {
                // Lotus ��Ϣ
                iLength = sizeof(szBuffer);

                while(iNewMsgCount < 1000)
                {
                    riMsgLength = 0;
                    iRet = m_stZoneMsgTransceiver.RecvOneMsg(szBuffer, iLength, riMsgLength, GAME_SERVER_LOTUSZONE, i);
                    if(iRet <0 || riMsgLength<=0)
                    {
                        //���ղ���������Ϣ
                        break;
                    }

                    m_stZoneProtocolEngine.OnRecvCode(szBuffer, riMsgLength, GAME_SERVER_LOTUSZONE, i);
                    iNewMsgCount++;
                }
            }
        }

        astTimeVal[1].RefreshTime();
        stDiff = astTimeVal[1] - astTimeVal[0];
        if (stDiff > m_stLotusMsgMaxProsessTime)
        {
            m_stLotusMsgMaxProsessTime = stDiff;
        }

        // World ��Ϣ
        iLength = sizeof(szBuffer);
        riMsgLength=0;
        iRet = m_stZoneMsgTransceiver.RecvOneMsg(szBuffer, iLength, riMsgLength, GAME_SERVER_WORLD);
        if ((iRet >= 0) && (riMsgLength > 0))
        {
            m_stZoneProtocolEngine.OnRecvCode(szBuffer, riMsgLength, GAME_SERVER_WORLD);
            iNewMsgCount++;
        }

        astTimeVal[2].RefreshTime();
        stDiff = astTimeVal[2] - astTimeVal[1];
        if (stDiff > m_stWorldMsgMaxProcessTime)
        {
            m_stWorldMsgMaxProcessTime = stDiff;
        }

        EGameServerStatus enServerStatus = GetServerStatus();

        // ���÷�����״̬
        if (iNewMsgCount == 0)
        {
            if (enServerStatus != GAME_SERVER_STATUS_PRESTOP
                    && enServerStatus != GAME_SERVER_STATUS_STOP)
            {
                SetServerStatus(GAME_SERVER_STATUS_IDLE);
            }
            iBusyCount = 0;
        }
        else
        {
            if (enServerStatus != GAME_SERVER_STATUS_PRESTOP
                && enServerStatus != GAME_SERVER_STATUS_STOP)
            {
                SetServerStatus(GAME_SERVER_STATUS_BUSY);
                iBusyCount++;
            };
        }
        m_stAppTick.OnTick();

        NowTimeSingleton::Instance()->RefreshNowTimeVal();
        stTimeLoopEnd = NowTimeSingleton::Instance()->GetNowTimeVal();
        int64_t iLeftUsec = stTimeLoopStart.tv_sec * 1000000 + stTimeLoopStart.tv_usec + APP_ZONE_MAX_SLEEP_USEC
                            - stTimeLoopEnd.tv_sec * 1000000 - stTimeLoopEnd.tv_usec;

        if (iLeftUsec > 0)
        {
            usleep(iLeftUsec);
        }
        else if (iBusyCount > 1000)
        {
            iBusyCount = 0;
            usleep(10);
        }

        astTimeVal[3].RefreshTime();
        stDiff = astTimeVal[3] - astTimeVal[2];
        if (stDiff > m_stTickMaxProcessTime)
        {
            m_stTickMaxProcessTime = stDiff;
        }
    }

    return 0;
}

// ͣ��
bool CAppLoop::Stop()
{
    //m_stPinDrawLucky.WriteData();
    // PRESTOP״̬
    if ((GAME_SERVER_STATUS_PRESTOP != GetServerStatus())
        && (GAME_SERVER_STATUS_STOP != GetServerStatus()))
    {
        TRACESVR("Receive Command: APPCMD_STOP_SERVICE\n");
        SetServerStatus(GAME_SERVER_STATUS_PRESTOP);

        // ����ͣ����ʱ
        m_stStopService.RefreshTime();
        timeval stTimeval = m_stStopService.GetTimeValue();
        stTimeval.tv_sec += SERVER_PRESTOP_TIME;
        m_stStopService.SetTimeValue(stTimeval);

        return false;
    }

    // �������������Ϊ�����˳�
    static int iRoleOnline = 0;
    int iRoleOld = iRoleOnline;
    iRoleOnline = GameTypeK32<CGameRoleObj>::GetUsedObjNumber();
    if (0 == iRoleOnline)
    {
        TRACESVR("Waiting for 0 users...............\n");
        return true;
    }

    CTimeValue stTimeVal;
    stTimeVal.RefreshTime();
    // ��ʱ�����÷�����״̬�����ٴ���ĳЩ����
    if (stTimeVal > m_stStopService)
    {
        SetServerStatus(GAME_SERVER_STATUS_STOP);

        if (iRoleOld != iRoleOnline)
        {
            TRACESVR("Waiting for %d users...............\n", iRoleOnline);
        }

        return false;
    }

    // ������ʾ������ÿ��һ��
    static int siSeconds = 0;
    if (stTimeVal.GetTimeValue().tv_sec < (siSeconds + 1))
    {
        return false;
    }
    siSeconds = stTimeVal.GetTimeValue().tv_sec;

    // ÿ10����ʾ��С��10����ÿ����ʾ
    int iDeltaTime = m_stStopService.GetTimeValue().tv_sec - stTimeVal.GetTimeValue().tv_sec;
    if ( (0 != (iDeltaTime % 10)) && (iDeltaTime > 10))
    {
        return false;
    }

    // ��ʾ���пͻ���
    //todo jasonxiong ���������޸� -1 Ϊ�Լ��Ĵ����� EQEC_Server_Maintence
    //CPromptMsgHelper::SendPromptMsgToZoneAll(-1, PROMPTMSG_TYPE_POPUP, iDeltaTime);

    return false;
}

