#ifndef __ACCOUNT_ADD_NEW_NAME_HANDLER_HPP__
#define __ACCOUNT_ADD_NEW_NAME_HANDLER_HPP__

#include "Handler.hpp"

class CAccountHandlerSet;

// ��������World�� MSGID_ADDNEWNAME_RESPONSE ��Ϣ
class CAccountAddNewNameHandler : public IHandler
{
private:
    // ����Ķ���ֻ����CAccountHandlerSet���д���
    friend class CAccountHandlerSet;
    CAccountAddNewNameHandler();

public:
    virtual void OnClientMsg(TNetHead_V2* pstNetHead, GameProtocolMsg* pstMsg, SHandleResult* pstResult);

private:

    //���ʹ�����ɫ������
    int SendCreateRoleRequestToWorld(const std::string& strName, unsigned int uin);

    //���ʹ�����ɫʧ�ܵ�����Lotus
    void SendCreateRoleFailedToLotus(unsigned int uin, int iResult);

    //ɾ��NameDB�е�����
    int SendDeleteNameRequestToWorld(const std::string& strName, int iType, unsigned uin);

private:
    GameProtocolMsg* m_pRequestMsg;
};

#endif // __ACCOUNT_ADD_NEW_NAME_HANDLER_HPP__
