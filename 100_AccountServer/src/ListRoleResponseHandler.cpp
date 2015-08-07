#include <assert.h>

#include "GameProtocol.hpp"
#include "LogAdapter.hpp"
#include "FixedHashCache.hpp"
#include "LRUHashCache.hpp"
#include "RoleNumberObj.hpp"
#include "SessionObj.hpp"
#include "AccountSingleton.hpp"
#include "ProtoDataUtility.hpp"
#include "TimeStampConverter.hpp"

#include "ListRoleResponseHandler.hpp"

using namespace ServerLib;

CListRoleResponseHandler::CListRoleResponseHandler()
{
}

void CListRoleResponseHandler::OnClientMsg(TNetHead_V2* pstNetHead,
        GameProtocolMsg* pstMsg, SHandleResult* pstResult)
{
    // ��ʹ��NetHead��Result
    ASSERT_AND_LOG_RTN_VOID(pstMsg);
    m_pstRequestMsg = pstMsg;
    m_uiSessionFD = TimeStampToSessionFD(m_pstRequestMsg->m_stmsghead().m_uisessionfd());
    m_unValue = TimeStampToValue(m_pstRequestMsg->m_stmsghead().m_uisessionfd());

    Account_ListRole_Response* pstListRoleResponse =
        m_pstRequestMsg->mutable_m_stmsgbody()->mutable_m_staccountlistroleresponse();
    unsigned int uiUin = pstListRoleResponse->uin();
    short nWorldID = pstListRoleResponse->world();

    LOGDETAIL("Handling ListRoleResponse from world server, result: %d, uin: %u, world: %d, role number: %u, sockfd: %u, value: %d\n",
             pstListRoleResponse->iresult(), uiUin, nWorldID, pstListRoleResponse->roles_size(), m_uiSessionFD, m_unValue);

    CSessionObj* pSession = SessionManager->GetSession(m_uiSessionFD, m_unValue);
    if (pSession != NULL) // ԭʼsession��Ȼ����
    {
        // ��������
        CRoleNumberObj* pRoleNumberObj = CLRUHashCache<CRoleNumberObj>::GetByUin(uiUin);
        if (!pRoleNumberObj)
        {
            // ���û���δ�ڽ�ɫ�����������д���������cache������
            // ����������Ҫ�������ݿ⣬�����ݼ��ص�cache�У�������û������е�world�ϵĽ�ɫ������Ϊ0��
            // ����cache��none role��־Ϊtrue��

            // ���������㲢��ʼ��
            pRoleNumberObj = CLRUHashCache<CRoleNumberObj>::CreateByUin(uiUin);
            if (pRoleNumberObj)
            {
                pRoleNumberObj->Initialize();
                pRoleNumberObj->SetUin(uiUin);

                // ��ʼ����������
                int i = 0;
                for (i = 0; i < pstListRoleResponse->roles_size(); i++)
                {
                    pRoleNumberObj->AddOneRoleToWorld(pstListRoleResponse->roles(i).stroleid());
                }
            }
            else
            {
                LOGERROR("Failed to create RoleNumberObj in cache, uin: %u\n", uiUin);
            }
        }

        // ������Ӧ��Ϣ����NetHead��ת����LotusServer

        // �Ƿ��ֹ������ɫ
        if (!ConfigMgr->IsCreateRoleEnabled())
        {
            if (pstListRoleResponse->roles_size() == 0)
            {
                pstListRoleResponse->set_iresult(T_ACCOUNT_CANNOT_CREATE_ROLE);
            }
        }

        //���浵ժҪ��Ϣ����
        //todo jasonxiong2 �ⲿ����ʱȥ��,Ӧ�ò���Ҫ���ɫ
        /*
        for(int i=0; i<pstListRoleResponse->roles_size(); ++i)
        {
            RoleSummary* pstOneRole = pstListRoleResponse->mutable_roles(i);

            for(int j=0; j<pstOneRole->strdigests_size(); ++j)
            {
                if(!DecodeProtoData(pstOneRole->strdigests(j), *pstOneRole->add_stdidests()))
                {
                    LOGERROR("Failed to decode base role info, uin %u\n", pstListRoleResponse->uin());
                    continue;
                }
            }

            //���δ��������
            pstOneRole->clear_strdigests();
        }
        */

        SendListRoleResponseToLotus();

        // ���cache�е�session���
        //SessionManager->DeleteSession(m_uiSessionFD);
    }
}

void CListRoleResponseHandler::SendListRoleResponseToLotus()
{
    if (EncodeAndSendCode(CSProtocolEngine,
                          SessionManager->GetSession(m_uiSessionFD, m_unValue)->GetNetHead(),
                          m_pstRequestMsg, GAME_SERVER_LOTUSACCOUNT) != 0)
    {
        LOGERROR("Failed to send ListRoleResponse to lotus server\n");
        return;
    }

    LOGDEBUG("Send ListRoleResponse to lotus server\n");
}
