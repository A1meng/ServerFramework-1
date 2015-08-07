#include <assert.h>

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
#include "SessionKeyUtility.hpp"
#include "RegAuthBillLog.hpp"

using namespace ServerLib;

CAccountDBAddHandler::CAccountDBAddHandler()
{
}

void CAccountDBAddHandler::OnClientMsg(TNetHead_V2* pstNetHead, GameProtocolMsg* pstMsg, SHandleResult* pstResult)
{
    // ��ʹ��Result
    ASSERT_AND_LOG_RTN_VOID(pstNetHead);
    ASSERT_AND_LOG_RTN_VOID(pstMsg);

    m_pstNetHead = pstNetHead;
    m_pstRequestMsg = pstMsg;

    switch (m_pstRequestMsg->m_stmsghead().m_uimsgid())
    {
        case MSGID_ACCOUNTDB_ADDACCOUNT_RESPONSE:
            {
                //�������ʺŵķ���
                OnResponseAccountDBAdd();
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

int CAccountDBAddHandler::OnResponseAccountDBAdd()
{
    //AccountDB�����ʺŵķ���
    const AccountDB_AddAccount_Response& rstResp = m_pstRequestMsg->m_stmsgbody().m_staccountdb_addaccount_response();

    SendResponseToLotus(m_pstRequestMsg->m_stmsghead().m_uisessionfd(), rstResp.iresult());

    return T_SERVER_SUCESS;
}

//�������ʺ�
void CAccountDBAddHandler::AddAccount(unsigned uiSessionFd, const AccountID& stAccountID, int iWorldID, const std::string& strPasswd)
{
    static GameProtocolMsg stMsg;

    //������Ϣͷ
    GenerateMsgHead(&stMsg, uiSessionFd, MSGID_ACCOUNTDB_ADDACCOUNT_REQUEST, GetAccountHash(stAccountID.straccount()));

    //AccountDB�������ʺŵ�����
    AccountDB_AddAccount_Request* pstReq = stMsg.mutable_m_stmsgbody()->mutable_m_staccountdb_addaccount_request();
    pstReq->mutable_staccountid()->CopyFrom(stAccountID);
    pstReq->set_iworldid(iWorldID);

    //��������
    char szEncryptPasswd[256] = {0};
    int iEncryptBuffLen = sizeof(szEncryptPasswd);

    int iRet = CPasswordEncryptionUtility::DoPasswordEncryption(strPasswd.c_str(), strPasswd.size(), szEncryptPasswd, iEncryptBuffLen);
    if(iRet)
    {
        TRACESVR("Failed to encrypt account password, account: %s, password: %s\n", stAccountID.straccount().c_str(), strPasswd.c_str());
        return;
    }

    //��������Ϊ���ܺ������
    pstReq->set_strpassword(szEncryptPasswd, iEncryptBuffLen);

    //ת����Ϣ��AccountDBServer
    if(EncodeAndSendCode(SSProtocolEngine, NULL, &stMsg, GAME_SERVER_ACCOUNTDB) != 0)
    {
        TRACESVR("Failed to send add account request to Account DB server\n");
        return;
    }

    LOGDEBUG("Send add account request to Account DB server\n");

    return;
}

void CAccountDBAddHandler::SendResponseToLotus(unsigned int uiSessionFd, const unsigned int uiResultID)
{
    GameProtocolMsg stResponse;
    GenerateMsgHead(&stResponse, m_pstRequestMsg->m_stmsghead().m_uisessionfd(), MSGID_REGAUTH_REGACCOUNT_RESPONSE, 0);

    //������Ϣ��
    RegAuth_RegAccount_Response* pstResp = stResponse.mutable_m_stmsgbody()->mutable_m_stregauth_regaccount_response();
    pstResp->set_iresult(uiResultID);

    //��ȡsession��NetHead
    CSessionObj* pSessionObj = SessionManager->GetSession(uiSessionFd);
    ASSERT_AND_LOG_RTN_VOID(pSessionObj);

    TNetHead_V2* pstNetHead = pSessionObj->GetNetHead();
    ASSERT_AND_LOG_RTN_VOID(pstNetHead);

    if (EncodeAndSendCode(CSProtocolEngine, pstNetHead, &stResponse, GAME_SERVER_LOTUSREGAUTH) != 0)
    {
        TRACESVR("Failed to send RegAccountResponse to lotus server\n");
        return;
    }

    TRACESVR("Send RegAccountResponse to lotus server, result: %d\n", uiResultID);

    return;
}

