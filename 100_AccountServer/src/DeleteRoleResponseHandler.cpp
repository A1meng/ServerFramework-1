#include <assert.h>

#include "GameProtocol.hpp"
#include "LogAdapter.hpp"
#include "FixedHashCache.hpp"
#include "LRUHashCache.hpp"
#include "SessionObj.hpp"
#include "AccountSingleton.hpp"
#include "TimeStampConverter.hpp"
#include "DeleteRoleResponseHandler.hpp"

CDeleteRoleResponseHandler::CDeleteRoleResponseHandler()
{
}

void CDeleteRoleResponseHandler::OnClientMsg(TNetHead_V2* pstNetHead,
        GameProtocolMsg* pstMsg, SHandleResult* pstResult)
{
    // ��ʹ��NetHead��Result
    ASSERT_AND_LOG_RTN_VOID(pstMsg);

    m_pstRequestMsg = pstMsg;
    m_uiSessionFD = TimeStampToSessionFD(m_pstRequestMsg->m_stmsghead().m_uisessionfd());
    m_unValue = TimeStampToValue(m_pstRequestMsg->m_stmsghead().m_uisessionfd());

    DeleteRole_Account_Response* pstDeleteRoleResponse =
        m_pstRequestMsg->mutable_m_stmsgbody()->mutable_m_staccountdeleteroleresponse();
    unsigned int uiUin = pstDeleteRoleResponse->stroleid().uin();

    LOGDEBUG("Handling DeleteRoleResponse from world server, "
             "result: %d, uin: %u, sockfd: %u, value: %d\n",
             pstDeleteRoleResponse->iresult(), uiUin, m_uiSessionFD, m_unValue);

    CSessionObj* pSession = SessionManager->GetSession(m_uiSessionFD, m_unValue);

    // ��¼�ɹ���־
    if (T_SERVER_SUCESS == pstDeleteRoleResponse->iresult())
    {
        /*
        TEventId stEventId;
        CAccountBillLog::TraceRoleDelete(
            pstDeleteRoleResponse->uin(),
            pSession? pSession->GetNetHead()->m_uiSrcIP : 0,
            pstDeleteRoleResponse->sznickname().c_str(),
            pstDeleteRoleResponse->ucgender(),
            pstDeleteRoleResponse->uprofession(),
            pstDeleteRoleResponse->ulevel(),
            stEventId);
            */
    }

    if (pSession != NULL) // ԭʼsession��Ȼ����
    {
        // ������Ӧ��Ϣ����NetHead��ת����LotusServer
        SendDeleteRoleResponseToLotus();

        // ���cache�е�session���
        //SessionManager->DeleteSession(m_uiSessionFD);
    }
}

void CDeleteRoleResponseHandler::SendDeleteRoleResponseToLotus()
{
    if (EncodeAndSendCode(CSProtocolEngine,
                          SessionManager->GetSession(m_uiSessionFD, m_unValue)->GetNetHead(),
                          m_pstRequestMsg, GAME_SERVER_LOTUSACCOUNT) != 0)
    {
        LOGERROR("Failed to send DeleteRoleResponse to lotus server\n");
        return;
    }

    LOGDEBUG("Send DeleteRoleResponse to lotus server\n");
}

//jasonxiong ��ʱ��ע�͵������濴�Ƿ���Ҫ����NameSvr
/*
void CDeleteRoleResponseHandler::SendUpdateNicknameRequestToName()
{
    TAccountMsg stUpdateNicknameRequest;

    memset(&stUpdateNicknameRequest, 0, sizeof(TAccountMsg));
    GenerateMsgHead(&stUpdateNicknameRequest,
        m_pstRequestMsg->m_stMsgHead.m_uiTimeStamp_Low, MSGID_NAME_UPDATENAME_REQUEST,
        SSProtocolEngine->GetProtocolVersion(), m_pstRequestMsg->m_stMsgHead.m_uiUin);

    UPDATENAME_NAME_REQUEST* pstRequest =
        &(stUpdateNicknameRequest.m_stMsgBody.m_stUpdateNameRequest);

    pstRequest->m_ucNameType = NAMETYPE_ROLEID;
    strncpy(pstRequest->m_szName,
            m_pstRequestMsg->m_stMsgBody.m_stAccountDeleteRoleResponse.m_szNickName,
            sizeof(pstRequest->m_szName) - 1);
    pstRequest->m_ucUpdateType = NAMEUPDATETYPE_REMOVE; // ɾ��nickname����
    pstRequest->m_stPrimaryKey.m_stRoleID =
        m_pstRequestMsg->m_stMsgBody.m_stAccountDeleteRoleResponse.m_stRoleID;

    if (EncodeAndSendCode(SSProtocolEngine,
                          NULL, &stUpdateNicknameRequest, GAME_SERVER_WORLD) != 0)
    {
        TRACESVR("Failed to send UpdateNicknameRequest to World server\n");
        return;
    }

    TRACESVR("Send UpdateNicknameRequest to World server, "
             "uin: %u, nickname: %s, update type: %d\n",
             m_pstRequestMsg->m_stMsgHead.m_uiUin,
             pstRequest->m_szName, pstRequest->m_ucUpdateType);
}
*/
