#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <arpa/inet.h>

#include "GameProtocol.hpp"
#include "AppDef.hpp"
#include "NowTime.hpp"
#include "LogAdapter.hpp"
#include "RegAuthApp.hpp"
#include "RegAuthSingleton.hpp"
#include "AppUtility.hpp"
#include "RegAuthBillLog.hpp"

int CRegAuthApp::ms_iAppCmd = APPCMD_NOTHING_TODO;
int CRegAuthApp::m_iWorldID;

CRegAuthApp::CRegAuthApp()
{
    m_bResumeMode = false;
    memset(&m_stNetHead, 0, sizeof(TNetHead_V2));
    m_uiLoopTimes = 0;
}

CRegAuthApp::~CRegAuthApp()
{
}

int CRegAuthApp::Initialize(bool bResume, int iWorldID)
{
    g_pRegAuthApp = this;

    // ��ȡ����
    LoadConfig();

    m_bResumeMode = bResume;

    int iRes = RegAuthMsgTransceiver->Initialize(bResume, iWorldID);
    if (iRes < 0)
    {
        return -1;
    }

    iRes = RegAuthObjectAllocator->Initialize(bResume);
    if (iRes < 0)
    {
        return -1;
    }

    iRes = CSProtocolEngine->Initialize();
    if (iRes < 0)
    {
        return -1;
    }

    iRes = SSProtocolEngine->Initialize();
    if (iRes < 0)
    {
        return -1;
    }

    iRes = RegAuthHandlerSet->Initialize();
    if (iRes < 0)
    {
        return -1;
    }

    iRes = SessionManager->Initialize(bResume);
    if (iRes < 0)
    {
        return -1;
    }

    // ��ȡRegAuth Server������
    iRes = ConfigMgr->LoadAllConf();
    if (iRes != 0)
    {
        TRACESVR("failed to load config\n");
        return -1;
    }

    m_iWorldID = iWorldID;

    return 0;
}

void CRegAuthApp::Run()
{
    TRACESVR("RegAuthServer starts running ...\n");

    int iRes = 0;
    m_uiLoopTimes = 0;

    while (true)
    {
        NowTimeSingleton::Instance()->RefreshNowTime();
        NowTimeSingleton::Instance()->RefreshNowTimeVal();

        if (ms_iAppCmd == APPCMD_STOP_SERVICE)
        {
            TRACESVR("Receive Command: APPCMD_STOP_SERVICE\n");
            return;
        }

        if (ms_iAppCmd == APPCMD_RELOAD_CONFIG)
        {
            TRACESVR("Receive Command: APPCMD_RELOAD_CONFIG. \n");
            ReloadConfig();
            ms_iAppCmd = APPCMD_NOTHING_TODO;
        }

        if (ms_iAppCmd == APPCMD_QUIT_SERVICE)
        {
            TRACESVR("Receive Command: APPCMD_QUIT_SERVICE\n");
            break;
        }

        const int MAX_MSG_COUNT_PER_COUNT = 10;
        int iRecvMsgCount = 0;

        while (true)
        {
            int iNewMsgCount = 0;

            for (int i = 0; i < RegAuthMsgTransceiver->GetCodeQueueNum(); i++)
            {
                iRes = HandleMsgIn(CSProtocolEngine, GAME_SERVER_LOTUSREGAUTH, i);
                if (iRes == 0)
                {
                    iNewMsgCount++;
                }
            }

            iRes = HandleMsgIn(SSProtocolEngine, GAME_SERVER_ACCOUNTDB);
            if (iRes == 0)
            {
                iNewMsgCount++;
            }

            // û������, �������ݳ�����������, ��ͣ��������
            iRecvMsgCount += iNewMsgCount;
            if (iNewMsgCount == 0 || iRecvMsgCount >= MAX_MSG_COUNT_PER_COUNT)
            {
                break;
            }
        }

        usleep(MAIN_LOOP_SLEEP);
        m_uiLoopTimes++;

        // ����ʱ��cache��㣬��ѭ��1�������һ��
        if (TIMES_TO_CLEAR_CACHE == m_uiLoopTimes)
        {
            ClearCache();
            m_uiLoopTimes = 0; // ���¼���
        }
    }
}

void CRegAuthApp::SetAppCmd(int iAppCmd)
{
    ms_iAppCmd = iAppCmd;
}

int CRegAuthApp::ReloadConfig()
{
    // StarRegAuthServer.tcm
    CAppUtility::LoadLogConfig(APP_CONFIG_FILE, "RegAuthServer");

    // osslog.xml
    CRegAuthBillLog::Initialize();

    // WhiteList.ini
    // zone_conf.txt
    ConfigMgr->LoadAllConf();

    return 0;
}

int CRegAuthApp::LoadConfig()
{
    // StarRegAuthServer.tcm
    CAppUtility::LoadLogConfig(APP_CONFIG_FILE, "RegAuthServer");

    // osslog.xml
    CRegAuthBillLog::Initialize();

    return 0;
}

int CRegAuthApp::HandleMsgIn(IProtocolEngine* pProtocolEngine, EGameServerID enMsgPeer, int iInstance)
{
    ASSERT_AND_LOG_RTN_INT(pProtocolEngine);

    static unsigned char szCodeBuf[MAX_CODE_LEN];
    int iBufLen = sizeof(szCodeBuf);
    int iCodeLen;
    static GameProtocolMsg stRequestMsg;

    // ��ʼ��code buffer��code length
    memset(szCodeBuf, 0, sizeof(szCodeBuf));
    iCodeLen = 0;

    // ������������
    int iRet = RegAuthMsgTransceiver->RecvOneMsg((char*)szCodeBuf, iBufLen, iCodeLen, enMsgPeer, iInstance);
    if ((iRet < 0) || (0 == iCodeLen))
    {
        return -1;
    }

    TRACESVR("Receive code OK, len: %d\n", iCodeLen);

    // ��ʼ��Ϊ0������ģ�
    memset(&m_stNetHead, 0, sizeof(TNetHead_V2));
    stRequestMsg.Clear();

    // ������������Ϊ��������
    iRet = pProtocolEngine->Decode((unsigned char*)szCodeBuf, iCodeLen, &m_stNetHead, &stRequestMsg);
    if (iRet != 0)
    {
        return -1;
    }

    // ����������
    IHandler* pHandler = NULL;
    if (enMsgPeer == GAME_SERVER_LOTUSREGAUTH)
    {
        pHandler = RegAuthHandlerSet->GetHandler(stRequestMsg.m_stmsghead().m_uimsgid(), EKMT_CLIENT);
        unsigned  int uiSessionID = ntohl(m_stNetHead.m_uiSocketFD);
        uiSessionID += iInstance * MAX_FD_NUMBER;
        m_stNetHead.m_uiSocketFD = htonl(uiSessionID);
    }
    else
    {
        pHandler = RegAuthHandlerSet->GetHandler(stRequestMsg.m_stmsghead().m_uimsgid(), EKMT_SERVER);
    }

    if (!pHandler)
    {
        TRACESVR("Failed to find a message handler, msg id: %u\n", stRequestMsg.m_stmsghead().m_uimsgid());
        return -1;
    }

    pHandler->OnClientMsg(&m_stNetHead, &stRequestMsg, NULL);

    return 0;
}

void CRegAuthApp::ClearCache()
{
    // TRACESVR("clear cache...\n");

    time_t stClearTime = time(NULL) - MAX_LIVE_TIME_OF_CACHE;

    // ����session����
    SessionManager->ClearCache(stClearTime);
}
