#ifndef __DELETE_ACCOUNT_HANDLER_HPP__
#define __DELETE_ACCOUNT_HANDLER_HPP__

#include "DBClientWrapper.hpp"
#include "Handler.hpp"
#include "AccountDBPublic.hpp"

class CAccountDBHandlerSet;

// MSGID_ACCOUNTDB_DELETE_REQUEST ��Ϣ������
class CDeleteAccountHandler : public CHandler
{
private:
    DBClientWrapper* m_pDatabase;   // �������ݿ��ָ��
    GameProtocolMsg* m_pstRequestMsg; // ���������Ϣ

	int m_iThreadIdx;

    //���ɵ�SQL���
    static char m_szQueryString[GameConfig::ACCOUNT_TABLE_SPLIT_FACTOR][256];

public:
	void SetThreadIdx(const int iThreadIdx){m_iThreadIdx = iThreadIdx;}

private:
    // ����Ķ���ֻ����CAccountDBHandlerSet���д���
    friend class CAccountDBHandlerSet;
    CDeleteAccountHandler(DBClientWrapper* pDatabase);

public:
    virtual void OnClientMsg(GameProtocolMsg* pstRequestMsg, SHandleResult* pstHandleResult);

private:

    //ɾ��Account�ʺ�
    int DeleteAccount(const AccountID& stAccountID);

    void FillFailedResponse(const unsigned int uiResultID, GameProtocolMsg* pstResponseMsg);
    void FillSuccessfulResponse(GameProtocolMsg* pstResponseMsg);
};

#endif
