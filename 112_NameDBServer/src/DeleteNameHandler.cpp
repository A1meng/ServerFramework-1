#include <string.h>

#include "AppDef.hpp"
#include "NameDBLogManager.hpp"
#include "StringUtility.hpp"
#include "NameDBApp.hpp"

#include "DeleteNameHandler.hpp"

using namespace ServerLib;

//ִ�е�SQL���
char CDeleteNameHandler::m_szQueryString[GameConfig::NAME_TABLE_SPLIT_FACTOR][256];

CDeleteNameHandler::CDeleteNameHandler(DBClientWrapper* pDatabase)
{
    m_pDatabase = pDatabase;
}

void CDeleteNameHandler::OnClientMsg(GameProtocolMsg* pstRequestMsg,
        SHandleResult* pstHandleResult)
{
    if (!pstRequestMsg || !pstHandleResult)
	{
		return;
	}

    //Delete������Ҫ�ذ�
    pstHandleResult->iNeedResponse = false;

    //Delete Name ��������Ϣ
    m_pstRequestMsg = pstRequestMsg;
    const DeleteName_Request& rstReq = m_pstRequestMsg->m_stmsgbody().m_stdeletename_request();

    TRACE_THREAD(m_iThreadIdx, "Handling DeleteNameRequest, name: %s, type %d\n", rstReq.strname().c_str(), rstReq.itype());

    // ��Ӧ��Ϣͷ
    GenerateResponseMsgHead(&pstHandleResult->stResponseMsg, 
                            m_pstRequestMsg->m_stmsghead().m_uisessionfd(),
                            MSGID_DELETENAME_RESPONSE, 
                            0);

    //����NameID��NameTypeɾ����¼
    int iRet = DeleteName(rstReq.strname(), rstReq.itype());
    if (iRet)
    {
        FillFailedResponse(iRet, &pstHandleResult->stResponseMsg);
        return;
    }

    FillSuccessfulResponse(&pstHandleResult->stResponseMsg);

    return;
}

//ɾ��Name
int CDeleteNameHandler::DeleteName(const std::string& strName, int iType)
{
    //��ȡNAMEDB���ݿ��������
    const ONENAMEDBINFO* pstDBConfig = (CNameDBApp::m_stNameDBConfigManager).GetOneNameDBInfoByIndex(m_iThreadIdx);
    if(!pstDBConfig)
    {
        TRACE_THREAD(m_iThreadIdx, "Failed to get name db config, thread index %d\n", m_iThreadIdx);
        return -1;
    }

    //����Ҫ���������ݿ������Ϣ
    m_pDatabase->SetMysqlDBInfo(pstDBConfig->szDBHost, pstDBConfig->szUserName, pstDBConfig->szUserPasswd, pstDBConfig->szDBName);

    char* pszQueryString = m_szQueryString[m_iThreadIdx];

    //����ɾ����SQL���
    int iLength = SAFE_SPRINTF(pszQueryString, sizeof(m_szQueryString[m_iThreadIdx])-1, "delete from %s where nickname= '%s' and nametype=%d", 
                 MYSQL_NAMEINFO_TABLE, strName.c_str(), iType);

    int iRet = m_pDatabase->ExecuteRealQuery(pszQueryString, iLength, false);
    if(iRet)
    {
        TRACE_THREAD(m_iThreadIdx, "Failed to execute delete sql query, name %s, type %d, ret %d\n", strName.c_str(), iType, iRet);
        return iRet;
    }

    DEBUG_THREAD(m_iThreadIdx, "The number of affected rows is %d\n", m_pDatabase->GetAffectedRows());

    return 0;
}

void CDeleteNameHandler::FillFailedResponse(const unsigned int uiResultID,
        GameProtocolMsg* pstResponseMsg)
{
	if (!pstResponseMsg)
	{
		return;
	}
    
    DeleteName_Response* pstResp = pstResponseMsg->mutable_m_stmsgbody()->mutable_m_stdeletename_response();
    pstResp->set_iresult(uiResultID);

    TRACE_THREAD(m_iThreadIdx, "Info of failed DeleteNameResponse: result: %d\n", uiResultID);
}

void CDeleteNameHandler::FillSuccessfulResponse(GameProtocolMsg* pstResponseMsg)
{
	if (!pstResponseMsg)
	{
		return;
	}

    DeleteName_Response* pstResp = pstResponseMsg->mutable_m_stmsgbody()->mutable_m_stdeletename_response();
    pstResp->set_iresult(T_SERVER_SUCESS);

    TRACE_THREAD(m_iThreadIdx, "Info of DeleteNameResponse: result: %d\n", T_SERVER_SUCESS);

    return;
}
