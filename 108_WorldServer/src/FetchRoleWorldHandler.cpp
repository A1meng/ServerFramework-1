#include <string.h>

#include "ProtoDataUtility.hpp"
#include "GameProtocol.hpp"
#include "LogAdapter.hpp"
#include "FetchRoleWorldHandler.hpp"
#include "ModuleHelper.hpp"
#include "WorldMsgHelper.hpp"

using namespace ServerLib;

CFetchRoleWorldHandler::~CFetchRoleWorldHandler()
{
}

int CFetchRoleWorldHandler::OnClientMsg(const void* pMsg)
{
	ASSERT_AND_LOG_RTN_INT(pMsg);

	m_pMsg = (GameProtocolMsg*)pMsg;

	switch (m_pMsg->m_stmsghead().m_uimsgid())
	{
	case MSGID_WORLD_FETCHROLE_REQUEST:
		{
			OnRequestFetchRoleWorld();
			break;
		}
    case MSGID_WORLD_FETCHROLE_RESPONSE:
        {
            OnResponseFetchRoleWorld();
            break;
        }
	default:
		{
			break;
		}
	}

	return 0;
}

int CFetchRoleWorldHandler::OnRequestFetchRoleWorld()
{
    const World_FetchRole_Request& rstRequest = m_pMsg->m_stmsgbody().m_stworld_fetchrole_request();

    unsigned uiUin = rstRequest.stroleid().uin();
	if (uiUin == 0)
	{
		SendFailedFetchRoleResponse(rstRequest.stroleid(), rstRequest.ireqid());
		return -1;
	}
    
    LOGDEBUG("FetchRoleWorld: Uin = %d\n", uiUin);

    CWorldRoleStatusWObj* pWorldRoleStatusObj = WorldTypeK32<CWorldRoleStatusWObj>::GetByRoleID(rstRequest.stroleid());
    // �����ɫ�����Ѿ�����, �����ǵ�¼����, �򷵻�ʧ��
    if (pWorldRoleStatusObj && rstRequest.bislogin())
    {
        LOGERROR("Role Already Login: Uin = %d.\n", uiUin);
        SendFailedFetchRoleResponse(rstRequest.stroleid(), rstRequest.ireqid());
        return -2;
    }

    // ������ǵ�¼���󣬲���Ҫ��ѯ�Ľ�ɫ�����ڣ��򷵻�ʧ��
    if (!pWorldRoleStatusObj && !rstRequest.bislogin())
    {
        LOGERROR("Role Non-Exists: Uin = %u.\n", uiUin);
        SendFailedFetchRoleResponse(rstRequest.stroleid(), rstRequest.ireqid());
        return -3;
    }
    
    // ����ǵ�¼���߶�ȡ�浵����, ������ɫ���󲢲�ѯ��ɫ��Ϣ
    if (rstRequest.bislogin())
    {
        //��¼������Ҫ���´�������Obj
        pWorldRoleStatusObj = WorldTypeK32<CWorldRoleStatusWObj>::CreateByUin(uiUin);
        if (!pWorldRoleStatusObj)
        {
            LOGERROR("CreateRoleStatusObj Failed: Uin = %u.\n", uiUin);
            SendFailedFetchRoleResponse(rstRequest.stroleid(), rstRequest.ireqid());
            return -4;
        }

        pWorldRoleStatusObj->SetZoneID(rstRequest.ireqid());
        pWorldRoleStatusObj->SetRoleID(rstRequest.stroleid());

        //todo jasonxiong2 ��ʱ����ҪEnterType���Ⱥ�����Ҫʱ�����
        pWorldRoleStatusObj->SetEnterType(0);

        // �����ݿ�����ȡ����
        int iRet = CWorldMsgHelper::SendWorldMsgToRoleDB(*m_pMsg);
        if (iRet < 0)
        {
            LOGERROR("SendFetchRoleDB Failed: Uin = %d, iRet = %d\n", uiUin, iRet);

            SendFailedFetchRoleResponse(rstRequest.stroleid(), rstRequest.ireqid());
            WorldTypeK32<CWorldRoleStatusWObj>::DeleteByUin(uiUin);
            return -6;
        }

        return 0;
    }
    
    // ����ֱ�ӷ��ؽ�ɫ����
    SendFetchRoleResponse(rstRequest.stroleid(), rstRequest.ireqid());

	return 0;
}

//��ѯ���󷵻ص�World�Ĵ������
int CFetchRoleWorldHandler::OnResponseFetchRoleWorld()
{
    World_FetchRole_Response* pstResp = m_pMsg->mutable_m_stmsgbody()->mutable_m_stworld_fetchrole_response();
    unsigned uiUin = pstResp->stroleid().uin();

    //����World����ı����������
    CWorldRoleStatusWObj* pUserStatusObj = WorldTypeK32<CWorldRoleStatusWObj>::GetByRoleID(pstResp->stroleid());
    if(!pUserStatusObj)
    {
        LOGERROR("World No Cache Data: Uin = %u\n", uiUin);
        return -1;
    }

    int iZoneID = pUserStatusObj->GetZoneID();

    //����ǵ�¼����֤��ҵĲ�����״̬
    if(pstResp->bislogin() && (pUserStatusObj->GetRoleStatus() & EGUS_ONLINE))
    {
        LOGERROR("Fail to fetch role from DB, already online, uin %u\n", uiUin);
        
        WorldTypeK32<CWorldRoleStatusWObj>::DeleteByUin(uiUin);
        SendFailedFetchRoleResponse(pstResp->stroleid(), iZoneID);
        return -2;
    }

    //�����DB��������ʧ��
    if(pstResp->iresult())
    {
        LOGERROR("Fail to fetch role from DB, uin %u, ret 0x%0x\n", uiUin, pstResp->iresult());

        WorldTypeK32<CWorldRoleStatusWObj>::DeleteByUin(uiUin);
        SendFailedFetchRoleResponse(pstResp->stroleid(), iZoneID);
        return -3;
    }

    LOGDEBUG("Fetch ROLEDB OK, uin %u, from zone id %d\n", uiUin, iZoneID);

    //����World����һ���������Ϣ
    BASEDBINFO stBaseInfo;
    if(!DecodeProtoData(pstResp->stuserinfo().strbaseinfo(), stBaseInfo))
    {
        LOGERROR("Failed to decode base proto data, uin %u\n", uiUin);
        return -4;
    }

    unsigned int uStatus = stBaseInfo.ustatus();
    uStatus = uStatus|EGUS_ONLINE;

    stBaseInfo.set_ustatus(uStatus);
    if(!EncodeProtoData(stBaseInfo, *pstResp->mutable_stuserinfo()->mutable_strbaseinfo()))
    {
        LOGERROR("Failed to encode base proto data, uin %u\n", uiUin);
        return -5;
    }

    pUserStatusObj->SetRoleInfo(pstResp->stuserinfo());
    pUserStatusObj->SetRoleStatus(uStatus);

    //���ز�ѯ���
    int iRet = CWorldMsgHelper::SendWorldMsgToWGS(*m_pMsg, iZoneID);
    if(iRet)
    {
        LOGERROR("SendFetchRoleResponse to GS failed, uin %u, zone id %d\n", uiUin, iZoneID);

        WorldTypeK32<CWorldRoleStatusWObj>::DeleteByUin(uiUin);
        SendFailedFetchRoleResponse(pstResp->stroleid(), iZoneID); 
    }

    return iRet;
}

// ����ʧ����Ϣ
int CFetchRoleWorldHandler::SendFailedFetchRoleResponse(const RoleID& stRoleID, int iReqID)
{
    CWorldMsgHelper::GenerateMsgHead(m_stWorldMsg, 0, MSGID_WORLD_FETCHROLE_RESPONSE, stRoleID.uin());
    
    World_FetchRole_Response* rstResp = m_stWorldMsg.mutable_m_stmsgbody()->mutable_m_stworld_fetchrole_response();
    rstResp->mutable_stroleid()->CopyFrom(stRoleID);
    rstResp->set_iresult(T_WORLD_FETCHROLE_FAIED);
	
    int iRet = CWorldMsgHelper::SendWorldMsgToWGS(m_stWorldMsg, iReqID);

	return iRet;
}

// ���ؽ�ɫ����
int CFetchRoleWorldHandler::SendFetchRoleResponse(const RoleID& stRoleID, int iReqID)
{
    CWorldRoleStatusWObj* pWorldRoleStatusObj = WorldTypeK32<CWorldRoleStatusWObj>::GetByRoleID(stRoleID);
    if (!pWorldRoleStatusObj)
    {
        return -1;
    }

    CWorldMsgHelper::GenerateMsgHead(m_stWorldMsg, 0, MSGID_WORLD_FETCHROLE_RESPONSE, stRoleID.uin());

    World_FetchRole_Response* pstResp = m_stWorldMsg.mutable_m_stmsgbody()->mutable_m_stworld_fetchrole_response();
    pstResp->set_iresult(T_SERVER_SUCESS);
    pstResp->mutable_stroleid()->CopyFrom(stRoleID);

    pstResp->mutable_stuserinfo()->CopyFrom(pWorldRoleStatusObj->GetRoleInfo());

    int iRet = CWorldMsgHelper::SendWorldMsgToWGS(m_stWorldMsg, iReqID);

    return iRet;
}

