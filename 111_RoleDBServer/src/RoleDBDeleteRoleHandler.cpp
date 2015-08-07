#include <string.h>

#include "AppDef.hpp"
#include "RoleDBLogManager.hpp"
#include "SeqConverter.hpp"
#include "StringUtility.hpp"
#include "RoleDBApp.hpp"

#include "RoleDBDeleteRoleHandler.hpp"

using namespace ServerLib;

//todo jasonxiong2 Ŀǰû��ɾ�����󣬵�����������

char CRoleDBDeleteRoleHandler::m_szQueryString[GameConfig::ROLE_TABLE_SPLIT_FACTOR][512];

CRoleDBDeleteRoleHandler::CRoleDBDeleteRoleHandler(DBClientWrapper* pDatabase)
{
    m_pDatabase = pDatabase;
}

void CRoleDBDeleteRoleHandler::OnClientMsg(GameProtocolMsg* pstRequestMsg,
        SHandleResult* pstHandleResult)
{
    if (!pstRequestMsg || !pstHandleResult)
	{
		return;
	}

    // delete role������Ϣ
    m_pstRequestMsg = pstRequestMsg;
    const DeleteRole_Account_Request& rstReq = m_pstRequestMsg->m_stmsgbody().m_staccountdeleterolerequest();
    
    unsigned int uiUin = rstReq.stroleid().uin();

    TRACE_THREAD(m_iThreadIdx, "Handling DeleteRoleRequest, uin: %u\n", uiUin);

    // delete role��Ӧ��Ϣ
    GameProtocolMsg* pstMsgResp = &(pstHandleResult->stResponseMsg);

    // ��Ӧ��Ϣͷ
    GenerateResponseMsgHead(pstMsgResp, 0, MSGID_ACCOUNT_DELETEROLE_RESPONSE, uiUin);

    // ����uin��worldid��ȡ���ݿ��е�nickname������m_szNickname��
    int iRes = QueryRoleInfo(rstReq.stroleid());
    if (iRes != 0)
    {
        // û���ҵ�Ҫɾ���Ľ�ɫ, ֱ�ӷ��سɹ�
        FillSuccessfulResponse(pstMsgResp);
        return;
    }

    // ����RoleIDɾ����DBRoleInfo�е���Ӧ��¼
    iRes = DeleteRole(rstReq.stroleid());
    if (iRes != 0)
    {
        FillFailedResponse(T_ROLEDB_SQL_EXECUTE_FAILED, pstMsgResp);
        return;
    }

    FillSuccessfulResponse(pstMsgResp);
}

int CRoleDBDeleteRoleHandler::DeleteRole(const RoleID& stRoleID)
{
    //��ȡROLEDB���ݿ��������
    const ONEROLEDBINFO* pstDBConfig = (CRoleDBApp::m_stRoleDBConfigManager).GetOneRoleDBInfoByIndex(m_iThreadIdx);
    if(!pstDBConfig)
    {
        TRACE_THREAD(m_iThreadIdx, "Failed to get roledb config, thread index %d\n", m_iThreadIdx);
        return -1;
    }

    //����Ҫ���������ݿ������Ϣ
    m_pDatabase->SetMysqlDBInfo(pstDBConfig->szDBHost, pstDBConfig->szUserName, pstDBConfig->szUserPasswd, pstDBConfig->szDBName);

    char* pszQueryString = m_szQueryString[m_iThreadIdx];
    int iQueryBuffLen = sizeof(m_szQueryString[m_iThreadIdx])-1;
    int iLength = SAFE_SPRINTF(pszQueryString, iQueryBuffLen, "delete from %s where uin = %u and seq=%u\n", MYSQL_USERINFO_TABLE, stRoleID.uin(), stRoleID.uiseq());

    int iRet = m_pDatabase->ExecuteRealQuery(pszQueryString, iLength, false);
    if(iRet)
    {
        TRACE_THREAD(m_iThreadIdx, "Failed to execute delete sql query, uin %u, ret %d\n", stRoleID.uin(), iRet);
        return iRet;
    }

    DEBUG_THREAD(m_iThreadIdx, "The number of affected rows is %d\n", m_pDatabase->GetAffectedRows());
    return 0;
}

int CRoleDBDeleteRoleHandler::QueryRoleInfo(const RoleID& stRoleID)
{
    //��XML�ļ��ж�ȡ���ݿ�������Ϣ
    const ONEROLEDBINFO* pstDBInfoConfig = (CRoleDBApp::m_stRoleDBConfigManager).GetOneRoleDBInfoByIndex(m_iThreadIdx);
    if(!pstDBInfoConfig)
    {
        TRACE_THREAD(m_iThreadIdx, "Failed to query role info, invalid DB cfg, thread idx %d\n", m_iThreadIdx);
        return -1;
    }

    //����Ҫ���������ݿ������Ϣ
    m_pDatabase->SetMysqlDBInfo(pstDBInfoConfig->szDBHost, pstDBInfoConfig->szUserName, pstDBInfoConfig->szUserPasswd, pstDBInfoConfig->szDBName);

    //��ʼ����ѯ��SQL���
    char* pszQueryString = m_szQueryString[m_iThreadIdx];
    int iQueryBuffLen = sizeof(m_szQueryString[m_iThreadIdx])-1;
    int iLength = SAFE_SPRINTF(pszQueryString, iQueryBuffLen, "select uin from %s where uin=%u and seq=%u\n", MYSQL_USERINFO_TABLE, stRoleID.uin(), stRoleID.uiseq());

    int iRet = m_pDatabase->ExecuteRealQuery(pszQueryString, iLength, true);
    if(iRet)
    {
        TRACE_THREAD(m_iThreadIdx, "Fail to execute sql query, uin %u, ret %d\n", stRoleID.uin(), iRet);
        return T_ROLEDB_SQL_EXECUTE_FAILED;
    }

    //�ж��Ƿ��Ѿ�����Ҫɾ���ļ�¼
    int iNum = m_pDatabase->GetNumberRows();
    if(iNum != 1)
    {
        TRACE_THREAD(m_iThreadIdx, "Wrong select result, uin %u\n", stRoleID.uin());
        return T_ROLEDB_INVALID_RECORD;
    }

    return 0;
}

void CRoleDBDeleteRoleHandler::FillFailedResponse(const unsigned int uiResultID,
        GameProtocolMsg* pstResponseMsg)
{
	if (!pstResponseMsg)
	{
		return;
	}
    
    DeleteRole_Account_Response* pstResp = pstResponseMsg->mutable_m_stmsgbody()->mutable_m_staccountdeleteroleresponse();
    pstResp->set_iresult(uiResultID);

    TRACE_THREAD(m_iThreadIdx, "Info of failed DeleteRoleResponse: result: %d\n",
             uiResultID);
}

void CRoleDBDeleteRoleHandler::FillSuccessfulResponse(GameProtocolMsg* pstResponseMsg)
{
	if (!pstResponseMsg)
	{
		return;
	}

    DeleteRole_Account_Response* pstResp = pstResponseMsg->mutable_m_stmsgbody()->mutable_m_staccountdeleteroleresponse();
    pstResp->set_iresult(T_SERVER_SUCESS);

    TRACE_THREAD(m_iThreadIdx, "Info of DeleteRoleResponse: result: %d, uin: %u\n", T_SERVER_SUCESS, pstResponseMsg->m_stmsghead().m_uin());

    return;
}
