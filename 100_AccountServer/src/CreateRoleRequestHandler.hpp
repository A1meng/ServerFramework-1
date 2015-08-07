#ifndef __CREATE_ROLE_REQUEST_HANDLER_HPP__
#define __CREATE_ROLE_REQUEST_HANDLER_HPP__

#include "Handler.hpp"

class CAccountHandlerSet;

// ��������LotusServer��MSGID_ACCOUNT_CREATEROLE_REQUEST��Ϣ
class CCreateRoleRequestHandler : public IHandler
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
    CCreateRoleRequestHandler();

public:
    virtual void OnClientMsg(TNetHead_V2* pstNetHead, GameProtocolMsg* pstMsg,
                             SHandleResult* pstResult);

private:
    void SendAddNewNameRequestToWorld(const CreateRole_Account_Request& rstReq);
    void SendFailedResponseToLotus(const unsigned int uiResultID);
    int CheckParam();
};

#endif // __CREATE_ROLE_REQUEST_HANDLER_HPP__

