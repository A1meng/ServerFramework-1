#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <arpa/inet.h>

#include "GameProtocol.hpp"
#include "AppDef.hpp"
#include "NowTime.hpp"
#include "LogAdapter.hpp"
#include "FixedHashCache.hpp"
#include "AccountSingleton.hpp"
#include "AppUtility.hpp"
#include "CreateRoleRequestObj.hpp"
#include "AccountOssLog.hpp"

#include "AccountApp.hpp"

int CAccountApp::ms_iAppCmd = APPCMD_NOTHING_TODO;
int CAccountApp::m_iWorldID;

CAccountApp::CAccountApp()
{
    m_bResumeMode = false;
    memset(&m_stNetHead, 0, sizeof(TNetHead_V2));
    m_uiLoopTimes = 0;
}

CAccountApp::~CAccountApp()
{
}

int CAccountApp::Initialize(bool bResume, int iWorldID)
{
    g_pAccountApp = this;

    // ��ȡ����
    LoadConfig();

    m_bResumeMode = bResume;

    int iRes = AccountMsgTransceiver->Initialize(bResume, iWorldID);
    if (iRes < 0)
    {
        return -1;
    }

    iRes = AccountObjectAllocator->Initialize(bResume);
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

    iRes = AccountHandlerSet->Initialize();
    if (iRes < 0)
    {
        return -1;
    }

    iRes = SessionManager->Initialize(bResume);
    if (iRes < 0)
    {
        return -1;
    }

    //todo jasonxiong ����������Ҫ������Ҫ������ּ�鹦��

    // ��ȡAccount Server������
    iRes = ConfigMgr->LoadAllConf();
    if (iRes != 0)
    {
        TRACESVR("failed to load config\n");
        return -1;
    }

    m_iWorldID = iWorldID;

    return 0;
}

void CAccountApp::Run()
{
    TRACESVR("AccountServer starts running ...\n");

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

            for (int i = 0; i < AccountMsgTransceiver->GetCodeQueueNum(); i++)
            {
                iRes = HandleMsgIn(CSProtocolEngine, GAME_SERVER_LOTUSACCOUNT, i);
                if (iRes == 0)
                {
                    iNewMsgCount++;
                }
            }

            iRes = HandleMsgIn(SSProtocolEngine, GAME_SERVER_WORLD);
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

void CAccountApp::SetAppCmd(int iAppCmd)
{
    ms_iAppCmd = iAppCmd;
}

int CAccountApp::ReloadConfig()
{
    // StarAccountServer.tcm
    CAppUtility::LoadLogConfig(APP_CONFIG_FILE, "AccountServer");

    // OssLogConfig.xml
    CAccountOssLog::Initialize();

    // WhiteList.ini
    // zone_conf.txt
    ConfigMgr->LoadAllConf();

    //todo jasonxiong ����������Ҫ��Ӷ����ֿ������load
    //CStringUtility::ForceUpdateDirtyWorld();

    return 0;
}

int CAccountApp::LoadConfig()
{
    // StarAccountServer.tcm
    CAppUtility::LoadLogConfig(APP_CONFIG_FILE, "AccountServer");

    // OssLogConfig.xml
    CAccountOssLog::Initialize();

    return 0;
}

int CAccountApp::HandleMsgIn(IProtocolEngine* pProtocolEngine, EGameServerID enMsgPeer, int iInstance)
{
    ASSERT_AND_LOG_RTN_INT(pProtocolEngine);

    unsigned char szCodeBuf[MAX_CODE_LEN];
    int iBufLen = sizeof(szCodeBuf);
    int iCodeLen;
    static GameProtocolMsg stRequestMsg;

    // ��ʼ��code buffer��code length
    memset(szCodeBuf, 0, sizeof(szCodeBuf));
    iCodeLen = 0;

    // ������������
    int iRet = AccountMsgTransceiver->RecvOneMsg((char*)szCodeBuf, iBufLen, iCodeLen, enMsgPeer, iInstance);
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
    if (enMsgPeer == GAME_SERVER_LOTUSACCOUNT)
    {
        pHandler = AccountHandlerSet->GetHandler(stRequestMsg.m_stmsghead().m_uimsgid(), EKMT_CLIENT);
        unsigned  int uiSessionID = ntohl(m_stNetHead.m_uiSocketFD);
        uiSessionID += iInstance * MAX_FD_NUMBER;
        m_stNetHead.m_uiSocketFD = htonl(uiSessionID);
    }
    else
    {
        pHandler = AccountHandlerSet->GetHandler(stRequestMsg.m_stmsghead().m_uimsgid(), EKMT_SERVER);
    }

    if (!pHandler)
    {
        TRACESVR("Failed to find a message handler, msg id: %u\n", stRequestMsg.m_stmsghead().m_uimsgid());
        return -1;
    }

    pHandler->OnClientMsg(&m_stNetHead, &stRequestMsg, NULL);

    return 0;
}

void CAccountApp::ClearCache()
{
    // TRACESVR("clear cache...\n");

    time_t stClearTime = time(NULL) - MAX_LIVE_TIME_OF_CACHE;

    // ����session����
    SessionManager->ClearCache(stClearTime);

    // ����CreateRoleRequestObj����
    CFixedHashCache<CCreateRoleRequestObj>::ClearCache(stClearTime);
}

void CAccountApp::TestClearCache()
{
    time_t tmNow;
    TNetHead_V2 stNetHead;
    CreateRole_Account_Request stRequest;
    char szTime[32] = "";

    for (int i = 0; i < 10; i++)
    {
        tmNow = time(NULL); // �����������ʱ��

        stNetHead.m_uiSocketFD = htonl(i);
        CSessionObj* pSessionObj = SessionManager->CreateSession(stNetHead);
        ASSERT_AND_LOG_RTN_VOID(pSessionObj);
        pSessionObj->SetCreatedTime(&tmNow);

        pSessionObj->GetCreatedTime(szTime, sizeof(szTime));
        TRACESVR("session cache created time: %s", szTime); // ����: Wed Sep 29 10:59:46 2010

        stRequest.set_uin(i+100);
        CCreateRoleRequestObj* pRequestObj = CFixedHashCache<CCreateRoleRequestObj>::CreateByUin(stRequest.uin());
        ASSERT_AND_LOG_RTN_VOID(pRequestObj);
        pRequestObj->Initialize();
        pRequestObj->SetRequestInfo(stRequest);
        pRequestObj->SetCreatedTime(&tmNow);

        pRequestObj->GetCreatedTime(szTime, sizeof(szTime));
        TRACESVR("CreateRoleRequest cache created time: %s", szTime);

        sleep(1);
    }
}
