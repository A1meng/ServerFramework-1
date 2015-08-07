#ifndef __REGAUTH_HANDLER_SET_HPP__
#define __REGAUTH_HANDLER_SET_HPP__

#include "HandlerSet.hpp"
#include "SingletonTemplate.hpp"
#include "RegisterAccountHandler.hpp"
#include "AuthAccountHandler.hpp"
#include "UpdateAccountHandler.hpp"
#include "AccountDBAddHandler.hpp"
#include "AccountDBFetchHandler.hpp"
#include "ClientClosedHandler.hpp"

using namespace ServerLib;

// RegAuthӦ���е���Ϣ�����߹�����
class CRegAuthHandlerSet : public CHandlerSet
{
private:
    // �ü��Ϲ����������Ϣ������
    CRegisterAccountHandler* m_pRegisterAccountHandler;
    CAccountDBAddHandler* m_pAccountDBAddHandler;
    CAccountDBFetchHandler* m_pAccountDBFetchHandler;

    CAuthAccountHandler* m_pAuthAccountHandler;
    CUpdateAccountHandler* m_pUpdateAccountHandler;
    CClientClosedHandler* m_pClientClosedHandler;

private:
    // ����Ķ���ֻ����CSingleton<CRoleDBHandlerSet>���д���
    friend class CSingleton<CRegAuthHandlerSet>;
    CRegAuthHandlerSet();

private:
    // ����������handler��ע��Ҫ�������ĸ�����������Ӧ�޸�
    void InitAllHandlers();
    void FreeAllHandlers();
    int AllocAllHandlers();
    int RegisterAllHandlers();

public:
    virtual ~CRegAuthHandlerSet();

    // ��ʼ���ü����е���Ϣ������
    virtual int Initialize();
};

#endif // __REGAUTH_HANDLER_SET_HPP__

