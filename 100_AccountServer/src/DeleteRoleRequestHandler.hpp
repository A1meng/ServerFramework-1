#ifndef __DELETE_ROLE_REQUEST_HANDLER_HPP__
#define __DELETE_ROLE_REQUEST_HANDLER_HPP__

#include "Handler.hpp"

class CAccountHandlerSet;

// ��������LotusServer��MSGID_ACCOUNT_DELETEROLE_REQUEST��Ϣ
class CDeleteRoleRequestHandler : public IHandler
{
private:
    TNetHead_V2* m_pstNetHead;  // �ͻ�����
    GameProtocolMsg* m_pstRequestMsg; // ���������Ϣ

    // ���������������ڱ�ʶһ��session
    unsigned int m_uiSessionFD;
    unsigned short m_unValue;

private:
    // ����Ķ���ֻ����CAccountHandlerSet���д���
    friend class CAccountHandlerSet;
    CDeleteRoleRequestHandler();

public:
    virtual void OnClientMsg(TNetHead_V2* pstNetHead, GameProtocolMsg* pstMsg,
                             SHandleResult* pstResult);

private:
    void SendDeleteRoleRequestToWorld();
    void SendFailedResponseToLotus(const unsigned int uiResultID);
    int CheckParam();
};

#endif // __DELETE_ROLE_REQUEST_HANDLER_HPP__

