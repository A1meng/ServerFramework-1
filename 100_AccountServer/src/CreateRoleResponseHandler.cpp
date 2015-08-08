#include <assert.h>

#include "GameProtocol.hpp"
#include "LogAdapter.hpp"
#include "FixedHashCache.hpp"
#include "LRUHashCache.hpp"
#include "CreateRoleRequestObj.hpp"
#include "SessionObj.hpp"
#include "AccountSingleton.hpp"
#include "TimeStampConverter.hpp"
#include "AccountOssLog.hpp"

#include "CreateRoleResponseHandler.hpp"


CCreateRoleResponseHandler::CCreateRoleResponseHandler()
{
}

void CCreateRoleResponseHandler::OnClientMsg(TNetHead_V2* pstNetHead,
        GameProtocolMsg* pstMsg, SHandleResult* pstResult)
{
    // ��ʹ��NetHead��Result
    ASSERT_AND_LOG_RTN_VOID(pstMsg);

    m_pstRequestMsg = pstMsg;
    m_uiSessionFD = TimeStampToSessionFD(m_pstRequestMsg->m_stmsghead().m_uisessionfd());
    m_unValue = TimeStampToValue(m_pstRequestMsg->m_stmsghead().m_uisessionfd());

    CreateRole_Account_Response* pstCreateRoleResponse =
        m_pstRequestMsg->mutable_m_stmsgbody()->mutable_m_staccountcreateroleresponse();
    unsigned int uiUin = pstCreateRoleResponse->stroleid().uin();

    //���ý�ɫ��world id
    pstCreateRoleResponse->set_uworldid(CAccountApp::GetWorldID());

    LOGDEBUG("Handling CreateRoleResponse from world server, uin: %u, result: %d, sockfd: %u, value: %d\n",
             uiUin, pstCreateRoleResponse->iresult(), m_uiSessionFD, m_unValue);

    // ��Ϣ����ɹ�������̨������ɫ�ɹ�
    if (T_SERVER_SUCESS == pstCreateRoleResponse->iresult())
    {
        //��¼������ɫ�ɹ���OSS��־
        CAccountOssLog::TraceCreateAccount(pstCreateRoleResponse->stroleid().uin(), m_uiSessionFD);
    }
    else
    {
        //����ɾ�����ֵ������NameDB
        SendDeleteNameRequestToWorld(pstCreateRoleResponse->sznickname());
    }

    CSessionObj* pSession = SessionManager->GetSession(m_uiSessionFD, m_unValue);
    if (pSession != NULL)
    {
        // ������Ӧ��Ϣ����NetHead��ת����LotusServer
        SendCreateRoleResponseToLotus();

        // ���cache�е�session���
        //SessionManager->DeleteSession(m_uiSessionFD);
    }

    // ���cache�еĴ�����ɫ��Ϣ���
    CFixedHashCache<CCreateRoleRequestObj>::DeleteByUin(uiUin);
}

//����ɾ�����ֵ������NameDB
void CCreateRoleResponseHandler::SendDeleteNameRequestToWorld(const std::string& strName)
{
    static GameProtocolMsg stDeleteNameMsg;

    GenerateMsgHead(&stDeleteNameMsg, m_uiSessionFD, MSGID_ACCOUNT_DELETEROLE_REQUEST, 0);

    DeleteName_Request* pstReq = stDeleteNameMsg.mutable_m_stmsgbody()->mutable_m_stdeletename_request();
    pstReq->set_strname(strName);
    pstReq->set_itype(EN_NAME_TYPE_ROLE);

    if(EncodeAndSendCode(SSProtocolEngine, NULL, &stDeleteNameMsg, GAME_SERVER_WORLD) != 0)
    {
        LOGERROR("Failed to send DeleteNameRequest to world!\n");
        return;
    }

    LOGDEBUG("Send DeleteNameRequest to world!\n");

    return;
}

void CCreateRoleResponseHandler::SendCreateRoleResponseToLotus()
{
    if (EncodeAndSendCode(CSProtocolEngine,
                          SessionManager->GetSession(m_uiSessionFD, m_unValue)->GetNetHead(),
                          m_pstRequestMsg, GAME_SERVER_LOTUSACCOUNT) != 0)
    {
        LOGERROR("Failed to send CreateRoleResponse to lotus server\n");
        return;
    }

    LOGDEBUG("Send CreateRoleResponse to lotus server\n");
}
