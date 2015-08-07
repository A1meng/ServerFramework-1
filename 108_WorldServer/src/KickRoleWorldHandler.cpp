#include "ModuleHelper.hpp"
#include "WorldMsgHelper.hpp"
#include "LogAdapter.hpp"
#include "WorldRoleStatus.hpp"

#include "KickRoleWorldHandler.hpp"

CKickRoleWorldHandler::~CKickRoleWorldHandler(void)
{
}


GameProtocolMsg CKickRoleWorldHandler::ms_stGameMsg;
int CKickRoleWorldHandler::OnClientMsg(const void* pMsg)
{
    ASSERT_AND_LOG_RTN_INT(pMsg);

    m_pRequestMsg = (GameProtocolMsg*)pMsg;

    switch (m_pRequestMsg->m_stmsghead().m_uimsgid())
    {
    case MSGID_WORLD_KICKROLE_REQUEST:
        {
            OnZoneRequestKickRole();
            break;
        }

    case MSGID_WORLD_KICKROLE_RESPONSE:
        {
            OnZoneResponseKickRole();
            break;
        } 

    default:
        {
            break;
        }
    }

    return 0;
}

// ��������Zone����������
int CKickRoleWorldHandler::OnZoneRequestKickRole()
{
    const World_KickRole_Request& rstReq = m_pRequestMsg->m_stmsgbody().m_stworld_kickrole_request();

    int iRet = 0;

    // �жϱ�world�ĵ�¼���
    LOGDEBUG("Recv Zone KickRole Req: Uin = %u, From ZoneID = %d\n", rstReq.stkickedroleid().uin(), rstReq.ifromzoneid()); 

    CWorldRoleStatusWObj* pWorldRoleStatusWObj = WorldTypeK32<CWorldRoleStatusWObj>::GetByRoleID(rstReq.stkickedroleid());

    //������ڸ�world��¼, ��Zone������������
    if (pWorldRoleStatusWObj != NULL)
    {
        int iZoneID = pWorldRoleStatusWObj->GetZoneID();
        if (CModuleHelper::GetZoneTick()->IsZoneActive(iZoneID))
        {
            CWorldMsgHelper::GenerateMsgHead(ms_stGameMsg, 0, MSGID_WORLD_KICKROLE_REQUEST, rstReq.stkickedroleid().uin());

            World_KickRole_Request* pstKickReq = ms_stGameMsg.mutable_m_stmsgbody()->mutable_m_stworld_kickrole_request();
            pstKickReq->CopyFrom(rstReq);

            iRet =  CWorldMsgHelper::SendWorldMsgToWGS(ms_stGameMsg, iZoneID);
            LOGDEBUG("Send zone KickRole Req: iRet:%d Uin = %u, zoneid:%d\n",iRet, rstReq.stkickedroleid().uin(), iZoneID);
            return 0;
        }
        else
        {
            // ֱ��ɾ����uin����Ϣ�����µ�¼
            WorldTypeK32<CWorldRoleStatusWObj>::DeleteByUin(rstReq.stkickedroleid().uin());
        }
    }

    // ��Zone���ͻظ�
    CWorldMsgHelper::GenerateMsgHead(ms_stGameMsg, 0, MSGID_WORLD_KICKROLE_RESPONSE, rstReq.stkickedroleid().uin());
    World_KickRole_Response* pstKickResp = ms_stGameMsg.mutable_m_stmsgbody()->mutable_m_stworld_kickrole_response();
    pstKickResp->set_ifromworldid(rstReq.ifromworldid());
    pstKickResp->set_ifromzoneid(rstReq.ifromzoneid());
    pstKickResp->set_isessionid(rstReq.isessionid());
    pstKickResp->set_bislogin(rstReq.bislogin());
    pstKickResp->mutable_stkickedroleid()->CopyFrom(rstReq.stkickedroleid());
    
    pstKickResp->set_iresult(T_SERVER_SUCESS);

    iRet = CWorldMsgHelper::SendWorldMsgToWGS(ms_stGameMsg, rstReq.ifromzoneid());

    LOGDEBUG("Send Zone KickRole Resp: Uin = %u, iRet = %d\n", rstReq.stkickedroleid().uin(), iRet);

    return 0;
}

// ��������Zone�ı��߻ظ�
int CKickRoleWorldHandler::OnZoneResponseKickRole()
{
    const World_KickRole_Response& rstResp = m_pRequestMsg->m_stmsgbody().m_stworld_kickrole_response();
    unsigned int uin = rstResp.stkickedroleid().uin();
    int iRet = -1;

    LOGDEBUG("Recv Zone KickRole Resp: Uin = %u, result = %d\n", uin, rstResp.iresult());

    // ������˳ɹ�, ���ٴ�ȷ��Worldû������. �����, ��ǿ��ɾ����������.
    if (rstResp.iresult() == T_SERVER_SUCESS)
    {
        CWorldRoleStatusWObj* pWorldRoleStatusWObj = WorldTypeK32<CWorldRoleStatusWObj>::GetByRoleID(rstResp.stkickedroleid());
        if (pWorldRoleStatusWObj)
        {
            WorldTypeK32<CWorldRoleStatusWObj>::DeleteByUin(uin);
        }
    }

    //��¼��zone�����˵�zone����һ��world
    if (rstResp.ifromworldid() == CModuleHelper::GetWorldID())
    {
        // ��Zone���ͻظ�
        CWorldMsgHelper::GenerateMsgHead(ms_stGameMsg, 0, MSGID_WORLD_KICKROLE_RESPONSE, uin);
        World_KickRole_Response* pstKickResp = ms_stGameMsg.mutable_m_stmsgbody()->mutable_m_stworld_kickrole_response();
        pstKickResp->CopyFrom(rstResp);

        iRet =CWorldMsgHelper::SendWorldMsgToWGS(ms_stGameMsg, rstResp.ifromzoneid());

        LOGDEBUG("Send Zone KickRole Resp: Uin = %u, iRet = %d\n", uin, iRet);

    }
    //��world��¼�����ذ���
    else
    {

    }

    return iRet;
}

