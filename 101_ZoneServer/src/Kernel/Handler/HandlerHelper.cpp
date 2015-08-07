
#include "GameProtocol.hpp"
#include "ModuleHelper.hpp"
#include "HandlerHelper.hpp"
#include "LogAdapter.hpp"
#include "GameRole.hpp"
#include "GameSession.hpp"

using namespace ServerLib;

unsigned int CHandlerHelper::m_enErrorCode;

CGameSessionObj* CHandlerHelper::FindValidSession(const int iSessionID, const RoleID& stRoleID)
{
    CSessionManager* pSessionManager = CModuleHelper::GetSessionManager();
    if(!pSessionManager)
    {
        return NULL;
    }

    CGameSessionObj* pSession = pSessionManager->FindSessionByID(iSessionID);
    if(!pSession)
    {
        //�Ự�����ڣ�
        return NULL;
    }

    //Ч��Ự�Ƿ�Ϸ�
    if((stRoleID.uin()!=pSession->GetRoleID().uin()) || (stRoleID.uiseq()!=pSession->GetRoleID().uiseq()))
    {
        return NULL;
    }

    return pSession;
}

CGameRoleObj* CHandlerHelper::GetMsgSender(GameProtocolMsg* pZoneMsg, const TNetHead_V2* pNetHead)
{
    ASSERT_AND_LOG_RTN_PTR(pNetHead);
    ASSERT_AND_LOG_RTN_PTR(pZoneMsg);

    int iSessionID = (int)ntohl(pNetHead->m_uiSocketFD);

    CSessionManager* pSessionManager = CModuleHelper::GetSessionManager();
    if(!pSessionManager)
    {
        TRACESVR("Cannot Get SessionManager!\n");
        return NULL;
    }

    CGameSessionObj* pSession = pSessionManager->FindSessionByID(iSessionID);
    if (!pSession)
    {
        TRACESVR("Cannot Find Session: SessionID = %d\n", iSessionID);
        return NULL;
    }

    CGameRoleObj* pRoleObj = pSession->GetBindingRole();
    if (!pRoleObj)
    {
        TRACESVR("Cannot Find Binding Role: SessionID = %d\n", iSessionID);
        return NULL;
    }

    // ˢ�»�Ľ�ɫ��Ϣ
    pRoleObj->ActiveRefresh();

    return pRoleObj;
}

void CHandlerHelper::SetErrorCode(unsigned int enErrorCode)
{
    m_enErrorCode = enErrorCode;
}

unsigned int CHandlerHelper::GetErrorCode()
{
    return m_enErrorCode;
}
