#ifndef __ROLE_DB_HANDLER_SET_HPP__
#define __ROLE_DB_HANDLER_SET_HPP__

#include "HandlerSet.hpp"
#include "AddAccountHandler.hpp"
#include "DeleteAccountHandler.hpp"
#include "FetchAccountHandler.hpp"
#include "UpdateAccountHandler.hpp"

// AccountDBӦ���е���Ϣ�����߹�����
class CAccountDBHandlerSet : public CHandlerSet
{
private:
    // �ü��Ϲ����������Ϣ������
    CAddAccountHandler m_stAddAccountHandler;
    CDeleteAccountHandler m_stDeleteAccountHandler;
    CFetchAccountHandler m_stFetchAccountHandler;
    CUpdateAccountHandler m_stUpdateAccountHandler;

private:
    int RegisterAllHandlers();
    int OpenAccountDB();

private:
    // ��Ϣ�����ߴ�����Ϣʱ��Ҫ�������ݿ�
    DBClientWrapper m_oDBClient;

	int m_iThreadIdx;

public:
    CAccountDBHandlerSet();
    ~CAccountDBHandlerSet();

    // ��ʼ���ü����е���Ϣ�����ߺ����ݿ����ò��������ݿ�
    int Initialize(const int iThreadIdx);
};

#endif // __ROLE_DB_HANDLER_SET_HPP__
