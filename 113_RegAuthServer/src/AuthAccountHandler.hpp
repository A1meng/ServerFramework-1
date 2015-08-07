
#ifndef __AUTH_ACCOUNT_HANDLER_HPP__
#define __AUTH_ACCOUNT_HANDLER_HPP__

#include "Handler.hpp"

//��ҵ�¼��֤����Ϣ����

class CRegAuthHandlerSet;

// ��������LotusServer��MSGID_AUTHACCOUNT_REQUEST��Ϣ
class CAuthAccountHandler : public IHandler
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
    CAuthAccountHandler();

public:
    virtual void OnClientMsg(TNetHead_V2* pstNetHead, GameProtocolMsg* pstMsg, SHandleResult* pstResult);

private:
    int OnRequestAuthAccount();

private:

    //������֤ʧ�ܵĻظ���LotusServer
    void SendFailedResponseToLotus(unsigned int uiResult);

    int CheckParam();
};

#endif // __AUTH_ACCOUNT_HANDLER_HPP__

