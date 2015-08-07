#include <stdlib.h>
#include <string.h>

#include "RoleDBApp.hpp"
#include "RoleDBLogManager.hpp"
#include "StringUtility.hpp"
#include "RoleDBListRoleHandler.hpp"

#include "FetchFriendInfoHandler.hpp"

using namespace ServerLib;

//todo jasonxiong2 ����ȵ�ʵ�ʿ�������ʱ��������

CFetchFriendInfoHandler::CFetchFriendInfoHandler(DBClientWrapper* pDatabase)
{
    m_pDatabase = pDatabase;
}

void CFetchFriendInfoHandler::OnClientMsg(GameProtocolMsg* pstRequestMsg,
                                          SHandleResult* pstHandleResult)
{
    if (!pstRequestMsg || !pstHandleResult)
    {
        return;
    }

    // fetch friend info������Ϣ
    m_pstRequestMsg = pstRequestMsg;

    const Zone_GetFriendInfo_Request& stReq = m_pstRequestMsg->m_stmsgbody().m_stzone_getfriendinfo_request();;
    unsigned int uiUin = stReq.uin();
    unsigned int uiFriendUin = stReq.friend_uin();

    DEBUG_THREAD(m_iThreadIdx, "Handling FetchFriendInfoRequest, operator uin: %u, "
        "target uin: %u\n", uiUin, uiFriendUin);

    // fetch friend info��Ӧ��Ϣ
    GameProtocolMsg* pstRespMsg = &pstHandleResult->stResponseMsg;

    // ��Ӧ��Ϣͷ
    GenerateResponseMsgHead(pstRespMsg, 0, MSGID_ZONE_FRIENDINFO_RESPONSE, uiUin);

    // ��Ӧ��Ϣ��
    Zone_GetFriendInfo_Response* pstResp = pstRespMsg->mutable_m_stmsgbody()->mutable_m_stzone_getfriendinfo_response();

    // ����role id��ѯ�ý�ɫ����Ϣ
    int iRes = QueryFriendInfo(uiUin, pstResp);
    if (iRes != 0)
    {
        FillFailedResponse(iRes, pstRespMsg);
        return;
    }

    // ���ɹ��ظ�
    FillSuccessfulResponse(pstRespMsg);
}

int CFetchFriendInfoHandler::QueryFriendInfo(unsigned int uin, Zone_GetFriendInfo_Response* pstResponse)
{
    if(!pstResponse)
    {
        return -3;
    }

    //todo jasonxiong ����ʵ����ȡ������Ϣ����ʱ����Ҫ����ʵ���������

    /*

    char* pszSelectSqlString = "select base_info from t_magic_userdata where uin = %u";


    DBROLEINFO stObj;
    char szObjMetaName[] = "DBRoleInfo";
    int iObjSize = sizeof(DBROLEINFO);
    char szWhere[1024] = "";

    memset(&stObj, 0, iObjSize);
    stObj.fUin = pstRoleID->m_uiUin; // ����
    stObj.fSeq = pstRoleID->m_uiSeq; // ����

    int iRet = m_pDatabase->Execute(&stObj, iObjSize, szObjMetaName, TDR_DBOP_SELECT, szWhere);
    if (iRet != 0)
    {
        TRACE_THREAD(m_iThreadIdx, "Failed to execute db query\n");
        return EQEC_RoleDB_SqlExecuteFailed;
    }

    long lNumberRows = m_pDatabase->GetNumberRows();
    if (1 != lNumberRows)
    {
        TRACE_THREAD(m_iThreadIdx, "The number of rows is %ld\n", lNumberRows);
        return EQEC_RoleDB_RoleNotFound;
    }

    memset(&stObj, 0, iObjSize);
    iRet = m_pDatabase->FetchRows(&stObj, iObjSize, lNumberRows, szObjMetaName);
    if (iRet != 0)
    {
        TRACE_THREAD(m_iThreadIdx, "Failed to fetch select result\n");
        return EQEC_RoleDB_SqlExecuteFailed;
    }

    // role info
    pstResponse->m_stRoleInfo.m_iActiveTitleConfigID = 
        stObj.fGloryInfo.m_stTitleList.m_stOwnedTitleList.m_iActiveTitleConfigID;

*/
    return 0;
}

void CFetchFriendInfoHandler::FillFailedResponse(const unsigned int uiResultID,
                                                 GameProtocolMsg* pstResponseMsg)
{
    Zone_GetFriendInfo_Response* pstResp = pstResponseMsg->mutable_m_stmsgbody()->mutable_m_stzone_getfriendinfo_response();

    pstResp->set_iresult(uiResultID);

    DEBUG_THREAD(m_iThreadIdx, "Info of failed FetchFriendInfoResponse: result: %d\n",
        pstResp->iresult());
}

void CFetchFriendInfoHandler::FillSuccessfulResponse(GameProtocolMsg* pstResponseMsg)
{
    Zone_GetFriendInfo_Response* pstResp = pstResponseMsg->mutable_m_stmsgbody()->mutable_m_stzone_getfriendinfo_response();

    pstResp->set_iresult(T_SERVER_SUCESS);

    return;
}
