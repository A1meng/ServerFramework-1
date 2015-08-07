
#include "GameProtocol.hpp"
#include "LogAdapter.hpp"
#include "ZoneErrorNumDef.hpp"
#include "ModuleHelper.hpp"
#include "ZoneMsgHelper.hpp"

#include "RepThingsHandler.hpp"

using namespace ServerLib;

GameProtocolMsg CRepThingsHandler::ms_stZoneMsg;

CRepThingsHandler::~CRepThingsHandler()
{

}

int CRepThingsHandler::OnClientMsg()
{
    switch (m_pRequestMsg->m_stmsghead().m_uimsgid())
    {
    case MSGID_ZONE_REPOPERA_REQUEST:
        {
            OnRequestOperaRepItem();
            break;    
        }

    case MSGID_ZONE_GETREPINFO_REQUEST:
        {
            OnRequestGetRepInfo();
            break;
        }

    case MSGID_ZONE_WEAREQUIP_REQUEST:
        {
            OnRequestWearEquipment();
            break;
        }

    case MSGID_ZONE_SELLITEM_REQUEST:
        {
            OnRequestSellItem();
            break;
        }

    default:
        {
            break;
        }
    }

    return 0;
}

//��ұ�����Ʒ�Ĳ���
int CRepThingsHandler::OnRequestOperaRepItem()
{
    int iRet = SecurityCheck();
    if(iRet < 0)
    {
        LOGERROR("Security Check Failed: Uin = %u, iRet = %d\n", m_pRequestMsg->m_stmsghead().m_uin(), iRet);
        
        SendFailedResponse(MSGID_ZONE_REPOPERA_RESPONSE, T_ZONE_SECURITY_CHECK_FAILED, *m_pNetHead);

        return -1;
    }

    //��ұ�����Ʒ�Ĳ���
    const Zone_RepOpera_Request& rstRequest = m_pRequestMsg->m_stmsgbody().m_stzone_repopera_request();
    iRet = CRepThingsFramework::Instance()->OperaRepItem(m_pRequestMsg->m_stmsghead().m_uin(), rstRequest);
    if(iRet)
    {
        LOGERROR("Failed to opera rep item, uin %u, ret %d\n", m_pRequestMsg->m_stmsghead().m_uin(), iRet);
        SendFailedResponse(MSGID_ZONE_REPOPERA_RESPONSE, iRet, *m_pNetHead);
        return -2;
    }

    //����ɹ���Framework�з��أ��˴�����Ҫ���ذ�

    return T_SERVER_SUCESS;
}

//�����ȡ��������ϸ��Ϣ
int CRepThingsHandler::OnRequestGetRepInfo()
{
    int iRet = SecurityCheck();
    if(iRet < 0)
    {
        LOGERROR("Security Check Failed: Uin = %u, iRet = %d\n", m_pRequestMsg->m_stmsghead().m_uin(), iRet);
        
        SendFailedResponse(MSGID_ZONE_GETREPINFO_RESPONSE, T_ZONE_SECURITY_CHECK_FAILED, *m_pNetHead);

        return -1;
    }

    //��ȡ��ҵı�����Ʒ��Ϣ
    const Zone_GetRepInfo_Request& rstRequest = m_pRequestMsg->m_stmsgbody().m_stzone_getrepinfo_request();
    iRet = CRepThingsFramework::Instance()->GetRepItemInfo(m_pRequestMsg->m_stmsghead().m_uin(), rstRequest);
    if(iRet)
    {
        LOGERROR("Failed to process get rep info, uin %u, ret %d\n", m_pRequestMsg->m_stmsghead().m_uin(), iRet);
        SendFailedResponse(MSGID_ZONE_GETREPINFO_RESPONSE, iRet, *m_pNetHead);
        return -2;
    }

    //����ɹ���Framework�з��أ��˴�����Ҫ���ذ�

    return T_SERVER_SUCESS;
}

//��Ҵ���ս����λװ���Ĳ���
int CRepThingsHandler::OnRequestWearEquipment()
{
    int iRet = SecurityCheck();
    if(iRet < 0)
    {
        LOGERROR("Security Check Failed: Uin = %u, iRet = %d\n", m_pRequestMsg->m_stmsghead().m_uin(), iRet);
        
        SendFailedResponse(MSGID_ZONE_WEAREQUIP_RESPONSE, T_ZONE_SECURITY_CHECK_FAILED, *m_pNetHead);

        return -1;
    }

    //��Ҵ���ս��װ���Ĳ���
    const Zone_WearEquip_Request& rstRequest = m_pRequestMsg->m_stmsgbody().m_stzone_wearequip_request();
    iRet = CRepThingsFramework::Instance()->WearEquipment(m_pRequestMsg->m_stmsghead().m_uin(), rstRequest);
    if(iRet)
    {
        LOGERROR("Failed to process wear equipment, uin %u, ret %d\n", m_pRequestMsg->m_stmsghead().m_uin(), iRet);
        SendFailedResponse(MSGID_ZONE_WEAREQUIP_RESPONSE, iRet, *m_pNetHead);
        return -2;
    }

    //����ɹ���Framework�з��أ��˴�����Ҫ���ذ�

    return T_SERVER_SUCESS;
}

//��ҳ��۱�����Ʒ
int CRepThingsHandler::OnRequestSellItem()
{
    int iRet = SecurityCheck();
    if(iRet < 0)
    {
        LOGERROR("Security Check Failed: Uin = %u, iRet = %d\n", m_pRequestMsg->m_stmsghead().m_uin(), iRet);
        
        SendFailedResponse(MSGID_ZONE_SELLITEM_RESPONSE, T_ZONE_SECURITY_CHECK_FAILED, *m_pNetHead);

        return -1;
    }

    //��ҳ��۱�����Ʒ
    const Zone_SellItem_Request& rstRequest = m_pRequestMsg->m_stmsgbody().m_stzone_sellitem_request();
    iRet = CRepThingsFramework::Instance()->SellRepItem(m_pRequestMsg->m_stmsghead().m_uin(), rstRequest);
    if(iRet)
    {
        LOGERROR("Failed to process sell item, uin %u, ret %d\n", m_pRequestMsg->m_stmsghead().m_uin(), iRet);
        SendFailedResponse(MSGID_ZONE_SELLITEM_RESPONSE, iRet, *m_pNetHead);
        return -2;
    }

    //����ɹ���Framework�з��أ��˴�����Ҫ���ذ�

    return T_SERVER_SUCESS;
}

// ����ʧ�ܻظ�
int CRepThingsHandler::SendFailedResponse(unsigned uiMsgID, const unsigned int uiResultID, const TNetHead_V2& rstNetHead)
{
    CZoneMsgHelper::GenerateMsgHead(ms_stZoneMsg, uiMsgID);

    switch (uiMsgID)
    {
    case MSGID_ZONE_REPOPERA_RESPONSE:
        {
            ms_stZoneMsg.mutable_m_stmsgbody()->mutable_m_stzone_repopera_response()->set_iresult(uiResultID);
        }
        break;

    case MSGID_ZONE_GETREPINFO_RESPONSE:
        {
            ms_stZoneMsg.mutable_m_stmsgbody()->mutable_m_stzone_getrepinfo_response()->set_iresult(uiResultID);
        }
        break;

    case MSGID_ZONE_WEAREQUIP_RESPONSE:
        {
            ms_stZoneMsg.mutable_m_stmsgbody()->mutable_m_stzone_wearequip_response()->set_iresult(uiResultID);
        }
        break;

    case MSGID_ZONE_SELLITEM_RESPONSE:
        {
            ms_stZoneMsg.mutable_m_stmsgbody()->mutable_m_stzone_sellitem_response()->set_iresult(uiResultID);
        }
        break;

    default:
        {
            return -1;
        }
        break;
    }

    CZoneMsgHelper::SendZoneMsgToClient(ms_stZoneMsg, rstNetHead);

    return 0;
}

