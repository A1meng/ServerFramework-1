#ifndef __ACCOUNT_HANDLER_SET_HPP__
#define __ACCOUNT_HANDLER_SET_HPP__

#include "HandlerSet.hpp"
#include "SingletonTemplate.hpp"
#include "CreateRoleRequestHandler.hpp"
#include "CreateRoleResponseHandler.hpp"
#include "DeleteRoleRequestHandler.hpp"
#include "DeleteRoleResponseHandler.hpp"
#include "ListRoleRequestHandler.hpp"
#include "ListRoleResponseHandler.hpp"
#include "ClientClosedHandler.hpp"
#include "AccountAddNewNameHandler.hpp"
#include "ListZoneHandler.hpp"

using namespace ServerLib;

// AccountӦ���е���Ϣ�����߹�����
class CAccountHandlerSet : public CHandlerSet
{
private:
    // �ü��Ϲ����������Ϣ������
    CCreateRoleRequestHandler*  m_pCreateRoleRequestHandler;
    CCreateRoleResponseHandler* m_pCreateRoleResponseHandler;
    CDeleteRoleRequestHandler* m_pDeleteRoleRequestHandler;
    CDeleteRoleResponseHandler* m_pDeleteRoleResponseHandler;
    CListRoleRequestHandler* m_pListRoleRequestHandler;
    CListRoleResponseHandler* m_pListRoleResponseHandler;
    CClientClosedHandler* m_pClientClosedHandler;
    CAccountAddNewNameHandler* m_pAddNewNameHandler;
    CListZoneHandler* m_pListZoneHandler;

private:
    // ����Ķ���ֻ����CSingleton<CRoleDBHandlerSet>���д���
    friend class CSingleton<CAccountHandlerSet>;
    CAccountHandlerSet();

private:
    // ����������handler��ע��Ҫ�������ĸ�����������Ӧ�޸�
    void InitAllHandlers();
    void FreeAllHandlers();
    int AllocAllHandlers();
    int RegisterAllHandlers();

public:
    virtual ~CAccountHandlerSet();

    // ��ʼ���ü����е���Ϣ������
    virtual int Initialize();
};

#endif // __ACCOUNT_HANDLER_SET_HPP__

