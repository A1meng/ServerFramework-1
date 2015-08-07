#ifndef __DELETE_ROLE_RESPONSE_HANDLER_HPP__
#define __DELETE_ROLE_RESPONSE_HANDLER_HPP__

#include "Handler.hpp"

class CAccountHandlerSet;

// ��������WorldServer��MSGID_ACCOUNT_DELETEROLE_RESPONSE��Ϣ
class CDeleteRoleResponseHandler : public IHandler
{
private:
    GameProtocolMsg* m_pstRequestMsg; // ���������Ϣ

    // ���������������ڱ�ʶһ��session
    unsigned int m_uiSessionFD;
    unsigned short m_unValue;

private:
    // ����Ķ���ֻ����CAccountHandlerSet���д���
    friend class CAccountHandlerSet;
    CDeleteRoleResponseHandler();

public:
    virtual void OnClientMsg(TNetHead_V2* pstNetHead, GameProtocolMsg* pstMsg,
                             SHandleResult* pstResult);
private:
    //void SendUpdateNicknameRequestToName();
    void SendDeleteRoleResponseToLotus();
};

#endif // __DELETE_ROLE_RESPONSE_HANDLER_HPP__

