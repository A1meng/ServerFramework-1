#ifndef __ACCOUNT_APP_HPP__
#define __ACCOUNT_APP_HPP__

#include "App.hpp"
#include "CodeQueue.hpp"
#include "ProtocolEngine.hpp"
#include "AccountPublic.hpp"
#include "SessionManager.hpp"

using namespace ServerLib;

// AccountӦ�ó���
class CAccountApp : public CApp
{
private:
    TNetHead_V2 m_stNetHead;

private:
    static const unsigned int MAIN_LOOP_SLEEP = 10; // ��ѭ����ÿ��ѭ�����˯��10΢��
    static const unsigned int TIMES_TO_CLEAR_CACHE = 10000; // ��ѭ��1�������һ�γ�ʱ�Ļ�����
    unsigned int m_uiLoopTimes; // ��ѭ������

    CSessionManager m_stSessionManager;

public:
    CSessionManager* GetSessionManager() {return &m_stSessionManager;}

public:
    CAccountApp();
    virtual ~CAccountApp();

    virtual int Initialize(bool bResume, int iWorldID);
    virtual void Run();

private:
    virtual int HandleMsgIn(IProtocolEngine* pProtocolEngine, EGameServerID enMsgPeer, int iInstance = 0);

private:
    bool m_bResumeMode; // �����ڴ�ָ�ģʽ����������ϴ������������Ĺ����ڴ�δ��ɾ����ֱ��attach��ȥ
    static int ms_iAppCmd;
    static int m_iWorldID;

public:
    static void SetAppCmd(int iAppCmd);
    static int GetWorldID() {return m_iWorldID;}

private:
    virtual int ReloadConfig();
    virtual int LoadConfig();

public:
    void ClearCache(); // ����ʱ��cache���

private:
    void TestClearCache();
};

#endif // __ACCOUNT_APP_HPP__

