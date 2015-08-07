#include <assert.h>
#include <arpa/inet.h>

#include "GameProtocol.hpp"
#include "LogAdapter.hpp"
#include "FixedHashCache.hpp"
#include "LRUHashCache.hpp"
#include "RoleNumberObj.hpp"
#include "SessionObj.hpp"
#include "AccountSingleton.hpp"
#include "TimeStampConverter.hpp"
#include "ListRoleRequestHandler.hpp"
#include "AccountApp.hpp"
#include "ConfigMgr.hpp"

CListRoleRequestHandler::CListRoleRequestHandler()
{
}

int CListRoleRequestHandler::CheckParam(unsigned int& rushResultID)
{
    rushResultID = T_ACCOUNT_SYSTEM_PARA_ERR;
    // ��m_pstRequestMsg�е��ֶ������
    Account_ListRole_Request* pstListRoleRequest =
        m_pstRequestMsg->mutable_m_stmsgbody()->mutable_m_staccountlistrolerequest();

    // ���Uin
    if (m_pstRequestMsg->m_stmsghead().m_uin() != pstListRoleRequest->uin())
    {
        LOGERROR("Invalid Uin: %u\n", pstListRoleRequest->uin());
        return -1;
    }

    rushResultID = T_SERVER_SUCESS;

    return 0;
}

void CListRoleRequestHandler::OnClientMsg(TNetHead_V2* pstNetHead,
        GameProtocolMsg* pstMsg, SHandleResult* pstResult)
{
    // ��ʹ��Result
    ASSERT_AND_LOG_RTN_VOID(pstNetHead);
    ASSERT_AND_LOG_RTN_VOID(pstMsg);

    m_pstNetHead = pstNetHead;
    m_pstRequestMsg = pstMsg;

    // ���������Ϣ���Ƿ���ڷǷ��ֶ�
    unsigned ushResultID = T_SERVER_SUCESS;
    if (CheckParam(ushResultID) != 0)
    {
        LOGERROR("Invalid parameter found in the request\n");
        SendResponseToLotus(ushResultID);
        return;
    }

    Account_ListRole_Request* pstListRoleRequest =
        m_pstRequestMsg->mutable_m_stmsgbody()->mutable_m_staccountlistrolerequest();
    unsigned int uiUin = pstListRoleRequest->uin();
    short nWorldID = pstListRoleRequest->world();

    LOGDETAIL("Handling ListRoleRequest from lotus server, uin: %u, world: %u\n", uiUin, nWorldID);

    // ���session�Ƿ��Ѿ����ڣ�session�����Ƿ񵽴�����
    int iRes = SessionManager->CheckSession(*m_pstNetHead);
    if (iRes != T_SERVER_SUCESS)
    {
        SendResponseToLotus(iRes);
        return;
    }

    time_t tmNow = time(NULL); // �����������ʱ��

    // ����NetHead����session
    CSessionObj* pSessionObj = SessionManager->CreateSession(*m_pstNetHead);
    ASSERT_AND_LOG_RTN_VOID(pSessionObj);
    pSessionObj->SetCreatedTime(&tmNow);

    // ����Session��Uin
    if (pSessionObj->GetUin() == 0)
    {
        pSessionObj->SetUin(m_pstRequestMsg->m_stmsghead().m_uin());
    }

    // ��֤Session��Uin
    if (pSessionObj->GetUin() != m_pstRequestMsg->m_stmsgbody().m_staccountlistrolerequest().uin())
    {
        return;
    }

    m_uiSessionFD = pSessionObj->GetSessionFD();
    m_unValue = pSessionObj->GetValue();
    LOGDETAIL("sockfd: %u, value: %d\n", m_uiSessionFD, m_unValue);

    char szTime[32] = "";
    pSessionObj->GetCreatedTime(szTime, sizeof(szTime));
    LOGDETAIL("session cache created time: %s", szTime); // ����: Wed Sep 29 10:59:46 2010

    //todo jasonxiong ������Կ����ڴ��н�����ҽ�ɫ������Ϣ
    // �����û��ڽ�ɫ�������������Ƿ�����Ӧ���
//    CRoleNumberObj* pRoleNumberObj = CLRUHashCache<CRoleNumberObj>::GetByUin(uiUin);
//    if (pRoleNumberObj) // ���л���
//    {
//        // ���û��ڸ�world�ϵĽ�ɫ����Ϊ0
//        if (0 == pRoleNumberObj->GetRoleNumberOnWorld(nWorldID))
//        {
//            SendResponseToLotus(EQEC_Success);
//            return;
//        }
//    }
//
    SendListRoleRequestToWorld(); // ת����ѯ��ɫ�б���Ϣ��world server
}

void CListRoleRequestHandler::SendListRoleRequestToWorld()
{
    // �޸���Ϣͷ�е�TimeStamp
    m_pstRequestMsg->mutable_m_stmsghead()->set_m_uisessionfd(GenerateTimeStamp(m_uiSessionFD, m_unValue));

    if (EncodeAndSendCode(SSProtocolEngine,
                          NULL, m_pstRequestMsg, GAME_SERVER_WORLD) != 0)
    {
        LOGERROR("Failed to send ListRoleRequest to world server\n");
        return;
    }

    const Account_ListRole_Request& rstRequest = m_pstRequestMsg->m_stmsgbody().m_staccountlistrolerequest();

    LOGDEBUG("Send ListRoleRequest to world server, uin: %u, world: %d\n", rstRequest.uin(), rstRequest.world());
}

void CListRoleRequestHandler::SendResponseToLotus(const unsigned int uiResultID)
{
    GameProtocolMsg stFailedResponse;

    GenerateMsgHead(&stFailedResponse,
        m_pstRequestMsg->m_stmsghead().m_uisessionfd(), MSGID_ACCOUNT_LISTROLE_RESPONSE, m_pstRequestMsg->m_stmsghead().m_uin());

    Account_ListRole_Response* pstListRoleResponse = stFailedResponse.mutable_m_stmsgbody()->mutable_m_staccountlistroleresponse();

    pstListRoleResponse->set_iresult(uiResultID);   //������
    pstListRoleResponse->set_uin(m_pstRequestMsg->m_stmsgbody().m_staccountlistrolerequest().uin());
    pstListRoleResponse->set_world(m_pstRequestMsg->m_stmsgbody().m_staccountlistrolerequest().world());

    if (EncodeAndSendCode(CSProtocolEngine,
                          m_pstNetHead, &stFailedResponse, GAME_SERVER_LOTUSACCOUNT) != 0)
    {
        LOGERROR("Failed to send ListRoleResponse to lotus server\n");
        return;
    }

    LOGDETAIL("Send ListRoleResponse to lotus server, result: %d, uin: %u, world: %d, role number: %d\n",uiResultID, 
             pstListRoleResponse->uin(), pstListRoleResponse->world(), pstListRoleResponse->roles_size());
}

