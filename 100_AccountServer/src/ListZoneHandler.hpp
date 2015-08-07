
#ifndef __LIST_ZONE_HANDLER_HPP__
#define __LIST_ZONE_HANDLER_HPP__

#include "Handler.hpp"

class CAccountHandlerSet;

// ��ȡ����������ZoneServer����ϸ��Ϣ
class CListZoneHandler : public IHandler
{
private:
    TNetHead_V2* m_pstNetHead;
    GameProtocolMsg* m_pstRequestMsg; // ���������Ϣ

private:
    // ����Ķ���ֻ����CAccountHandlerSet���д���
    friend class CAccountHandlerSet;
    CListZoneHandler();

public:
    virtual void OnClientMsg(TNetHead_V2* pstNetHead, GameProtocolMsg* pstMsg,
                             SHandleResult* pstResult);
private:

    //��ListZone������ת����WorldServer
    void SendListZoneRequestToWorld();

    //��ListZone�ķ���ת����Client
    void SendListZoneResponseToLotus();
};

#endif

