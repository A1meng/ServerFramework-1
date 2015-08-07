#include <string.h>

#include "RoleDBLogManager.hpp"
#include "NowTime.hpp"
#include "UnixTime.hpp"
#include "SeqConverter.hpp"
#include "StringUtility.hpp"
#include "RoleDBApp.hpp"
#include "AppDef.hpp"
#include "RoleDBApp.hpp"

#include "RoleDBCreateRoleHandler.hpp"

using namespace ServerLib;

char CRoleDBCreateRoleHandler::m_szQueryString[GameConfig::ROLE_TABLE_SPLIT_FACTOR][10240];

CRoleDBCreateRoleHandler::CRoleDBCreateRoleHandler(DBClientWrapper* pDatabase)
{
    m_pDatabase = pDatabase;
}

void CRoleDBCreateRoleHandler::OnClientMsg(GameProtocolMsg* pstRequestMsg,
        SHandleResult* pstHandleResult)
{
    if(!pstRequestMsg || !pstHandleResult)
    {
        return;
    }

    // create role������Ϣ
    m_pstRequestMsg = pstRequestMsg;
    const World_CreateRole_Request& rstReq = m_pstRequestMsg->m_stmsgbody().m_stworld_createrole_request();

    // create role��Ӧ��Ϣ
    GameProtocolMsg* pstMsgResp = &(pstHandleResult->stResponseMsg);
    World_CreateRole_Response* pstResp = pstMsgResp->mutable_m_stmsgbody()->mutable_m_stworld_createrole_response();

    // ��Ӧ��Ϣͷ
    GenerateResponseMsgHead(pstMsgResp, 
                            m_pstRequestMsg->m_stmsghead().m_uisessionfd(), 
                            MSGID_WORLD_CREATEROLE_RESPONSE, 
                            m_pstRequestMsg->m_stmsghead().m_uin());

    short shCacheSeq = 1;

    //��ȡ��ɫ������ʱ��
    unsigned short usCreateTime = 0;
    GetRoleCreateTime(usCreateTime);

    //��CacheSeqת����DBSeq
    unsigned int uiDBSeq = GenerateDBSeq(usCreateTime, rstReq.world(), shCacheSeq);

    // ��Ӧ��Ϣ��
    pstResp->mutable_stroleid()->set_uin(rstReq.uin());
    pstResp->mutable_stroleid()->set_uiseq(uiDBSeq);

    // �����е��û�����д�����ݱ�DBRoleInfo
    int iRet = InsertNewRoleRecord(rstReq, uiDBSeq);
    unsigned int uiResultID = T_SERVER_SUCESS;
    if (iRet != 0)
    {
        uiResultID = T_ROLEDB_SQL_EXECUTE_FAILED;

        FillFailedResponse(uiResultID, pstMsgResp);
    }
    else
    {
    	FillSuccessfulResponse(0, pstMsgResp);
    }

    TRACE_THREAD(m_iThreadIdx, "Info of CreateRoleResponse: result: %d, uin: %u\n", uiResultID, pstResp->stroleid().uin());

    return;
}

int CRoleDBCreateRoleHandler::InsertNewRoleRecord(
    const World_CreateRole_Request& rstCreateRoleRequest, unsigned int uiSeq)
{
    //��XML�ļ��ж�ȡ����
    const ONEROLEDBINFO* pstDBInfoConfig = (CRoleDBApp::m_stRoleDBConfigManager).GetOneRoleDBInfoByIndex(m_iThreadIdx);
    if(!pstDBInfoConfig)
    {
        TRACE_THREAD(m_iThreadIdx, "Failed to insert new role record, invalid cfg, thread idx %d\n", m_iThreadIdx);
        return -1;
    }

    //����Ҫ���������ݿ������Ϣ
    m_pDatabase->SetMysqlDBInfo(pstDBInfoConfig->szDBHost, pstDBInfoConfig->szUserName, pstDBInfoConfig->szUserPasswd, pstDBInfoConfig->szDBName);

    //��ʼ��SQL���
    int iLength = 0;
    int iRet = GenerateQueryString(rstCreateRoleRequest, uiSeq, m_szQueryString[m_iThreadIdx], sizeof(m_szQueryString[m_iThreadIdx])-1, iLength);
    if(iRet)
    {
        TRACE_THREAD(m_iThreadIdx, "Fail to generate insert query sql string, ret %d\n", iRet);
        return iRet;
    }

    //ִ��SQL���
    iRet = m_pDatabase->ExecuteRealQuery(m_szQueryString[m_iThreadIdx], iLength, false);
    if(iRet)
    {
        TRACE_THREAD(m_iThreadIdx, "Fail to insert user data, uin %u, ret %d\n", rstCreateRoleRequest.uin(), iRet);
        
        return iRet;
    }

    DEBUG_THREAD(m_iThreadIdx, "The number of affected rows is %d\n", m_pDatabase->GetAffectedRows());
    return 0;
}

//��ȡ��ҽ�ɫ������ʱ��,�Ǵ�2014-01-01��ʼ�������
void CRoleDBCreateRoleHandler::GetRoleCreateTime(unsigned short& usCreateTime)
{
    CUnixTime stNow(time(NULL));
    stNow.GetDaysAfterYear(GAME_START_YEAR, usCreateTime);
}

//���ɲ����SQL Query���
int CRoleDBCreateRoleHandler::GenerateQueryString(const World_CreateRole_Request& rstCreateRoleRequest, unsigned int uiSeq, char* pszBuff, int iBuffLen, int& iLength)
{
    if(!pszBuff)
    {
        TRACE_THREAD(m_iThreadIdx, "Fail to generate query string, invlaid buff pointer!\n");
        return T_ROLEDB_SYSTEM_PARA_ERR;
    }

    unsigned int uiUin = rstCreateRoleRequest.uin();

    SAFE_SPRINTF(pszBuff, iBuffLen-1, "insert into %s(uin,seq,base_info,quest_info,item_info,fight_info,friend_info,reserved1,reserved2) values(%u,%u,", MYSQL_USERINFO_TABLE, uiUin, uiSeq);
    char* pEnd = pszBuff + strlen(pszBuff);

    MYSQL& stDBConn = m_pDatabase->GetCurMysqlConn();

    const GameUserInfo& rstUserInfo = rstCreateRoleRequest.stbirthdata();

    //1.��һ�����Ϣ base_info
    *pEnd++ = '\'';
    pEnd += mysql_real_escape_string(&stDBConn, pEnd, rstUserInfo.strbaseinfo().c_str(), rstUserInfo.strbaseinfo().size());
    *pEnd++ = '\'';
    *pEnd++ = ',';

    //2.��ҵ�������Ϣ quest_info
    *pEnd++ = '\'';
    pEnd += mysql_real_escape_string(&stDBConn, pEnd, rstUserInfo.strquestinfo().c_str(), rstUserInfo.strquestinfo().size());
    *pEnd++ = '\'';
    *pEnd++ = ',';

    //3.��ҵ���Ʒ��Ϣ item_info
    *pEnd++ = '\'';
    pEnd += mysql_real_escape_string(&stDBConn, pEnd, rstUserInfo.striteminfo().c_str(), rstUserInfo.striteminfo().size());
    *pEnd++ = '\'';
    *pEnd++ = ',';

    //4.��ҵ�ս����Ϣ
    *pEnd++ = '\'';
    pEnd += mysql_real_escape_string(&stDBConn, pEnd, rstUserInfo.strfightinfo().c_str(), rstUserInfo.strfightinfo().size());
    *pEnd++ = '\'';
    *pEnd++ = ',';

    //5.��ҵĺ�����Ϣ
    *pEnd++ = '\'';
    pEnd += mysql_real_escape_string(&stDBConn, pEnd, rstUserInfo.strfriendinfo().c_str(), rstUserInfo.strfriendinfo().size());
    *pEnd++ = '\'';
    *pEnd++ = ',';

    //6.��ҵı����ֶ�1
    *pEnd++ = '\'';
    pEnd += mysql_real_escape_string(&stDBConn, pEnd, rstUserInfo.strreserved1().c_str(), rstUserInfo.strreserved1().size());
    *pEnd++ = '\'';
    *pEnd++ = ',';

    //7.��ҵı����ֶ�2
    *pEnd++ = '\'';
    pEnd += mysql_real_escape_string(&stDBConn, pEnd, rstUserInfo.strreserved2().c_str(), rstUserInfo.strreserved2().size());
    *pEnd++ = '\'';
    *pEnd++ = ')';

    iLength = pEnd - pszBuff;

    return T_SERVER_SUCESS;
}

void CRoleDBCreateRoleHandler::FillFailedResponse(const unsigned int uiResultID, GameProtocolMsg* pstResponseMsg)
{
    if(!pstResponseMsg)
    {
        return;
    }

    World_CreateRole_Response* pstResp = pstResponseMsg->mutable_m_stmsgbody()->mutable_m_stworld_createrole_response();
    pstResp->set_iresult(uiResultID);

    return;
}

void CRoleDBCreateRoleHandler::FillSuccessfulResponse(unsigned int uiSeq,
        GameProtocolMsg* pstResponseMsg)
{
    if(!pstResponseMsg)
    {
        return;
    }
    
    World_CreateRole_Response* pstResp = pstResponseMsg->mutable_m_stmsgbody()->mutable_m_stworld_createrole_response();

    pstResp->set_iresult(T_SERVER_SUCESS);

    return;
}
