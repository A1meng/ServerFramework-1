#ifndef __ROLE_DB_APP_HPP__
#define __ROLE_DB_APP_HPP__

#include "SSProtocolEngine.hpp"
#include "AccountDBPublic.hpp"
#include "AccountDBMsgTransceiver.hpp"
#include "AccountDBHandlerSet.hpp"
#include "AccountDBThreadManager.hpp"
#include "AccountDBLogManager.hpp"
#include "AccountDBInfoConfig.hpp"

template<typename CApp>
class CAppType
{
private:
    static CApp* m_pApp;
public:
    static int RegApp(CApp* pApp)
    {
        if (!pApp)
        {
            return -1;
        }

        m_pApp = pApp;
        return 0;
    }

    static CApp* GetApp()
    {
        return m_pApp;
    }
};

template<typename CApp>
CApp* CAppType<CApp>::m_pApp = NULL;

// AccountDBӦ�ó���
class CAccountDBApp
{
public:
    CAccountDBApp();
    virtual ~CAccountDBApp();

    int Initialize(bool bResume, int iWorldID);
    void Run();
    int HandleMsgIn(int& riRecvMsgCount);
    //process thread msg
    int HandleThreadMsg(int& riSendMsgCount);

public:
    CSSProtocolEngine* GetProtocolEngine(int iThreadIdx);

public:
    static void SetAppCmd(int iAppCmd);

private:
    //receive msg and dispatch
    int ReceiveAndDispatchMsg();

private:
    bool m_bResumeMode; // �����ڴ�ָ�ģʽ����������ϴ������������Ĺ����ڴ�δ��ɾ����ֱ��attach��ȥ
    static int ms_iAppCmd;

private:
    int LoadLogConfig(const char* pszConfigFile, const char* pszLogName);
    int ReloadConfig();
    int LoadConfig();

public:
    CServerLogAdapter* GetLogAdapter(const int iThreadIdx) {return m_stLogManager.GetLogAdapter(iThreadIdx);};

public:

    //ACCOUNTDB��ص����ݿ���Ϣ������
    static CAccountDBInfoConfig m_stAccountDBConfigManager;

    //UniqUinDB��ص����ݿ���Ϣ�����ã�����ע���ʺ�ʱ����Ψһ��UIN
    static CAccountDBInfoConfig m_stUniqUinDBConfigManager;

private:
    CAccountDBMsgTransceiver m_stMsgTransceiver;

private:
    char m_szCodeBuf[MAX_MSGBUFFER_SIZE];

private:
    CAccountDBThreadManager m_stThreadManager;
    CAccountDBLogManager m_stLogManager;
};

#endif // __ROLE_DB_APP_HPP__
