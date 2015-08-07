
#include "GameProtocol.hpp"
#include "ModuleHelper.hpp"
#include "GameSession.hpp"
#include "GameRole.hpp"
#include "LogAdapter.hpp"
#include "ZoneObjectHelper.hpp"
#include "ZoneMsgHelper.hpp"
#include "HandlerHelper.hpp"
#include "UpdateRoleInfoHandler.hpp"
#include "HandlerHelper.hpp"
#include "ZoneOssLog.hpp"
#include "HandlerFactory.hpp"
#include "KickRoleWorldHandler.hpp"
#include "UnitUtility.hpp"
#include "CombatFramework.hpp"

#include "LogoutHandler.hpp"

CLogoutHandler::~CLogoutHandler()
{

}

GameProtocolMsg CLogoutHandler::m_stZoneMsg;
int CLogoutHandler::OnClientMsg()
{
    switch (m_pRequestMsg->m_stmsghead().m_uimsgid())
    {
    case MSGID_LOGOUTSERVER_REQUEST:
        {
            OnRequestLogout();
            break;
        }

    default:
        {
            break;
        }
    }

    return 0;
}

int CLogoutHandler::OnRequestLogout()
{
    ///////////////////////////////////////////////////////////////////////////////////////////////////////
    int iRet = SecurityCheck();
    if (iRet < 0)
    {
		LOGERROR("Securit check fail: %d\n", iRet);
        return -1;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////
	if (CUnitUtility::IsUnitStatusSet(&(m_pRoleObj->GetUnitInfo()), EGUS_LOGOUT))
	{
		LOGERROR("Role in logout: %u\n", m_pRoleObj->GetRoleID().uin());
		return -2;
	}

	//��������ԭ��
	//todo jasonxiong ��ʱ����Ҫ��������ԭ��
	//m_pRoleObj->SetLogoutRequestReason(m_pRequestMsg->m_stmsgbody().m_stlogoutserver_response();
    iRet = LogoutRole(m_pRoleObj, 0);

    return iRet;
}

int CLogoutHandler::LogoutRole(CGameRoleObj* pRoleObj, int iReason)
{
    ASSERT_AND_LOG_RTN_INT(pRoleObj);

    CGameSessionObj* pSessionObj = pRoleObj->GetSession();
	ASSERT_AND_LOG_RTN_INT(pSessionObj);

    unsigned int uiUin = pRoleObj->GetRoleID().uin();

    LOGDETAIL("Role log out: Uin = %u, Session = %d, Reason = %d .................\n", uiUin, pSessionObj->GetID(), iReason);

	CUnitUtility::SetUnitStatus(&(pRoleObj->GetUnitInfo()), EGUS_LOGOUT);
    time_t stNow = time(NULL);
    pRoleObj->SetLogoutTime(stNow);
	pRoleObj->SetLogoutReason(iReason);

    // 1. �ǳ�׼��, ֹͣ��ɫ��������Ϊ, ���½�ɫ������
    LogoutPrepare(pRoleObj);

	//��World����Logout Notify
	CZoneMsgHelper::GenerateMsgHead(m_stZoneMsg, MSGID_ZONE_LOGOUT_NOTIFY);
	Zone_Logout_Notify* pstNotify = m_stZoneMsg.mutable_m_stmsgbody()->mutable_m_stzone_logout_notify();
	pstNotify->mutable_stroleid()->CopyFrom(pRoleObj->GetRoleID());
	CZoneMsgHelper::SendZoneMsgToWorld(m_stZoneMsg);

    //ͬ����ҽ�ɫ���ݵ����ݿ�
    int iRet = CUpdateRoleInfo_Handler::UpdateRoleInfo(pRoleObj, 1);
    if(iRet)
    {
		LOGERROR("Failed to update role info before logout, uin %u\n", uiUin);
		return iRet;
    }

    return 0;
}

// �ǳ�׼������, ֹͣ��ҵ�������Ϊ
int CLogoutHandler::LogoutPrepare(CGameRoleObj* pRoleObj)
{
    ASSERT_AND_LOG_RTN_INT(pRoleObj);

    CGameSessionObj* pSessionObj = pRoleObj->GetSession();
    ASSERT_AND_LOG_RTN_INT(pSessionObj);

    // ��¼��ˮ
    CZoneOssLog::TraceLogout(*pRoleObj);

    unsigned int uiUin = pRoleObj->GetRoleID().uin();

	//ֹͣ���ս��
	CCombatFramework::Instance()->FinCombat(*pRoleObj);

    LOGDETAIL("LogoutPrepare:  Uin = %u, Sessoion = %d\n", uiUin, pSessionObj->GetID());

    return 0;
}

// ��Ϣ֪ͨ������ɾ��
int CLogoutHandler::LogoutAction(CGameRoleObj* pRoleObj)
{
    ASSERT_AND_LOG_RTN_INT(pRoleObj);

    CGameSessionObj* pSessionObj = pRoleObj->GetSession();
    ASSERT_AND_LOG_RTN_INT(pSessionObj);

    // ���سɹ��ظ�
	if(pRoleObj->GetKicker().ifromworldid() <= 0)
	{
		//ֻ�������ǳ�ʱ�ŷ���resp
		SendSuccessfulResponse(pRoleObj);
	}

	// ֪ͨ��Ұ�����
    NotifyRoleLogout(pRoleObj);

	// �����ǳ��¼�
    CModuleHelper::GetUnitEventManager()->NotifyUnitLogout(&(pRoleObj->GetRoleInfo().stUnitInfo));

    // �Ͽ��Ự
    CModuleHelper::GetSessionManager()->DeleteSession(pSessionObj->GetID());

	// ֪ͨWorld���ı����״̬
    NotifyLogoutToWorld(pRoleObj->GetRoleID());

    unsigned int uiUin = pRoleObj->GetRoleID().uin();

    // ֪ͨWorld���˳ɹ�
    World_KickRole_Request& rstKicker = pRoleObj->GetKicker();
    if (rstKicker.ifromworldid() > 0)
    {
		CKickRoleWorldHandler::SendSuccessfullResponse(rstKicker);
    }

	//ɾ��ս����λ����
	pRoleObj->GetFightUnitManager().ClearFightUnitObj();

    //ɾ����ҽ�ɫ
    CUnitUtility::DeleteUnit(&(pRoleObj->GetUnitInfo()));

    pRoleObj = NULL;

    LOGDETAIL("Logout Success: Uin = %u\n", uiUin);

    return 0;
}

int CLogoutHandler::SecurityCheck()
{
	if (m_pRequestMsg->m_stmsgbody().ByteSize() == 0)
	{
		CSessionManager* pSessionManager = CModuleHelper::GetSessionManager();

		int iSessionID = (int)ntohl(m_pNetHead->m_uiSocketFD);
		m_pSession = pSessionManager->FindSessionByID(iSessionID);
		if (!m_pSession)
		{
			LOGERROR("No session: ID = %d\n", iSessionID);
			return -1;
		}

		m_pRoleObj = m_pSession->GetBindingRole();
		if (!m_pRoleObj)
		{
			LOGERROR("Connect Closed: Session = %d\n", iSessionID);

			CModuleHelper::GetSessionManager()->DeleteSession(m_pSession->GetID());
			return -3;
		}

		LOGDEBUG("Connect Closed: Uin = %u�� Session = %d\n", 
			m_pRoleObj->GetRoleID().uin(), iSessionID);

		return 0;
	}

	int iRet = IHandler::SecurityCheck();
	if (iRet < 0)
	{
		LOGERROR("IHandler::SecurityCheck failed, iRet:%d\n", iRet);
		return iRet;
	}
    return 0;
}

int CLogoutHandler::NotifyLogoutToWorld(const RoleID& stRoleID)
{
    CZoneMsgHelper::GenerateMsgHead(m_stZoneMsg, MSGID_ZONE_LOGOUT_NOTIFY);

    m_stZoneMsg.mutable_m_stmsgbody()->mutable_m_stzone_logout_notify()->mutable_stroleid()->CopyFrom(stRoleID);

	return CZoneMsgHelper::SendZoneMsgToWorld(m_stZoneMsg); 
}

void CLogoutHandler::NotifyRoleLogout(CGameRoleObj* pstRoleObj)
{
	ASSERT_AND_LOG_RTN_VOID(pstRoleObj);

	CZoneMsgHelper::GenerateMsgHead(m_stZoneMsg, MSGID_ZONE_LOGOUT_NOTIFY);
	m_stZoneMsg.mutable_m_stmsgbody()->mutable_m_stzone_logout_notify()->mutable_stroleid()->CopyFrom(pstRoleObj->GetRoleID());

	//���͸��Լ�
	CZoneMsgHelper::SendZoneMsgToRole(m_stZoneMsg, pstRoleObj);

	return;
}

int CLogoutHandler::SendFailedResponse(const unsigned int uiResultID, const TNetHead_V2& rstNetHead)
{
    CZoneMsgHelper::GenerateMsgHead(m_stZoneMsg, MSGID_LOGOUTSERVER_RESPONSE);

	m_stZoneMsg.mutable_m_stmsgbody()->mutable_m_stlogoutserver_response()->set_iresult(uiResultID);

    CZoneMsgHelper::SendZoneMsgToClient(m_stZoneMsg, rstNetHead);

    return 0;
}

int CLogoutHandler::SendSuccessfulResponse(CGameRoleObj* pRoleObj)
{
    ASSERT_AND_LOG_RTN_INT(pRoleObj);

    CZoneMsgHelper::GenerateMsgHead(m_stZoneMsg, MSGID_LOGOUTSERVER_RESPONSE);

    m_stZoneMsg.mutable_m_stmsgbody()->mutable_m_stlogoutserver_response()->set_iresult(T_SERVER_SUCESS);

    CZoneMsgHelper::SendZoneMsgToRole(m_stZoneMsg, pRoleObj);

    return 0;
}

