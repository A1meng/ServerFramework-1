#include <stdlib.h>
#include <string.h>

#include "AppDef.hpp"
#include "RoleDBLogManager.hpp"
#include "UnixTime.hpp"
#include "NowTime.hpp"
#include "StringUtility.hpp"
#include "RoleDBApp.hpp"

#include "RoleDBFetchRoleHandler.hpp"

using namespace ServerLib;

char CRoleDBFetchRoleHandler::m_szQueryString[GameConfig::ROLE_TABLE_SPLIT_FACTOR][512];

CRoleDBFetchRoleHandler::CRoleDBFetchRoleHandler(DBClientWrapper* pDatabase)
{
    m_pDatabase = pDatabase;
}

void CRoleDBFetchRoleHandler::OnClientMsg(GameProtocolMsg* pstRequestMsg,
        SHandleResult* pstHandleResult)
{
    if (!pstRequestMsg || !pstHandleResult)
	{
		return;
	}

    // fetch role������Ϣ
    m_pstRequestMsg = pstRequestMsg;

    switch (m_pstRequestMsg->m_stmsghead().m_uimsgid())
    {
    case MSGID_WORLD_FETCHROLE_REQUEST:
        {
            OnFetchRoleRequest(pstHandleResult);
            break;
        }

    default:
        {
            break;
        }
    }
}

void CRoleDBFetchRoleHandler::OnFetchRoleRequest(SHandleResult* pstHandleResult)
{
	if (!pstHandleResult)
	{
		return;
	}

    World_FetchRole_Request* pstReq = m_pstRequestMsg->mutable_m_stmsgbody()->mutable_m_stworld_fetchrole_request();

    unsigned int uiUin = pstReq->stroleid().uin();
    TRACE_THREAD(m_iThreadIdx, "Handling FetchRoleRequest, uin: %u\n", uiUin);

    // fetch role��Ӧ��Ϣ
    GameProtocolMsg* pstMsgResp = &(pstHandleResult->stResponseMsg);

    // ��Ӧ��Ϣͷ
    GenerateResponseMsgHead(pstMsgResp, 0, MSGID_WORLD_FETCHROLE_RESPONSE, uiUin);

    // ��Ӧ��Ϣ��
    World_FetchRole_Response* pstResp = pstMsgResp->mutable_m_stmsgbody()->mutable_m_stworld_fetchrole_response();
    pstResp->mutable_stroleid()->CopyFrom(pstReq->stroleid());
    pstResp->set_bislogin(pstReq->bislogin());

    // ����uin��ѯ��ҽ�ɫ��Ϣ
    int iRet = QueryRole(pstReq->stroleid(), *(pstResp->mutable_stuserinfo()));
    if (iRet != 0)
    {
		TRACE_THREAD(m_iThreadIdx, "Query Role info failed, uin:%u iRes:%d\n", uiUin, iRet);
        FillFailedResponse(T_ROLEDB_SQL_EXECUTE_FAILED, pstMsgResp);
        return;
    }

    //todo jasonxiong ���ʺŹ�����ʱ��֧�֣��Ⱥ���������Ҫʱ������
    /*
	time_t tNow = time(NULL);
	if (tNow < pstFetchRoleResponse->m_stDBRoleInfo.fForbidTime)
	{
		TRACE_THREAD(m_iThreadIdx, "uin:%u is forbid login to time:%d\n", pstRoleID->m_uiUin, pstFetchRoleResponse->m_stDBRoleInfo.fForbidTime);
        FillFailedResponse(EQEC_UinInBlackList, pstResponseMsg);
		return;
	}
    */

    // ���ɹ��ظ�
    FillSuccessfulResponse(pstMsgResp);
}

int CRoleDBFetchRoleHandler::QueryRole(const RoleID& stRoleID, GameUserInfo& rstDBUserInfo)
{
    //��ȡROLEDB���ݿ������
    const ONEROLEDBINFO* pstDBConfig = (CRoleDBApp::m_stRoleDBConfigManager).GetOneRoleDBInfoByIndex(m_iThreadIdx);
    if(!pstDBConfig)
    {
        TRACE_THREAD(m_iThreadIdx, "Failed to get roledb config, thread index %d\n", m_iThreadIdx);
        return -1;
    }

    //����Ҫ���������ݿ������Ϣ
    m_pDatabase->SetMysqlDBInfo(pstDBConfig->szDBHost, pstDBConfig->szUserName, pstDBConfig->szUserPasswd, pstDBConfig->szDBName);

    rstDBUserInfo.set_uin(stRoleID.uin());
    rstDBUserInfo.set_uiseq(stRoleID.uiseq());

    //����SQL���
    char* pszQueryString = m_szQueryString[m_iThreadIdx];
    int iLength = SAFE_SPRINTF(pszQueryString, sizeof(m_szQueryString[m_iThreadIdx])-1, 
                               "select uin,seq,base_info,quest_info,item_info,fight_info,friend_info,"
                               "reserved1,reserved2 from %s where uin=%u and seq=%u ", 
                               MYSQL_USERINFO_TABLE, stRoleID.uin(), stRoleID.uiseq());

    //ִ��
    int iRet = m_pDatabase->ExecuteRealQuery(pszQueryString, iLength, true);
    if(iRet)
    {
        TRACE_THREAD(m_iThreadIdx, "Fail to execute select sql query, uin %u\n", stRoleID.uin());
        return iRet;
    }

    //�������
    int iRowNum = m_pDatabase->GetNumberRows();
    if(iRowNum != 1)
    {
        TRACE_THREAD(m_iThreadIdx, "Wrong result, invalid rows %d, uin %u\n", iRowNum, stRoleID.uin());
        return T_ROLEDB_INVALID_RECORD;
    }

    MYSQL_ROW pstResult = NULL;
    unsigned long* pLengths = NULL;
    unsigned int uFields = 0;

    iRet = m_pDatabase->FetchOneRow(pstResult, pLengths, uFields);
    if(iRet)
    {
        TRACE_THREAD(m_iThreadIdx, "Failed to fetch rows, uin %u, ret %d\n", stRoleID.uin(), iRet);
        return iRet;
    }

    //�ж�uFields�Ƿ����
    if(uFields != MYSQL_USERINFO_FIELDS)
    {
        TRACE_THREAD(m_iThreadIdx, "Wrong result, real fields %u, needed %u\n", uFields, MYSQL_USERINFO_FIELDS);
        return T_ROLEDB_INVALID_RECORD;
    }

    //�ӽ���н�����Ҫ���ֶ�

    //�ֶ�1��uin, �ֶ�2��seq, ������

    //�ֶ�3��base_info
    rstDBUserInfo.set_strbaseinfo(pstResult[2], pLengths[2]);

    //�ֶ�4��quest_info
    rstDBUserInfo.set_strquestinfo(pstResult[3], pLengths[3]);

    //�ֶ�5��item_info
    rstDBUserInfo.set_striteminfo(pstResult[4], pLengths[4]);

    //�ֶ�6��fight info
    rstDBUserInfo.set_strfightinfo(pstResult[5], pLengths[5]);

    //�ֶ�7��friend_info
    rstDBUserInfo.set_strfriendinfo(pstResult[6], pLengths[6]);

    //�ֶ�8�Ǳ����ֶ�reserved1
    rstDBUserInfo.set_strreserved1(pstResult[7], pLengths[7]);

    //�ֶ�9�Ǳ����ֶ�reserved2
    rstDBUserInfo.set_strreserved2(pstResult[8], pLengths[8]);

    return 0;
}

void CRoleDBFetchRoleHandler::FillFailedResponse(const unsigned int uiResultID, GameProtocolMsg* pstResponseMsg)
{
	if (!pstResponseMsg)
	{
		return;
	}

    World_FetchRole_Response* pstResp = pstResponseMsg->mutable_m_stmsgbody()->mutable_m_stworld_fetchrole_response();
    pstResp->set_iresult(uiResultID);

    TRACE_THREAD(m_iThreadIdx, "Info of failed FetchRoleResponse: result: %u, uin %u\n", uiResultID, pstResp->stroleid().uin());
}

void CRoleDBFetchRoleHandler::FillSuccessfulResponse(GameProtocolMsg* pstResponseMsg)
{
    if(!pstResponseMsg)
    {
        return;
    }

    World_FetchRole_Response* pstResp = pstResponseMsg->mutable_m_stmsgbody()->mutable_m_stworld_fetchrole_response();
    pstResp->set_iresult(T_SERVER_SUCESS);

    const GameUserInfo& rstDBUserInfo = pstResp->stuserinfo();
    TRACE_THREAD(m_iThreadIdx, "Info of FetchRoleResponse: result: %u, uin: %u, seq: %u\n",
             T_SERVER_SUCESS,
             rstDBUserInfo.uin(),
             rstDBUserInfo.uiseq());
    return;
}


