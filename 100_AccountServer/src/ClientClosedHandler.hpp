#ifndef __CLIENT_CLOSED_HANDLER_HPP__
#define __CLIENT_CLOSED_HANDLER_HPP__

#include "Handler.hpp"

class CAccountHandlerSet;

// ��������LotusServer��MSGID_LOGOUTSERVER_REQUEST��Ϣ
class CClientClosedHandler : public IHandler
{
private:
    // ����Ķ���ֻ����CAccountHandlerSet���д���
    friend class CAccountHandlerSet;
    CClientClosedHandler();

public:
    virtual void OnClientMsg(TNetHead_V2* pstNetHead, GameProtocolMsg* pstMsg,
                             SHandleResult* pstResult);
};

#endif // __CLIENT_CLOSED_HANDLER_HPP__
