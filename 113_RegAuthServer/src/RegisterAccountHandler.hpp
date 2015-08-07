#ifndef __REGISTER_ACCOUNT_HANDLER_HPP__
#define __REGISTER_ACCOUNT_HANDLER_HPP__

#include "Handler.hpp"

class CRegAuthHandlerSet;

// ��������LotusServer�� MSGID_REGAUTH_REGACCOUNT_REQUEST ��Ϣ
class CRegisterAccountHandler : public IHandler
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
    CRegisterAccountHandler();

public:
    virtual void OnClientMsg(TNetHead_V2* pstNetHead, GameProtocolMsg* pstMsg,
                             SHandleResult* pstResult);

private:
    int OnRequestRegAccount();

private:

    void SendFailedResponseToLotus(const unsigned int uiResultID);

    int CheckParam();
};

#endif // __CREATE_ROLE_REQUEST_HANDLER_HPP__

