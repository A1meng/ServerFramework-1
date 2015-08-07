#include <string.h>

#include "RoleDBLogManager.hpp"
#include "SeqConverter.hpp"
#include "StringUtility.hpp"
#include "RoleDBApp.hpp"
#include "ModuleHelper.hpp"
#include "AppDef.hpp"

#include "RoleDBListRoleHandler.hpp"

//SQL����ַ���������
char CRoleDBListRoleHandler::m_szQueryString[GameConfig::ROLE_TABLE_SPLIT_FACTOR][256];

CRoleDBListRoleHandler::CRoleDBListRoleHandler(DBClientWrapper* pDatabase)
{    
    m_pDatabase = pDatabase;
}

void CRoleDBListRoleHandler::OnClientMsg(GameProtocolMsg* pstRequestMsg,
        SHandleResult* pstHandleResult)
{
    if (!pstRequestMsg || !pstHandleResult)
	{
		return;
	}

    // list role������Ϣ
    m_pstRequestMsg = pstRequestMsg;
	int iRet = -1;
	switch(m_pstRequestMsg->m_stmsghead().m_uimsgid())
	{
		case MSGID_ACCOUNT_LISTROLE_REQUEST:
			{
				iRet = AccountListRole(pstHandleResult);
				break;
			}
				
		default:
			{
				break;
			}			
	}

	DEBUG_THREAD(m_iThreadIdx, "MsgId:%d, iRet:%d\n" , m_pstRequestMsg->m_stmsghead().m_uimsgid(), iRet);
}

int CRoleDBListRoleHandler::AccountListRole(SHandleResult* pstHandleResult)
{
	Account_ListRole_Request* pstReq = m_pstRequestMsg->mutable_m_stmsgbody()->mutable_m_staccountlistrolerequest();
	unsigned int uiUin = pstReq->uin();
	unsigned short usWorldID = pstReq->world();

    short usNewWorldID = static_cast<short>(CModuleHelper::GetWorldID());

    // list role��Ӧ��Ϣ
    GameProtocolMsg* pstResponseMsg = &(pstHandleResult->stResponseMsg);

    // ��Ӧ��Ϣͷ
    GenerateResponseMsgHead(pstResponseMsg, 
							m_pstRequestMsg->m_stmsghead().m_uisessionfd(), 
							MSGID_ACCOUNT_LISTROLE_RESPONSE, 
							uiUin);

    // ��Ӧ��Ϣ��
	Account_ListRole_Response* pstResp = pstResponseMsg->mutable_m_stmsgbody()->mutable_m_staccountlistroleresponse();

    // ����uin��DBRoleInfo���м�����seq���������е�world��Ϣ������ָ��world�ϵ�role�б�
	int iErrnoNum = T_ROLEDB_SQL_EXECUTE_FAILED;
	int iRes = QueryAndParseRole(uiUin, usWorldID, usNewWorldID, pstResp, iErrnoNum);

	TRACE_THREAD(m_iThreadIdx, "Info of ListRoleResponse: result: %u, uin: %u, world: %u:%u, role number: %d\n",
		iRes, uiUin, usWorldID, usNewWorldID, pstResp->roles_size());

    if (iRes != 0)
    {
        FillFailedResponse(iErrnoNum, pstResponseMsg);
        return -1;
    }

    FillSuccessfulResponse(pstResponseMsg);
	return 0;
}

int CRoleDBListRoleHandler::QueryAndParseRole(const unsigned int uiUin, short nWorldID, short nNewWorldID,
        Account_ListRole_Response* pstListRoleResponse, int& iErrnoNum)
{
    if (!pstListRoleResponse)
	{
		return -1;
	}

	//��ѯDB���Ƿ��Ѿ���������ʺ�

	//��ȡROLEDB��ص�����
	const ONEROLEDBINFO* pstDBConfig = (CRoleDBApp::m_stRoleDBConfigManager).GetOneRoleDBInfoByIndex(m_iThreadIdx);
	if(!pstDBConfig)
	{
		TRACE_THREAD(m_iThreadIdx, "Failed to get roledb config, invalid index %d\n", m_iThreadIdx);
		return -2;
	}

	//����Ҫ���ʵ����ݿ���Ϣ
	m_pDatabase->SetMysqlDBInfo(pstDBConfig->szDBHost, pstDBConfig->szUserName, pstDBConfig->szUserPasswd, pstDBConfig->szDBName);

	//����SQL���
	char* pszQueryString = m_szQueryString[m_iThreadIdx];
	static int iQueryStringLen = sizeof(m_szQueryString[m_iThreadIdx])-1;
	int iLength = 0;

	//1��������������2������Ԥ������,�����������ܻ��������������
	if(nNewWorldID==1 || nNewWorldID==2)
	{
		//��������Ԥ��������������������ݽ���
		iLength = SAFE_SPRINTF(pszQueryString, iQueryStringLen, 
							   "select uin,seq from %s where uin=%u\n", 
							   MYSQL_USERINFO_TABLE, uiUin);
	}
	else
	{
		//������ʽ�������ǺϷ������
		iLength = SAFE_SPRINTF(pszQueryString, iQueryStringLen, 
							   "select uin,seq from %s where uin = %u and floor(seq%%10000/10)=%d\n", 
							   MYSQL_USERINFO_TABLE, uiUin, nWorldID);
	}

	//ִ��
	int iRet = m_pDatabase->ExecuteRealQuery(pszQueryString, iLength, true);
	if(iRet)
	{
		TRACE_THREAD(m_iThreadIdx, "Failed to execute select sql query, uin %u\n", uiUin);
		return iRet;
	}

	//�������ؽ��
	int iRowNum = m_pDatabase->GetNumberRows();
	if(iRowNum == 0)
	{
		//��������Ҽ�¼��ֱ�ӷ��سɹ�
		return T_SERVER_SUCESS;
	}
	
	TRACE_THREAD(m_iThreadIdx, "List Role Num %d, uin %u, world id %d\n", iRowNum, uiUin, nWorldID);

	//����������ݼ�¼����װ���ݲ�����
	MYSQL_ROW pstResult = NULL;
	unsigned long* pLengths = NULL;
	unsigned int uFields = 0;
	for(int i=0; i<iRowNum; ++i)
	{
		iRet = m_pDatabase->FetchOneRow(pstResult, pLengths, uFields);
		if(iRet)
		{
			TRACE_THREAD(m_iThreadIdx, "Failed to fetch rows, uin %u, ret %d\n", uiUin, iRet);
			return iRet;
		}

		//�ж������Ƿ����
		if(uFields < 2)
		{
			TRACE_THREAD(m_iThreadIdx, "Wrong result, real fields %u, needed %u\n", uFields, 3);
			return -1;
		}

		//�ӽ���н�����Ҫ���ֶ�
		RoleSummary* pstRoleSummary = pstListRoleResponse->add_roles();
		pstRoleSummary->mutable_stroleid()->set_uin(uiUin);

		//�ֶ�0��uin

		//�ֶ�1��seq
		unsigned int uRoleSeq = (unsigned)atoi(pstResult[1]);
		pstRoleSummary->mutable_stroleid()->set_uiseq(uRoleSeq);
	}

    return T_SERVER_SUCESS;
}

void CRoleDBListRoleHandler::FillFailedResponse(const unsigned int uiResultID,
        GameProtocolMsg* pstResponseMsg)
{
    Account_ListRole_Response* pstListRoleResponse = pstResponseMsg->mutable_m_stmsgbody()->mutable_m_staccountlistroleresponse();
	pstListRoleResponse->set_iresult(uiResultID);
}

void CRoleDBListRoleHandler::FillSuccessfulResponse(GameProtocolMsg* pstResponseMsg)
{
	Account_ListRole_Response* pstListRoleResponse = pstResponseMsg->mutable_m_stmsgbody()->mutable_m_staccountlistroleresponse();

	pstListRoleResponse->set_iresult(T_SERVER_SUCESS);
	
	return;
}

