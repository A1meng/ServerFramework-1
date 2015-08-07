#ifndef __CREATE_ROLE_RESPONSE_HANDLER_HPP__
#define __CREATE_ROLE_RESPONSE_HANDLER_HPP__

#include <string>

#include "Handler.hpp"

class CAccountHandlerSet;

// ��������WorldServer��MSGID_ACCOUNT_CREATEROLE_RESPONSE��Ϣ
class CCreateRoleResponseHandler : public IHandler
{
private:
    GameProtocolMsg* m_pstRequestMsg; // ���������Ϣ

    // ���������������ڱ�ʶһ��session
    unsigned int m_uiSessionFD;
    unsigned short m_unValue;

private:
    // ����Ķ���ֻ����CAccountHandlerSet���д���
    friend class CAccountHandlerSet;
    CCreateRoleResponseHandler();

public:
    virtual void OnClientMsg(TNetHead_V2* pstNetHead, GameProtocolMsg* pstMsg,
                             SHandleResult* pstResult);
private:

    //����ɾ�����ֵ������NameDB
    void SendDeleteNameRequestToWorld(const std::string& strName);

    void SendCreateRoleResponseToLotus();
};

#endif // __CREATE_ROLE_RESPONSE_HANDLER_HPP__

