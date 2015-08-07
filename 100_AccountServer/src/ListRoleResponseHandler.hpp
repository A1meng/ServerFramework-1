#ifndef __LIST_ROLE_RESPONSE_HANDLER_HPP__
#define __LIST_ROLE_RESPONSE_HANDLER_HPP__

#include "Handler.hpp"

class CAccountHandlerSet;

// ��������WorldServer��MSGID_ACCOUNT_LISTROLE_RESPONSE��Ϣ
class CListRoleResponseHandler : public IHandler
{
private:
    GameProtocolMsg* m_pstRequestMsg; // ���������Ϣ

    // ���������������ڱ�ʶһ��session
    unsigned int m_uiSessionFD;
    unsigned short m_unValue;

private:
    // ����Ķ���ֻ����CAccountHandlerSet���д���
    friend class CAccountHandlerSet;
    CListRoleResponseHandler();

public:
    virtual void OnClientMsg(TNetHead_V2* pstNetHead, GameProtocolMsg* pstMsg,
                             SHandleResult* pstResult);
private:

    //void SendUpdateNicknameRequestToName();
    void SendListRoleResponseToLotus();
};

#endif // __LIST_ROLE_RESPONSE_HANDLER_HPP__

