#ifndef __ACCOUNT_DB_ADD_HANDLER_HPP__
#define __ACCOUNT_DB_ADD_HANDLER_HPP__

#include "Handler.hpp"

class CRegAuthHandlerSet;

// ��������AccountDB Server�� MSGID_ACCOUNTDB_ADDACCOUNT_RESPONSE ��Ϣ
class CAccountDBAddHandler : public IHandler
{
private:
    TNetHead_V2* m_pstNetHead;  // �ͻ�����
    GameProtocolMsg* m_pstRequestMsg; // ���������Ϣ

    // ���������������ڱ�ʶһ��session
    unsigned int m_uiSessionFD;
    unsigned short m_unValue;

private:
    // ����Ķ���ֻ����CRegAuthHandlerSet���д���
    friend class CRegAuthHandlerSet;
    CAccountDBAddHandler();

public:
    virtual void OnClientMsg(TNetHead_V2* pstNetHead, GameProtocolMsg* pstMsg, SHandleResult* pstResult);

    //�������ʺ�
    static void AddAccount(unsigned uiSessionFd, const AccountID& stAccountID, int iWorldID, const std::string& strPasswd);

private:
    int OnResponseAccountDBAdd();

private:

    void SendResponseToLotus(unsigned int uiSessionFd, const unsigned int uiResultID);
};

#endif // __ACCOUNT_DB_ADD_HANDLER_HPP__

