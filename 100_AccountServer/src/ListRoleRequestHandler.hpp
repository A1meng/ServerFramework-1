#ifndef __LIST_ROLE_REQUEST_HANDLER_HPP__
#define __LIST_ROLE_REQUEST_HANDLER_HPP__

#include "Handler.hpp"

class CAccountHandlerSet;

// ��������LotusServer��MSGID_ACCOUNT_LISTROLE_REQUEST��Ϣ
class CListRoleRequestHandler : public IHandler
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
    CListRoleRequestHandler();

public:
    virtual void OnClientMsg(TNetHead_V2* pstNetHead, GameProtocolMsg* pstMsg,
                             SHandleResult* pstResult);

private:
    void SendListRoleRequestToWorld();
    void SendResponseToLotus(const unsigned int uiResultID);
    int CheckParam(unsigned int& rushResultID);
};

#endif // __LIST_ROLE_REQUEST_HANDLER_HPP__

