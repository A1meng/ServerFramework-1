#include <assert.h>
#include <arpa/inet.h>

#include "GameProtocol.hpp"
#include "LogAdapter.hpp"
#include "UpdateAccountHandler.hpp"
#include "SessionObj.hpp"
#include "RegAuthSingleton.hpp"
#include "StringUtility.hpp"
#include "ConfigMgr.hpp"
#include "RegAuthApp.hpp"
#include "PasswordEncryptionUtility.hpp"
#include "PlatformDefine.hpp"

using namespace ServerLib;

CUpdateAccountHandler::CUpdateAccountHandler()
{
}

int CUpdateAccountHandler::CheckParam()
{
    //todo jasonxiong ������������Ҫ���ݲ߻���������ʺź�������ַ��Ϸ��Լ��

    return 0;
}

void CUpdateAccountHandler::OnClientMsg(TNetHead_V2* pstNetHead,
        GameProtocolMsg* pstMsg, SHandleResult* pstResult)
{
    // ��ʹ��Result
    ASSERT_AND_LOG_RTN_VOID(pstNetHead);
    ASSERT_AND_LOG_RTN_VOID(pstMsg);

    m_pstNetHead = pstNetHead;
    m_pstRequestMsg = pstMsg;

    switch (m_pstRequestMsg->m_stmsghead().m_uimsgid())
    {
        case MSGID_REGAUTH_UPDATE_REQUEST:
            {
                //����ƽ̨�ʺŵ�����
                OnRequestUpdateAccount();
                return;
            }
            break;

        case MSGID_ACCOUNTDB_UPDATE_RESPONSE:
            {
                //����ƽ̨�ʺŵķ���
                OnResponseUpdateAccount();
                return;
            }
            break;

        default:
            {

            }
            break;
    }

    return;
}

int CUpdateAccountHandler::OnRequestUpdateAccount()
{
    // ���������Ϣ���Ƿ���ڷǷ��ֶ�
    if (CheckParam() != 0)
    {
        TRACESVR("Invalid parameter found in the request\n");
        SendFailedResponseToLotus(T_COMMON_SYSTEM_PARA_ERR);
        return -1;
    }

    RegAuth_UpdateAccount_Request* pstReq = m_pstRequestMsg->mutable_m_stmsgbody()->mutable_m_stregauth_updateaccount_request();

    //todo jasonxiong ����������������ƽ̨�Ļ��������Ҫ�����Ӧƽ̨����֤

    TRACESVR("Handling RegAccountRequest from lotus server, account: %s, type: %d\n", 
             pstReq->staccountid().straccount().c_str(), pstReq->staccountid().iaccounttype());

    // ���session�Ƿ��Ѿ����ڣ�session�����Ƿ񵽴�����
    int iRes = SessionManager->CheckSession(*m_pstNetHead);
    if (iRes != T_SERVER_SUCESS)
    {
        SendFailedResponseToLotus(iRes);
        return -4;
    }

    time_t tmNow = time(NULL); // �����������ʱ��

    // ����NetHead����session
    CSessionObj* pSessionObj = SessionManager->CreateSession(*m_pstNetHead);
    ASSERT_AND_LOG_RTN_INT(pSessionObj);
    pSessionObj->SetCreatedTime(&tmNow);

    m_uiSessionFD = pSessionObj->GetSessionFD();
    m_unValue = pSessionObj->GetValue();
    TRACESVR("sockfd: %u, value: %d\n", m_uiSessionFD, m_unValue);

    char szTime[32] = "";
    pSessionObj->GetCreatedTime(szTime, sizeof(szTime));
    TRACESVR("session cache created time: %s", szTime); // ����: Wed Sep 29 10:59:46 2010

    // ת�����µ������ACCOUNTDB
    SendUpdateRequestToAccountDB();

    return T_SERVER_SUCESS;
}

int CUpdateAccountHandler::OnResponseUpdateAccount()
{
    //��װ��Ϣ���ظ��ͻ���
    static GameProtocolMsg stResponseMsg;

    //������Ϣͷ
    GenerateMsgHead(&stResponseMsg, m_pstRequestMsg->m_stmsghead().m_uisessionfd(), MSGID_REGAUTH_UPDATE_RESPONSE, 0);

    //��ȡAccountDB����Ӧ��Ϣ
    const AccountDB_UpdateAccount_Response& rstResp = m_pstRequestMsg->m_stmsgbody().m_staccountdb_update_response();

    //������Ϣ��
    RegAuth_UpdateAccount_Response* pstResp = stResponseMsg.mutable_m_stmsgbody()->mutable_m_stregauth_updateaccount_response();
    pstResp->set_iresult(rstResp.iresult());

    if(EncodeAndSendCodeBySession(CSProtocolEngine, m_pstRequestMsg->m_stmsghead().m_uisessionfd(), &stResponseMsg, GAME_SERVER_LOTUSREGAUTH) != 0)
    {
        TRACESVR("Failed to send update account response to lotus server!\n");
        return -3;
    }

    //todo jasonxiong ��ߺ����Ƿ���Ҫ����OSS��Ӫ��־���Ⱥ�����ȷ��

    return T_SERVER_SUCESS;
}

void CUpdateAccountHandler::SendUpdateRequestToAccountDB()
{
    static GameProtocolMsg stRequestMsg;

    const RegAuth_UpdateAccount_Request& rstReq = m_pstRequestMsg->m_stmsgbody().m_stregauth_updateaccount_request();

    //���ɸ��µ���Ϣͷ
    GenerateMsgHead(&stRequestMsg, m_uiSessionFD, MSGID_ACCOUNTDB_UPDATE_REQUEST, GetAccountHash(rstReq.staccountid().straccount()));

    //����ԭʼ������
    static char szEncryptPasswd[256] = {0};
    int iEncryptBuffLen = sizeof(szEncryptPasswd)-1;
    int iRet = CPasswordEncryptionUtility::DoPasswordEncryption(rstReq.strpassword().c_str(), rstReq.strpassword().size(), szEncryptPasswd, iEncryptBuffLen);
    if(iRet)
    {
        TRACESVR("Failed to encrypt account password, account: %s, password: %s\n", rstReq.staccountid().straccount().c_str(), rstReq.strpassword().c_str());
        return;
    }

    //���ɸ��µ���Ϣ��
    AccountDB_UpdateAccount_Request* pstUpdateReq = stRequestMsg.mutable_m_stmsgbody()->mutable_m_staccountdb_update_request();
    pstUpdateReq->mutable_staccountid()->CopyFrom(rstReq.staccountid());
    pstUpdateReq->set_strpassword(szEncryptPasswd, iEncryptBuffLen);

    //ת����Ϣ��AccountDBServer
    if (EncodeAndSendCode(SSProtocolEngine, NULL, &stRequestMsg, GAME_SERVER_ACCOUNTDB) != 0)
    {
        TRACESVR("Failed to send Update request to Account DB server\n");
        return;
    }

    TRACESVR("Send UpdateAccountRequest to Account DB server\n");

    return;
}

void CUpdateAccountHandler::SendFailedResponseToLotus(const unsigned int uiResultID)
{
    static GameProtocolMsg stFailedResponse;
    GenerateMsgHead(&stFailedResponse, m_pstRequestMsg->m_stmsghead().m_uisessionfd(), MSGID_REGAUTH_UPDATE_RESPONSE, 0);

    RegAuth_UpdateAccount_Response* pstResp = stFailedResponse.mutable_m_stmsgbody()->mutable_m_stregauth_updateaccount_response();
    pstResp->set_iresult(uiResultID);

    if (EncodeAndSendCode(CSProtocolEngine, m_pstNetHead, &stFailedResponse, GAME_SERVER_LOTUSREGAUTH) != 0)
    {
        TRACESVR("Failed to send failed UpdateAccountResponse to lotus server\n");
        return;
    }

    TRACESVR("Send failed UpdateAccountResponse to lotus server, account: %s, result: %d\n",
             m_pstRequestMsg->m_stmsgbody().m_stregauth_updateaccount_request().staccountid().straccount().c_str(), pstResp->iresult());

    return;
}

