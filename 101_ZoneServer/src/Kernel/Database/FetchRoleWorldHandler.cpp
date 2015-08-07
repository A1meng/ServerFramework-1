#include <assert.h>
#include <string.h>

#include "ProtoDataUtility.hpp"
#include "GameProtocol.hpp"
#include "ModuleHelper.hpp"
#include "ZoneObjectHelper.hpp"
#include "HandlerFactory.hpp"
#include "ZoneErrorNumDef.hpp"
#include "UpdateRoleInfoHandler.hpp"
#include "ZoneMsgHelper.hpp"
#include "UnitUtility.hpp"
#include "LoginHandler.hpp"
#include "HandlerHelper.hpp"
#include "GameConfigDefine.hpp"
#include "LogoutHandler.hpp"

#include "FetchRoleWorldHandler.hpp"

using namespace ServerLib;

CFetchRoleWorldHandler::~CFetchRoleWorldHandler(void)
{
}

GameProtocolMsg CFetchRoleWorldHandler::m_stZoneMsg;
int CFetchRoleWorldHandler::OnClientMsg()
{
    int iRet = -1;
    switch (m_pRequestMsg->m_stmsghead().m_uimsgid())
    {
    case MSGID_WORLD_FETCHROLE_RESPONSE:
        {
            iRet = OnFetchRole();
            break;
        }

    default:
        {
            break;
        }
    }

    return iRet;
}

int CFetchRoleWorldHandler::OnFetchRole()
{
    const World_FetchRole_Response& rstFetchResp = m_pRequestMsg->m_stmsgbody().m_stworld_fetchrole_response();
    unsigned int uiUin = rstFetchResp.stroleid().uin();

    // ��ȡ�Ự
    CSessionManager* pSessionManager = CModuleHelper::GetSessionManager();
    m_pSession = pSessionManager->FindSessionByRoleID(rstFetchResp.stroleid());
    if(!m_pSession)
    {
        TRACESVR("Cannot Find Session: Uin = %u\n", uiUin);
        return -2;
    }

    // Session �Ѿ���ʹ��,loginʱ���
    if (rstFetchResp.bislogin() && m_pSession->GetBindingRole() != NULL)
    {
        TRACESVR("Session Already Binding Role: Uin = %u, Session = %d\n",
            uiUin, m_pSession->GetID());
        CHandlerHelper::SetErrorCode(T_ZONE_SESSION_EXISTS_ERR);
        CLoginHandler::LoginFailed(m_pSession->GetNetHead());
        return -3;
    }

    // ��ȡ��ɫ��Ϣʧ��
    if (rstFetchResp.iresult() != 0)
    {
        TRACESVR("FetchRole Failed: Uin = %u, ResultID = %d\n", uiUin, rstFetchResp.iresult());
        CHandlerHelper::SetErrorCode(T_ZONE_SESSION_EXISTS_ERR);
        CLoginHandler::LoginFailed(m_pSession->GetNetHead());
        pSessionManager->DeleteSession(m_pSession->GetID());
        return -4;
    }

    // ���uin�Ƿ�һ��
    if (uiUin != m_pSession->GetRoleID().uin())
    {
        CHandlerHelper::SetErrorCode(T_ZONE_SESSION_EXISTS_ERR);
        CLoginHandler::LoginFailed(m_pSession->GetNetHead());
        pSessionManager->DeleteSession(m_pSession->GetID());

        // ���World����
        CLogoutHandler::NotifyLogoutToWorld(rstFetchResp.stroleid());

        TRACESVR("Invalid Session: Uin = %u\n", uiUin);

        return -5;
    }

    TRACESVR("FetchRole OK: Uin = %u\n", uiUin);

    // ��¼��ɫ
    int iRet = LoginRole();
    if (iRet < 0)
    {
        CHandlerHelper::SetErrorCode(T_ZONE_SESSION_EXISTS_ERR);
        CLoginHandler::LoginFailed(m_pSession->GetNetHead());
        pSessionManager->DeleteSession(m_pSession->GetID());

        // ���World����
        CLogoutHandler::NotifyLogoutToWorld(rstFetchResp.stroleid());

        TRACESVR("LoginRole Failed: iRet = %d\n", iRet);

        return -6;
    }

    TRACESVR("LoginRole OK: Uin = %u\n", uiUin);

    // ��ѯ�ʼ�����
    //todo jasonxiong ������ͳһ��������ʱɾ��������
    QueryMailNumber();

    // ��ȡ��Ϸ���ѻ��浽��ɫ������
    //todo jasonxiong ������ͳһ��������ʱɾ��������
    FetchGameFriend();

    return 0;
}

int CFetchRoleWorldHandler::LoginRole()
{
    const World_FetchRole_Response& rstFetchResp = m_pRequestMsg->m_stmsgbody().m_stworld_fetchrole_response();
    unsigned int uiUin = rstFetchResp.stroleid().uin();

    // ������ɫ����
    m_pRoleObj = (CGameRoleObj*)CUnitUtility::CreateUnit(EUT_ROLE, uiUin);
    if(!m_pRoleObj)
    {
        TRACESVR("Create RoleObj Failed.\n");
        return -1;
    }

    // ��ʼ����ɫ����ϵͳ
    int iRet = m_pRoleObj->InitRole(rstFetchResp.stroleid());
    if (iRet < 0)
    {
        TRACESVR("Init Role Failed: iRet = %d\n", iRet);

        CUnitUtility::DeleteUnit(&m_pRoleObj->GetUnitInfo());

        return -2;
    }

    // ���Ự�ͽ�ɫ��
    m_pSession->SetBindingRole(m_pRoleObj);
    m_pRoleObj->SetSessionID(m_pSession->GetID());

    // ��ʼ����ɫ����
    iRet = InitRoleData();
    if (iRet < 0)
    {
        CUnitUtility::DeleteUnit(&m_pRoleObj->GetUnitInfo());
        TRACESVR("Init RoleData Failed: iRet = %d\n", iRet);

        return -3;
    }

    // ֪ͨ��¼�ɹ�
    CLoginHandler::LoginOK(uiUin);

    // ��½���ʼ��
    InitRoleAfterLogin();

    return 0;
}

// ��ʼ����ɫ����
int CFetchRoleWorldHandler::InitRoleData()
{
    const World_FetchRole_Response& rstFetchResp = m_pRequestMsg->m_stmsgbody().m_stworld_fetchrole_response();
    unsigned int uiUin = rstFetchResp.stroleid().uin();

    CGameSessionObj *pSession = m_pRoleObj->GetSession();
    if (pSession == NULL)
    {
        TRACESVR("No session: %u\n", uiUin);
        return -20;
    }

    int iRet = T_SERVER_SUCESS;

    //1.��ʼ����ҵĻ�����Ϣ
    BASEDBINFO stBaseInfo;
    if(!DecodeProtoData(rstFetchResp.stuserinfo().strbaseinfo(), stBaseInfo))
    {
        TRACESVR("Failed to decode base proto data, uin %u\n", uiUin);
        return -21;
    }
    m_pRoleObj->InitBaseInfoFromDB(stBaseInfo);

    //2.��ʼ����ҵ�������Ϣ
    QUESTDBINFO stQuestInfo;
    if(!DecodeProtoData(rstFetchResp.stuserinfo().strquestinfo(), stQuestInfo))
    {
        TRACESVR("Failed to decode quest proto data, uin %u\n", uiUin);
        return -22;
    }
    //m_pRoleObj->InitStoryFromDB(stQuestInfo);
    
    //3.��ʼ����ҵ���Ʒ��Ϣ
    ITEMDBINFO stItemInfo;
    if(!DecodeProtoData(rstFetchResp.stuserinfo().striteminfo(), stItemInfo))
    {
        TRACESVR("Failed to decode item proto data, uin %u\n", uiUin);
        return -23;
    }
    m_pRoleObj->InitRepThingsFromDB(stItemInfo);

    //4.��ʼ����ҵ�����ս����Ϣ
    FIGHTDBINFO stFightInfo;
    if(!DecodeProtoData(rstFetchResp.stuserinfo().strfightinfo(), stFightInfo))
    {
        TRACESVR("Failed to decode fight proto data, uin %u\n", uiUin);
        return -25;
    }

    iRet = m_pRoleObj->InitFightUnitFromDB(stFightInfo);
    if(iRet)
    {
        TRACESVR("Failed to init fight unit data, ret %d, uin %u\n", iRet, uiUin);

        m_pRoleObj->GetFightUnitManager().ClearFightUnitObj();

        return iRet;
    }

    //5.��ʼ����ҵĺ�����Ϣ
    FRIENDDBINFO stFriendInfo;
    if(!DecodeProtoData(rstFetchResp.stuserinfo().strfriendinfo(), stFriendInfo))
    {
        TRACESVR("Failed to decode friend proto data, uin %u\n", uiUin);
        return -26;
    }

    //6.��ʼ����ҵ�Reserved1�ֶ�
    RESERVED1DBINFO stReserved1Info;
    if(!DecodeProtoData(rstFetchResp.stuserinfo().strreserved1(), stReserved1Info))
    {
        TRACESVR("Failed to decode reserved1 proto data, uin %u\n", uiUin);
        return -27;
    }

    //7.��ʼ����ҵ�Reserved2�ֶ�
    RESERVED2DBINFO stReserved2Info;
    if(!DecodeProtoData(rstFetchResp.stuserinfo().strreserved2(), stReserved2Info))
    {
        TRACESVR("Failed to decode reserved2 proto data, uin %u\n", uiUin);
        return -28;
    }
     
    //todo jasonxiong ��ʼ����ҵ�������Ϣ

    // GM��־
    //todo jasonxiong ��ע�͵�����GM��صĹ��ܣ�������Ҫʱ�ٽ��п���
    /*
    m_pRoleObj->GetRoleInfo().m_stBaseProfile.m_cGMType = rstDBRoleInfo.fGM;
    CWhiteListConfig& rWhiteListConfig = WhiteListCfgMgr();
    bool bGM = rWhiteListConfig.IsInGMList(m_pRoleObj->GetRoleID().m_uiUin);
    if (bGM)
    {
        CUnitUtility::SetUnitStatus(&(m_pRoleObj->GetRoleInfo().m_stUnitInfo), EUS_ISGM);
    }
    else
    {
        CUnitUtility::ClearUnitStatus(&(m_pRoleObj->GetRoleInfo().m_stUnitInfo), EUS_ISGM);
    } 
    */ 

    // ����ʱ��
    m_pRoleObj->SetLoginCount(stBaseInfo.ilogincount()+1);

    m_pRoleObj->SetOnline();      

    return 0;
}

// ��¼���ʼ��
int CFetchRoleWorldHandler::InitRoleAfterLogin()
{
    return 0;
}

int CFetchRoleWorldHandler::QueryMailNumber()
{
    //todo jasonxiong ������ͳһ����
    /*
    CMailMsgHandler* pHandler =
        dynamic_cast<CMailMsgHandler*>(CHandlerFactory::GetHandler(MSGID_MAIL_MAILNUMBER_NOTIFY));
    ASSERT_AND_LOG_RTN_INT(pHandler);

    return pHandler->SendMailNumberRequestToWorld(m_pRoleObj->GetRoleID());
    */

    return 0;
}

int CFetchRoleWorldHandler::FetchGameFriend()
{
    //todo jasonxiong ������ͳһ����
    /*
    CFriendMsgHandler* pHandler =
        dynamic_cast<CFriendMsgHandler*>(CHandlerFactory::GetHandler(MSGID_FRIEND_FETCHGAMEFRIEND_REQUEST));
    ASSERT_AND_LOG_RTN_INT(pHandler);

    return pHandler->SendFetchGameFriendRequestToWorld(m_pRoleObj->GetRoleID());
    */

    return 0;
}

