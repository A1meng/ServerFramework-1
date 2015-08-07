#include <string.h>

#include "GameProtocol.hpp"
#include "ModuleHelper.hpp"
#include "ZoneObjectHelper.hpp"
#include "HandlerFactory.hpp"
#include "ZoneMsgHelper.hpp"
#include "AppLoop.hpp"
#include "HandlerHelper.hpp"
#include "ZoneOssLog.hpp"
#include "StringUtility.hpp"
#include "LogAdapter.hpp"
#include "UnitUtility.hpp"

#include "LoginHandler.hpp"

GameProtocolMsg CLoginHandler::ms_stZoneMsg;

CLoginHandler::~CLoginHandler()
{
}

int CLoginHandler::OnClientMsg()
{
	switch (m_pRequestMsg->m_stmsghead().m_uimsgid())
	{
	case MSGID_ZONE_LOGINSERVER_REQUEST:
		{
			OnRequestLoginServer();
			break;
		}

	default:
		{
			break;
		}
	}

	return 0;
}

int CLoginHandler::OnRequestLoginServer()
{
    ///////////////////////////////////////////////////////////////////////////////////////////////////////
    int iRet = SecurityCheck();
    if (iRet < 0)
    {
        LOGERROR("Security Check Failed: Uin = %u, iRet = %d\n", m_pRequestMsg->m_stmsghead().m_uin(), iRet);

        LoginFailed(*m_pNetHead);
        return -1;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////
    const Zone_LoginServer_Request& rstRequest = m_pRequestMsg->m_stmsgbody().m_stzone_loginserver_request();
    iRet = LoginRole(rstRequest.stroleid(), m_pNetHead, rstRequest);
    if (iRet < 0)
    {
        CHandlerHelper::SetErrorCode(T_ZONE_LOGINSERVER_FAILED);
        LoginFailed(*m_pNetHead);
    }

    return iRet;	
}

//֪ͨ��ɫ��¼
int CLoginHandler::NotifyRoleLogin(CGameRoleObj* pstRoleObj)
{
    /*
    CZoneMsgHelper::GenerateMsgHead(ms_stZoneMsg, MSGID_ZONE_LOGINSERVER_NOTIFY);

    Zone_LoginServer_Notify* pstNotify = ms_stZoneMsg.mutable_m_stmsgbody()->mutable_m_stloginserver_notify();
    pstNotify->mutable_stroleid()->CopyFrom(pstRoleObj->GetRoleID());
    pstNotify->set_iunitid(pstRoleObj->GetRoleInfo().stUnitInfo.iUnitID);
    pstNotify->set_izoneid(CModuleHelper::GetZoneID());

    //todo jasonxiong2 ����ȷ���Ƿ�Ҫ���֪ͨ
    //CZoneMsgHelper::SendZoneMsgToSight(ms_stZoneMsg, &(pstRoleObj->GetRoleInfo().stUnitInfo));
    */ 
     
    return 0;
}

int CLoginHandler::SecurityCheck()
{
    unsigned int uiUin = m_pRequestMsg->m_stmsghead().m_uin();
    if (uiUin == 0)
    {
        CHandlerHelper::SetErrorCode(T_ZONE_SYSTEM_PARA_ERR);
        return -1; //nolog
    }

    //todo jasonxiong ����Ŀ����п�����Ҫ��Ӻڰ�����
    /*
    CWhiteListConfig& rWhiteListConfig = 
        WhiteListCfgMgr();
    if (rWhiteListConfig.IsInBlackList(uiUin))
    {
        TRACESVR("uin: %u is in black list, can not login\n", uiUin);
        CHandlerHelper::SetErrorCode(EQEC_UinInBlackList);
        return -2;
    }
    */
	
    return 0;
}

int CLoginHandler::LoginRole(const RoleID& stRoleID, TNetHead_V2* pNetHead, const Zone_LoginServer_Request& rstRequest)
{
    ASSERT_AND_LOG_RTN_INT(pNetHead);

	int iSessionID = (int)ntohl(pNetHead->m_uiSocketFD);

	LOGDETAIL("Login Req: Uin = %u, Session = %d\n", stRoleID.uin(), iSessionID);

	// ��Session�Ѿ�����
	CSessionManager* pSessionManager = CModuleHelper::GetSessionManager();
	CGameSessionObj* pSessionObj = pSessionManager->FindSessionByID(iSessionID);
	if (pSessionObj)
	{
		LOGERROR("Session Already Exist: ID = %d\n", iSessionID);
		return -2;
	}

	// ��RoleID��Session�Ѿ����ڣ��Ҵ���δ��¼״̬
	pSessionObj = pSessionManager->FindSessionByRoleID(stRoleID);
	if (pSessionObj && pSessionObj->GetBindingRole() == NULL)
	{
		LOGERROR("Session Alreay Binding Role: ID = %d, Uin = %u\n", iSessionID, stRoleID.uin());
		return -3;
	}

    // ����һ���µĻỰ
    pSessionObj = pSessionManager->CreateSession(pNetHead, stRoleID);
    if(!pSessionObj)
    {
        LOGERROR("Cannot Create Session: Uin = %u, SessionID = %d\n", stRoleID.uin(), iSessionID);  
        return -2;
    }

    // ����ͻ��˰汾��
    pSessionObj->SetClientVersion(rstRequest.uclientversion());

	//�����¼ԭ��
	pSessionObj->SetLoginReason(rstRequest.uloginreason());

    // ��World������������,��ͬuin�Ķ�����ȥ
    int iRet = KickRoleFromWorldServer(stRoleID, iSessionID);
    if (iRet < 0)
    {
        CModuleHelper::GetSessionManager()->DeleteSession(iSessionID);
    }

    return iRet;
}

int CLoginHandler::SendFailedResponse(const unsigned int uiResultID, const TNetHead_V2& rstNetHead)
{
    CZoneMsgHelper::GenerateMsgHead(ms_stZoneMsg, MSGID_ZONE_LOGINSERVER_RESPONSE);

    Zone_LoginServer_Response* pstResp = ms_stZoneMsg.mutable_m_stmsgbody()->mutable_m_stzone_loginserver_response();
    pstResp->Clear();

    pstResp->set_iresult(uiResultID);

    CZoneMsgHelper::SendZoneMsgToClient(ms_stZoneMsg, rstNetHead);

    return 0;
}

int CLoginHandler::SendSuccessfulResponse(CGameRoleObj* pLoginRoleObj)
{
    CZoneMsgHelper::GenerateMsgHead(ms_stZoneMsg, MSGID_ZONE_LOGINSERVER_RESPONSE);
    
    Zone_LoginServer_Response* pstLoginResp = ms_stZoneMsg.mutable_m_stmsgbody()->mutable_m_stzone_loginserver_response();
    pstLoginResp->set_iresult(T_SERVER_SUCESS);
    pstLoginResp->set_iworldid(CModuleHelper::GetWorldID());
    pstLoginResp->set_izoneid(CModuleHelper::GetZoneID());

    int iRet = T_SERVER_SUCESS;

    //����������ϵĻ�����Ϣ
    BASEDBINFO* pstBaseInfo = pstLoginResp->mutable_stlogininfo()->mutable_stbaseinfo();
    pLoginRoleObj->UpdateBaseInfoToDB(*pstBaseInfo);

    //������ҵı�����Ϣ
    ITEMDBINFO* pstItemInfo = pstLoginResp->mutable_stlogininfo()->mutable_stiteminfo();
    pLoginRoleObj->UpdateRepThingsToDB(*pstItemInfo);

    //������ҵ�ս����λ��Ϣ
    FIGHTDBINFO* pstFightUnitInfo = pstLoginResp->mutable_stlogininfo()->mutable_stfightinfo();
    iRet = pLoginRoleObj->UpdateFightUnitToDB(*pstFightUnitInfo);
    if(iRet)
    {
        pLoginRoleObj->GetFightUnitManager().ClearFightUnitObj();
        return iRet;
    }

    CZoneMsgHelper::SendZoneMsgToRole(ms_stZoneMsg, pLoginRoleObj);

    return 0;
}

// ��¼�ɹ�����
int CLoginHandler::LoginOK(unsigned int uiUin, bool bNeedResponse)
{
    CGameRoleObj* pLoginRoleObj =  GameTypeK32<CGameRoleObj>::GetByKey(uiUin);
    ASSERT_AND_LOG_RTN_INT(pLoginRoleObj);

    if(bNeedResponse)
    {
        // ��¼�ɹ�
        SendSuccessfulResponse(pLoginRoleObj);
    }

    // ������¼�¼�
    CModuleHelper::GetUnitEventManager()->NotifyUnitLogin(&(pLoginRoleObj->GetRoleInfo().stUnitInfo));

    // ֪ͨ�����û�
    NotifyRoleLogin(pLoginRoleObj);

    LOGDEBUG("Uin: %u\n", uiUin);

    // ��¼��ˮ
    CZoneOssLog::TraceLogin(*pLoginRoleObj);

    return 0;
}

// ��¼ʧ�ܴ���
int CLoginHandler::LoginFailed(const TNetHead_V2& rstNetHead)
{ 
	unsigned int uiResultID = CHandlerHelper::GetErrorCode();
	SendFailedResponse(uiResultID, rstNetHead);

    return 0;
}

//ͨ��World����ͬuin���Ѿ���¼�ĺ�������
int CLoginHandler::KickRoleFromWorldServer(const RoleID& stRoleID, int iFromSessionID)
{
    CZoneMsgHelper::GenerateMsgHead(ms_stZoneMsg, MSGID_WORLD_KICKROLE_REQUEST);

    World_KickRole_Request* pstReq = ms_stZoneMsg.mutable_m_stmsgbody()->mutable_m_stworld_kickrole_request();

    pstReq->set_ifromworldid(CModuleHelper::GetWorldID());
    pstReq->set_ifromzoneid(CModuleHelper::GetZoneID());
    pstReq->set_isessionid(iFromSessionID);
    pstReq->mutable_stkickedroleid()->CopyFrom(stRoleID);
    pstReq->set_bislogin((iFromSessionID>0));

    LOGDEBUG("Send KickRole Request: Uin = %u\n", stRoleID.uin());

    int iRet = CZoneMsgHelper::SendZoneMsgToWorld(ms_stZoneMsg);

    return iRet;
}
