#ifndef __ACCOUNT_DB_FETCH_HANDLER_HPP__
#define __ACCOUNT_DB_FETCH_HANDLER_HPP__

#include "Handler.hpp"

class CRegAuthHandlerSet;
struct WulinServerConfig;

// ��������AccountDB Server�� MSGID_ACCOUNTDB_FETCH_REQUEST ��Ϣ
class CAccountDBFetchHandler : public IHandler
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
    CAccountDBFetchHandler();

public:
    virtual void OnClientMsg(TNetHead_V2* pstNetHead, GameProtocolMsg* pstMsg, SHandleResult* pstResult);

    //��ȡ�ʺ���ϸ��Ϣ
    static void FetchAccount(unsigned uiSessionFd, const AccountID& stAccountID, const std::string& strPassword);

private:
    int OnResponseAccountDBFetch();

private:
    
    //������֤ʧ�ܻظ���LotusServer
    void SendAuthFailedResponseToLotus(unsigned int uiSessionFd, const unsigned int uiResultID);

    //������֤�ɹ��ظ���LotusServer
    void SendAuthSuccessResponseToLotus(unsigned int uiSessionFd, const WulinServerConfig& stServerConfig, unsigned int uin, bool bIsBinded);
};

#endif // __ACCOUNT_DB_FETCH_HANDLER_HPP__

