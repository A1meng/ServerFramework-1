#include <string.h>
#include "RoleDBLogManager.hpp"
#include "StringUtility.hpp"
#include "RoleDBApp.hpp"
#include "AppDef.hpp"

#include "RoleDBUpdateRoleHandler.hpp"

using namespace ServerLib;

//SQL����ַ���������
char CRoleDBUpdateRoleHandler::m_szQueryString[GameConfig::ROLE_TABLE_SPLIT_FACTOR][51200];

CRoleDBUpdateRoleHandler::CRoleDBUpdateRoleHandler(DBClientWrapper* pDatabase)
{ 
    m_pDatabase = pDatabase;
}

void CRoleDBUpdateRoleHandler::OnClientMsg(GameProtocolMsg* pstRequestMsg,
        SHandleResult* pstHandleResult)
{
    if (!pstRequestMsg || !pstHandleResult)
	{
		return;
	}

    // update role������Ϣ
    m_pstRequestMsg = pstRequestMsg;

    switch (pstRequestMsg->m_stmsghead().m_uimsgid())
    {
    case MSGID_WORLD_UPDATEROLE_REQUEST:
        {
            OnUpdateRoleRequest(pstHandleResult);
            break;
        }

    default:
        {
            break;
        }
    }
}

void CRoleDBUpdateRoleHandler::OnUpdateRoleRequest(SHandleResult* pstHandleResult)
{
	World_UpdateRole_Request* pstReq = m_pstRequestMsg->mutable_m_stmsgbody()->mutable_m_stworld_updaterole_request();

    TRACE_THREAD(m_iThreadIdx, "Handling UpdateRoleRequest, uin: %u"
             "need response: %d\n", 
		pstReq->stroleid().uin(), pstReq->bneedresponse());

    pstHandleResult->iNeedResponse = pstReq->bneedresponse(); // �Ƿ���Ҫ�ظ�

    // update role��Ӧ��Ϣ
    GameProtocolMsg* pstResponseMsg = &(pstHandleResult->stResponseMsg);

    // ��Ӧ��Ϣͷ
    GenerateResponseMsgHead(pstResponseMsg, 0, MSGID_WORLD_UPDATEROLE_RESPONSE, pstReq->stroleid().uin());

    // ��Ӧ��Ϣ��
	World_UpdateRole_Response* pstResp = pstResponseMsg->mutable_m_stmsgbody()->mutable_m_stworld_updaterole_response();
	pstResp->mutable_stroleid()->CopyFrom(pstReq->stroleid());

	int iRes = UpdateRole(pstReq);
	if (iRes != 0)
	{
		TRACE_THREAD(m_iThreadIdx, "Failed to execute db query\n");
		FillFailedResponse(T_ROLEDB_SQL_EXECUTE_FAILED, pstResponseMsg);
		return;
	}

    FillSuccessfulResponse(pstResponseMsg);
}

int CRoleDBUpdateRoleHandler::UpdateRole(const World_UpdateRole_Request* pstUpdateRoleRequest)
{
    if (!pstUpdateRoleRequest)
	{
		return -1;
	}

    //��ȡ���ӵ����ݿ���ص�����
    const ONEROLEDBINFO* pstDBConfig = (CRoleDBApp::m_stRoleDBConfigManager).GetOneRoleDBInfoByIndex(m_iThreadIdx);
    if(!pstDBConfig)
    {
        TRACE_THREAD(m_iThreadIdx, "Failed to get roledb config, invalid thread index %d\n", m_iThreadIdx);
        return -2;
    }

    //����Ҫ���������ݿ������Ϣ
    m_pDatabase->SetMysqlDBInfo(pstDBConfig->szDBHost, pstDBConfig->szUserName, pstDBConfig->szUserPasswd, pstDBConfig->szDBName);

    //��ʼ��SQL���
    int iLength = 0;
    char* pszQueryString = m_szQueryString[m_iThreadIdx];
    int iRet = GenerateQueryString(*pstUpdateRoleRequest, pszQueryString, sizeof(m_szQueryString[m_iThreadIdx])-1, iLength);
    if(iRet)
    {
        TRACE_THREAD(m_iThreadIdx, "Failed to generate update query sql string, ret %d\n", iRet);
        return iRet;
    }

    //ִ��
    iRet = m_pDatabase->ExecuteRealQuery(pszQueryString, iLength, false);
    if(iRet)
    {
        TRACE_THREAD(m_iThreadIdx, "Fail to execute select sql query, uin %u\n", pstUpdateRoleRequest->stroleid().uin());
        return iRet;
    }

	DEBUG_THREAD(m_iThreadIdx, "The number of affected rows is %d\n", m_pDatabase->GetAffectedRows());

	return 0;
}

int CRoleDBUpdateRoleHandler::GenerateQueryString(const World_UpdateRole_Request& rstUpdateReq, char* pszBuff, int iBuffLen, int& iLength)
{
    if(!pszBuff)
    {
        TRACE_THREAD(m_iThreadIdx, "Fail to generate update query string, invlaid buff pointer!\n");
        return T_ROLEDB_SYSTEM_PARA_ERR;
    }

    memset(pszBuff, 0, iBuffLen);

    SAFE_SPRINTF(pszBuff, iBuffLen-1, "update %s set ", MYSQL_USERINFO_TABLE);
    char* pEnd = pszBuff + strlen(pszBuff);

    MYSQL& stDBConn = m_pDatabase->GetCurMysqlConn();

    const GameUserInfo& rstUserInfo = rstUpdateReq.stuserinfo();

    //1.��һ�����Ϣ
    if(rstUserInfo.strbaseinfo().size() != 0)
    {
        SAFE_SPRINTF(pEnd, iBuffLen-1, "base_info=");
        pEnd += strlen("base_info=");

        //��һ�����Ϣ base_info
        *pEnd++ = '\'';
        pEnd += mysql_real_escape_string(&stDBConn, pEnd, rstUserInfo.strbaseinfo().c_str(), rstUserInfo.strbaseinfo().size());
        *pEnd++ = '\'';
    }

    //2.���������Ϣ
    if(rstUserInfo.strquestinfo().size() != 0)
    {
        *pEnd++ = ',';

        SAFE_SPRINTF(pEnd, iBuffLen-1, "quest_info=");

        pEnd += strlen("quest_info=");

        //��ҵ�������Ϣ�ֶ�
        *pEnd++ = '\'';
        pEnd += mysql_real_escape_string(&stDBConn, pEnd, rstUserInfo.strquestinfo().c_str(), rstUserInfo.strquestinfo().size());
        *pEnd++ = '\'';
    }

    //3.��ҵ���Ʒ��Ϣ
    if(rstUserInfo.striteminfo().size() != 0)
    {
        *pEnd++ = ',';

        SAFE_SPRINTF(pEnd, iBuffLen-1, "item_info=");

        pEnd += strlen("item_info=");

        //��ҵ���Ʒ��Ϣ item_info
        *pEnd++ = '\'';
        pEnd += mysql_real_escape_string(&stDBConn, pEnd, rstUserInfo.striteminfo().c_str(), rstUserInfo.striteminfo().size());
        *pEnd++ = '\'';
    }

    //4.��ҵ�ս����Ϣ
    if(rstUserInfo.strfightinfo().size() != 0)
    {
        *pEnd++ = ',';

        SAFE_SPRINTF(pEnd, iBuffLen-1, "fight_info=");

        pEnd += strlen("fight_info=");

        //��ҵ�ս����Ϣ
        *pEnd++ = '\'';
        pEnd += mysql_real_escape_string(&stDBConn, pEnd, rstUserInfo.strfightinfo().c_str(), rstUserInfo.strfightinfo().size());
        *pEnd++ = '\'';
    }

    //5.��ҵĺ�����Ϣ
    if(rstUserInfo.strfriendinfo().size() != 0)
    {
        *pEnd++ = ',';

        SAFE_SPRINTF(pEnd, iBuffLen-1, "friend_info=");

        pEnd += strlen("friend_info=");

        //��ҵĺ�����Ϣ
        *pEnd++ = '\'';
        pEnd += mysql_real_escape_string(&stDBConn, pEnd, rstUserInfo.strfriendinfo().c_str(), rstUserInfo.strfriendinfo().size());
        *pEnd++ = '\'';
    }

    //6.��ҵı����ֶ�1
    if(rstUserInfo.strreserved1().size() != 0)
    {        
        *pEnd++ = ',';

        SAFE_SPRINTF(pEnd, iBuffLen-1, "reserved1=");

        pEnd += strlen("reserved1=");

        //�����ֶ�1�ĸ���
        *pEnd++ = '\'';
        pEnd += mysql_real_escape_string(&stDBConn, pEnd, rstUserInfo.strreserved1().c_str(), rstUserInfo.strreserved1().size());
        *pEnd++ = '\'';
    }

    //7.��ҵı����ֶ�2
    if(rstUserInfo.strreserved2().size() != 0)
    {        
        *pEnd++ = ',';

        SAFE_SPRINTF(pEnd, iBuffLen-1, "reserved2=");

        pEnd += strlen("reserved2=");

        //�����ֶ�2�ĸ���
        *pEnd++ = '\'';
        pEnd += mysql_real_escape_string(&stDBConn, pEnd, rstUserInfo.strreserved2().c_str(), rstUserInfo.strreserved2().size());
        *pEnd++ = '\'';
    }

    pEnd += SAFE_SPRINTF(pEnd, iBuffLen-1, " where uin = %u and seq = %u\n", rstUpdateReq.stroleid().uin(), rstUpdateReq.stroleid().uiseq());
    
    iLength = pEnd - pszBuff;

    return T_SERVER_SUCESS;
}

void CRoleDBUpdateRoleHandler::FillFailedResponse(const unsigned int uiResultID,
        GameProtocolMsg* pstResponseMsg)
{
	World_UpdateRole_Response* pstResp = pstResponseMsg->mutable_m_stmsgbody()->mutable_m_stworld_updaterole_response();
	pstResp->set_iresult(uiResultID);

    TRACE_THREAD(m_iThreadIdx, "Info of failed UpdateRoleResponse: result: %d\n", uiResultID);

	return;
}

void CRoleDBUpdateRoleHandler::FillSuccessfulResponse(GameProtocolMsg* pstResponseMsg)
{
	World_UpdateRole_Response* pstResp = pstResponseMsg->mutable_m_stmsgbody()->mutable_m_stworld_updaterole_response();
	pstResp->set_iresult(T_SERVER_SUCESS);

    TRACE_THREAD(m_iThreadIdx, "Info of UpdateRoleResponse: result: %d, uin: %u\n",
             T_SERVER_SUCESS, pstResp->stroleid().uin());

	return;
}
