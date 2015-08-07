
#include "AppLoop.hpp"
#include "LogAdapter.hpp"
#include "HandlerHelper.hpp"
#include "ZoneMsgHelper.hpp"
#include "LogoutHandler.hpp"
#include "UnitUtility.hpp"

#include "KickRoleWorldHandler.hpp"

int CalcForbidTime(int iForbidType, int iIsForbidden)
{
	//��Ӧ���ʱ��
	//���÷����Ϊ2��
	int aiForbidTime[] = {0, 3600, 3*3600, 5*3600, 24*3600, 2*365*3600};
	int tForbidTime = 0;	
	//���
	if (iIsForbidden)
	{
		if ((iForbidType < 0))
		{
			TRACESVR("unknown forbidden tyep:%d\n", iForbidType);
			tForbidTime = 0;
		}
		else
		{
			tForbidTime = time(NULL) + aiForbidTime[iForbidType];
		}
	}
	//���
	else
	{
		tForbidTime = 0;
	}
	return tForbidTime;
}

CKickRoleWorldHandler::~CKickRoleWorldHandler()
{
}

GameProtocolMsg CKickRoleWorldHandler::ms_stZoneMsg;
int CKickRoleWorldHandler::OnClientMsg()
{
    switch (m_pRequestMsg->m_stmsghead().m_uimsgid())
    {
    case MSGID_WORLD_KICKROLE_REQUEST:
        {
            OnRequestKickRole();
            break;
        }

    case MSGID_WORLD_KICKROLE_RESPONSE:
        {
            OnResponseKickRole();
            break;
        }

    default:
        {
            break;
        }
    }

    return 0;
}

// �յ�World�ı�������֪ͨ
int CKickRoleWorldHandler::OnRequestKickRole()
{
	const World_KickRole_Request& rstReq = m_pRequestMsg->m_stmsgbody().m_stworld_kickrole_request();
	const unsigned int uiKickedUin = rstReq.stkickedroleid().uin();

    LOGDEBUG("Recv KickRole Req: Uin = %u\n", uiKickedUin);

    CGameRoleObj* pRoleObj = CUnitUtility::GetRoleByUin(uiKickedUin);
    // �Ҳ������, ���سɹ�
    if (!pRoleObj)
    {
        LOGERROR("KickRole OK: Uin = %u\n", uiKickedUin);

        SendSuccessfullResponse(rstReq);
        return 0;
    }

	//����������
	pRoleObj->SetKicker(rstReq);

    // ����������� 
    int iRet = CLogoutHandler::LogoutRole(pRoleObj, LOGOUT_REASON_KICKOFF);
    if (iRet < 0)
    {
        LOGERROR("KickRole Failed: Uin = %u\n", uiKickedUin);
        
        SendFailedResponse(rstReq);
        return iRet;
    }
     
    pRoleObj->SetKicker(rstReq);

    return iRet;
}

int CKickRoleWorldHandler::OnForbidKickRole()
{
	return 0;

	/*
	FORBIDROLEREQUEST &rstReq = m_pRequestMsg->m_stMsgBody.m_stForbidRoleRequest; 
	ROLEID stRoleID = rstReq.m_stRoleID;
	
	CGameRoleObj* pRoleObj = CUnitUtility::GetRoleByUin(stRoleID.m_uiUin);
	LOGDEBUG("Recv Forbid Kick Role");

	if (pRoleObj == NULL)
	{
		TRACESVR("not online\n");
		return 0;
	}
	
	if (pRoleObj->GetRoleID().m_uiSeq != stRoleID.m_uiSeq)
	{
		TRACESVR("not online\n");
		return 0;
	}

	//todo���������ʾ
	CPromptMsgHelper::SendPromptMsgToRole(pRoleObj, 
			EQEC_Kick_Reason, PROMPTMSG_TYPE_POPUP, rstReq.m_szForbidReason);
	
	//����T��ʱ��
	pRoleObj->SetForbidTime(CalcForbidTime(rstReq.m_cForbidType, rstReq.m_cIsForbidden));

    // ����������� 
    int iRet = CLogoutHandler::LogoutRole(pRoleObj, EQEC_Logout_Kickoff);
    if (iRet < 0)
    {
        TRACESVR("KickRole Failed: Uin = %u\n", stRoleID.m_uiUin);
    }

	return iRet;
	*/
}

// �յ�World���������߻ظ�
int CKickRoleWorldHandler::OnResponseKickRole()
{
	World_KickRole_Response* pstKickRoleResp = m_pRequestMsg->mutable_m_stmsgbody()->mutable_m_stworld_kickrole_response();
	unsigned int uiUin = pstKickRoleResp->stkickedroleid().uin();

	LOGDEBUG("Recv KickRole Resp: Uin = %u, iRet = %d\n", uiUin, pstKickRoleResp->iresult());

	if (pstKickRoleResp->ifromworldid() != CModuleHelper::GetWorldID()
		|| pstKickRoleResp->ifromzoneid() != CModuleHelper::GetZoneID())
	{
		LOGERROR("Not From Mine!\n");
		return -1;
	}

    // �ҵ�Session
    CGameSessionObj *pSession = CModuleHelper::GetSessionManager()->FindSessionByID(pstKickRoleResp->isessionid());
    if (!pSession)
    {
		LOGERROR("Cannot find Session: ID = %d\n", pstKickRoleResp->isessionid());
        return -1;
    }

	// ȷ��Sessionδ��¼
	if (pSession->GetBindingRole() != NULL)
	{
		LOGERROR("Session Already Binding Role!\n");
		return -2;
	}

    if (pstKickRoleResp->iresult() != T_SERVER_SUCESS)
    {
		if (pstKickRoleResp->bislogin())
		{
            //����ǵ�¼
			CLoginHandler::LoginFailed(pSession->GetNetHead());
			CModuleHelper::GetSessionManager()->DeleteSession(pSession->GetID());
		}

		LOGERROR("Kick fail: %u, %d\n", uiUin, pstKickRoleResp->iresult());
        return -3;
    }

    // ������˳ɹ�, ���ٴ�ȷ��Zoneû������
    CGameRoleObj* pRoleObj = CUnitUtility::GetRoleByUin(uiUin);
    if (pRoleObj)
    {
		if (pstKickRoleResp->bislogin())
		{
			CLoginHandler::LoginFailed(pSession->GetNetHead());
			CModuleHelper::GetSessionManager()->DeleteSession(pSession->GetID());
		}

		LOGERROR("Role Already Login!\n");
		return -4;
	}

	//����ǵ�¼��������ȡ����
	if(pstKickRoleResp->bislogin())
	{
		int iRet = FetchRoleFromWorldServer(pSession->GetRoleID(), 0);
		if (iRet < 0)
		{
			CLoginHandler::LoginFailed(pSession->GetNetHead());
			CModuleHelper::GetSessionManager()->DeleteSession(pSession->GetID());
		}
	}

	LOGDEBUG("Recv KickRole Ok: Uin = %u\n", uiUin);

    return 0;
}

int CKickRoleWorldHandler::FetchRoleFromWorldServer(const RoleID& stRoleID, char cEnterType)
{
    CZoneMsgHelper::GenerateMsgHead(ms_stZoneMsg, MSGID_WORLD_FETCHROLE_REQUEST);
	ms_stZoneMsg.mutable_m_stmsghead()->set_m_uin(stRoleID.uin());

	World_FetchRole_Request* pstFetchRequest = ms_stZoneMsg.mutable_m_stmsgbody()->mutable_m_stworld_fetchrole_request();

	pstFetchRequest->mutable_stroleid()->CopyFrom(stRoleID);
	pstFetchRequest->set_bislogin(true);

	pstFetchRequest->set_ireqid(CModuleHelper::GetZoneID());

    LOGDETAIL("Send FetchRole Request: Uin = %u\n", stRoleID.uin());

    int iRet = CZoneMsgHelper::SendZoneMsgToWorld(ms_stZoneMsg);

    return iRet;
}

// ���سɹ��ظ�
int CKickRoleWorldHandler::SendSuccessfullResponse(const World_KickRole_Request& rstKicker)
{
    CZoneMsgHelper::GenerateMsgHead(ms_stZoneMsg, MSGID_WORLD_KICKROLE_RESPONSE);
    World_KickRole_Response* pstResponse = ms_stZoneMsg.mutable_m_stmsgbody()->mutable_m_stworld_kickrole_response();

	pstResponse->set_ifromworldid(rstKicker.ifromworldid());
	pstResponse->set_ifromzoneid(rstKicker.ifromzoneid());
	pstResponse->set_isessionid(rstKicker.isessionid());
	pstResponse->mutable_stkickedroleid()->CopyFrom(rstKicker.stkickedroleid());
	pstResponse->set_bislogin(rstKicker.bislogin());
	pstResponse->set_iresult(T_SERVER_SUCESS);
	
    CZoneMsgHelper::SendZoneMsgToWorld(ms_stZoneMsg);

    return 0;
}

// ����ʧ�ܻظ�
int CKickRoleWorldHandler::SendFailedResponse(const World_KickRole_Request& rstKicker)
{
    CZoneMsgHelper::GenerateMsgHead(ms_stZoneMsg, MSGID_WORLD_KICKROLE_RESPONSE);
    World_KickRole_Response* pstResponse = ms_stZoneMsg.mutable_m_stmsgbody()->mutable_m_stworld_kickrole_response();

	pstResponse->set_ifromworldid(rstKicker.ifromworldid());
	pstResponse->set_ifromzoneid(rstKicker.ifromzoneid());
	pstResponse->set_isessionid(rstKicker.isessionid());
	pstResponse->mutable_stkickedroleid()->CopyFrom(rstKicker.stkickedroleid());
	pstResponse->set_bislogin(rstKicker.bislogin());
	pstResponse->set_iresult(T_ZONE_LOGINSERVER_FAILED);

    CZoneMsgHelper::SendZoneMsgToWorld(ms_stZoneMsg);

    return 0;
}
