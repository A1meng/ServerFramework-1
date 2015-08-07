#ifndef __NAME_DB_APP_HPP__
#define __NAME_DB_APP_HPP__

#include "SSProtocolEngine.hpp"
#include "NameDBPublic.hpp"
#include "NameDBMsgTransceiver.hpp"
#include "NameDBHandlerSet.hpp"
#include "NameDBThreadManager.hpp"
#include "NameDBLogManager.hpp"
#include "NameDBInfoConfig.hpp"

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

// NameDBӦ�ó���
class CNameDBApp
{
public:
    CNameDBApp();
    virtual ~CNameDBApp();

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

    //NAMEDB��ص����ݿ���Ϣ������
    static CNameDBInfoConfig m_stNameDBConfigManager;

private:
    CNameDBMsgTransceiver m_stMsgTransceiver;

private:
    char m_szCodeBuf[MAX_MSGBUFFER_SIZE];

private:
    CNameDBThreadManager m_stThreadManager;
    CNameDBLogManager m_stLogManager;
};

#endif // __NAME_DB_APP_HPP__
