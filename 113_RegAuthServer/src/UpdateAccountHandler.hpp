#ifndef __UPDATE_ACCOUNT_HANDLER_HPP__
#define __UPDATE_ACCOUNT_HANDLER_HPP__

#include "Handler.hpp"

class CRegAuthHandlerSet;

//�������ƽ̨�˻������Ϣ

// ��������LotusServer�� MSGID_REGAUTH_UPDATE_REQUEST ��Ϣ
class CUpdateAccountHandler : public IHandler
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
    CUpdateAccountHandler();

public:
    virtual void OnClientMsg(TNetHead_V2* pstNetHead, GameProtocolMsg* pstMsg, SHandleResult* pstResult);

private:
    int OnRequestUpdateAccount();
    int OnResponseUpdateAccount();

private:
    void SendUpdateRequestToAccountDB();
    void SendFailedResponseToLotus(const unsigned int uiResultID);

    int CheckParam();
};

#endif // __UPDATE_ACCOUNT_HANDLER_HPP__

