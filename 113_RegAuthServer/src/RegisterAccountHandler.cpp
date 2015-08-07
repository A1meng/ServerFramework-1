#include <assert.h>
#include <arpa/inet.h>

#include "GameProtocol.hpp"
#include "LogAdapter.hpp"
#include "SessionObj.hpp"
#include "RegAuthSingleton.hpp"
#include "TimeStampConverter.hpp"
#include "StringUtility.hpp"
#include "ConfigMgr.hpp"
#include "RegAuthApp.hpp"
#include "PasswordEncryptionUtility.hpp"
#include "PlatformDefine.hpp"
#include "RegAuthBillLog.hpp"

#include "RegisterAccountHandler.hpp"

using namespace ServerLib;

CRegisterAccountHandler::CRegisterAccountHandler()
{
}

int CRegisterAccountHandler::CheckParam()
{
    //todo jasonxiong ������������Ҫ���ݲ߻���������ʺź�������ַ��Ϸ��Լ��

    return 0;
}

void CRegisterAccountHandler::OnClientMsg(TNetHead_V2* pstNetHead,
        GameProtocolMsg* pstMsg, SHandleResult* pstResult)
{
    // ��ʹ��Result
    ASSERT_AND_LOG_RTN_VOID(pstNetHead);
    ASSERT_AND_LOG_RTN_VOID(pstMsg);

    m_pstNetHead = pstNetHead;
    m_pstRequestMsg = pstMsg;

    switch (m_pstRequestMsg->m_stmsghead().m_uimsgid())
    {
        case MSGID_REGAUTH_REGACCOUNT_REQUEST:
            {
                //ע��ƽ̨�ʺŵ�����
                OnRequestRegAccount();
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

int CRegisterAccountHandler::OnRequestRegAccount()
{
    // ���������Ϣ���Ƿ���ڷǷ��ֶ�
    if (CheckParam() != 0)
    {
        TRACESVR("Invalid parameter found in the request\n");
        SendFailedResponseToLotus(T_COMMON_SYSTEM_PARA_ERR);
        return -1;
    }

    const RegAuth_RegAccount_Request& rstReq = m_pstRequestMsg->m_stmsgbody().m_stregauth_regaccount_request();
    TRACESVR("Handling RegAccountRequest from lotus server, account: %s, type: %d\n",
             rstReq.staccountid().straccount().c_str(), rstReq.staccountid().iaccounttype());

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

    unsigned int uiSessionFd = GenerateTimeStamp(m_uiSessionFD, m_unValue);

    //ֱ��ת��ע�������AccountDB
    CAccountDBAddHandler::AddAccount(uiSessionFd, rstReq.staccountid(), 1, rstReq.strpassword());

    return T_SERVER_SUCESS;
}

void CRegisterAccountHandler::SendFailedResponseToLotus(const unsigned int uiResultID)
{
    GameProtocolMsg stFailedResponse;
    GenerateMsgHead(&stFailedResponse, m_pstRequestMsg->m_stmsghead().m_uisessionfd(), MSGID_REGAUTH_REGACCOUNT_RESPONSE, 0);

    RegAuth_RegAccount_Response* pstResp = stFailedResponse.mutable_m_stmsgbody()->mutable_m_stregauth_regaccount_response();
    pstResp->set_iresult(uiResultID); // ������

    if (EncodeAndSendCode(CSProtocolEngine, m_pstNetHead, &stFailedResponse, GAME_SERVER_LOTUSREGAUTH) != 0)
    {
        TRACESVR("Failed to send failed RegAccountResponse to lotus server\n");
        return;
    }

    TRACESVR("Send failed RegAccountResponse to lotus server, account: %s, result: %d\n",
             m_pstRequestMsg->m_stmsgbody().m_stregauth_regaccount_request().staccountid().straccount().c_str(), uiResultID);

    return;
}

