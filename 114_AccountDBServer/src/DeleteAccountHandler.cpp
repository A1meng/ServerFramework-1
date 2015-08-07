#include <string.h>

#include "AppDef.hpp"
#include "AccountDBLogManager.hpp"
#include "StringUtility.hpp"
#include "AccountDBApp.hpp"

#include "DeleteAccountHandler.hpp"

using namespace ServerLib;

//ִ�е�SQL���
char CDeleteAccountHandler::m_szQueryString[GameConfig::ACCOUNT_TABLE_SPLIT_FACTOR][256];

CDeleteAccountHandler::CDeleteAccountHandler(DBClientWrapper* pDatabase)
{
    m_pDatabase = pDatabase;
}

void CDeleteAccountHandler::OnClientMsg(GameProtocolMsg* pstRequestMsg,
        SHandleResult* pstHandleResult)
{
    if (!pstRequestMsg || !pstHandleResult)
	{
		return;
	}

    //Delete������Ҫ�ذ�
    //pstHandleResult->iNeedResponse = true;  //��Ҫ�ظ�

    //Delete Account ��������Ϣ
    m_pstRequestMsg = pstRequestMsg;
    const AccountDB_DeleteAccount_Request& rstDeleteReq = m_pstRequestMsg->m_stmsgbody().m_staccountdb_delete_request();

    TRACE_THREAD(m_iThreadIdx, "Handling DeleteAccountRequest, account: %s, type %d\n", rstDeleteReq.staccountid().straccount().c_str(), rstDeleteReq.staccountid().iaccounttype());

    // ��Ӧ��Ϣͷ
    GenerateResponseMsgHead(&pstHandleResult->stResponseMsg, 
                            m_pstRequestMsg->m_stmsghead().m_uisessionfd(),
                            MSGID_ACCOUNTDB_DELETE_RESPONSE, 
                            0);

    //����AccountID��AccountTypeɾ����¼
    int iRet = DeleteAccount(rstDeleteReq.staccountid());
    if (iRet)
    {
        FillFailedResponse(iRet, &pstHandleResult->stResponseMsg);
        return;
    }

    FillSuccessfulResponse(&pstHandleResult->stResponseMsg);

    return;
}

//ɾ��Account�ʺ�
int CDeleteAccountHandler::DeleteAccount(const AccountID& stAccountID)
{
    //��ȡACCOUNTDB���ݿ��������
    const ONEACCOUNTDBINFO* pstDBConfig = (CAccountDBApp::m_stAccountDBConfigManager).GetOneAccountDBInfoByIndex(m_iThreadIdx);
    if(!pstDBConfig)
    {
        TRACE_THREAD(m_iThreadIdx, "Failed to get account db config, thread index %d\n", m_iThreadIdx);
        return -1;
    }

    //����Ҫ���������ݿ������Ϣ
    m_pDatabase->SetMysqlDBInfo(pstDBConfig->szDBHost, pstDBConfig->szUserName, pstDBConfig->szUserPasswd, pstDBConfig->szDBName);

    char* pszQueryString = m_szQueryString[m_iThreadIdx];

    //����ɾ����SQL���
    int iLength = SAFE_SPRINTF(pszQueryString, sizeof(m_szQueryString[m_iThreadIdx])-1, "delete from %s where accountID= '%s' and accountType=%d", 
                 MYSQL_ACCOUNTINFO_TABLE, stAccountID.straccount().c_str(), stAccountID.iaccounttype());

    int iRet = m_pDatabase->ExecuteRealQuery(pszQueryString, iLength, false);
    if(iRet)
    {
        TRACE_THREAD(m_iThreadIdx, "Failed to execute delete sql query, account %s, ret %d\n", stAccountID.straccount().c_str(), iRet);
        return iRet;
    }

    DEBUG_THREAD(m_iThreadIdx, "The number of affected rows is %d\n", m_pDatabase->GetAffectedRows());

    return 0;
}

void CDeleteAccountHandler::FillFailedResponse(const unsigned int uiResultID,
        GameProtocolMsg* pstResponseMsg)
{
	if (!pstResponseMsg)
	{
		return;
	}
    
    AccountDB_DeleteAccount_Response* pstResp = pstResponseMsg->mutable_m_stmsgbody()->mutable_m_staccountdb_delete_response();
    pstResp->set_iresult(uiResultID);

    TRACE_THREAD(m_iThreadIdx, "Info of failed DeleteAccountResponse: result: %d\n", uiResultID);
}

void CDeleteAccountHandler::FillSuccessfulResponse(GameProtocolMsg* pstResponseMsg)
{
	if (!pstResponseMsg)
	{
		return;
	}

    AccountDB_DeleteAccount_Response* pstResp = pstResponseMsg->mutable_m_stmsgbody()->mutable_m_staccountdb_delete_response();
    pstResp->set_iresult(T_SERVER_SUCESS);

    const AccountDB_DeleteAccount_Request& rstReq = m_pstRequestMsg->m_stmsgbody().m_staccountdb_delete_request();

    TRACE_THREAD(m_iThreadIdx, "Info of DeleteAccountResponse: result: %d, account: %s, account type %d\n", T_SERVER_SUCESS, rstReq.staccountid().straccount().c_str(), rstReq.staccountid().iaccounttype());

    return;
}
