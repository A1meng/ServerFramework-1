
#include "GameProtocol.hpp"
#include "LogAdapter.hpp"
#include "ZoneErrorNumDef.hpp"
#include "GameRole.hpp"
#include "BattlefieldObj.hpp"
#include "ZoneMsgHelper.hpp"
#include "ZoneObjectHelper.hpp"
#include "CombatEventManager.hpp"
#include "CombatUtility.hpp"
#include "CombatUnitAI.hpp"
#include "CombatUnitObj.hpp"
#include "UnitUtility.hpp"

#include "CombatFramework.hpp"

GameProtocolMsg CCombatFramework::m_stGameMsg;

using namespace ServerLib;

CCombatFramework* CCombatFramework::Instance()
{
    static CCombatFramework* pInstance = NULL;
    if(!pInstance)
    {
        pInstance = new CCombatFramework();
    }

    return pInstance;
}

CCombatFramework::CCombatFramework()
{

}

CCombatFramework::~CCombatFramework()
{

}

//��ҿ�ʼս��������
int CCombatFramework::DoCombat(CGameRoleObj& stRoleObj, const Zone_DoCombat_Request& rstRequest, bool bNeedResp)
{
    //��ʼ����Ա����
    m_uiUin = stRoleObj.GetUin();
    m_stGameMsg.Clear();

    //�жϵ�ǰ�Ƿ���ս����
    if(stRoleObj.GetBattlefieObjID() >= 0)
    {
        //�Ѿ���ս���У�����
        return T_ZONE_SYSTEM_PARA_ERR;
    }

    CZoneMsgHelper::GenerateMsgHead(m_stGameMsg, MSGID_ZONE_DOCOMBAT_RESPONSE);

    //��ȡ��Ӧ����Ϣ��
    Zone_DoCombat_Response* pstResp = m_stGameMsg.mutable_m_stmsgbody()->mutable_m_stzone_docombat_response();
    
    int iRet = T_SERVER_SUCESS;
    switch(rstRequest.etype())
    {
        case COMBAT_TYPE_PVE:
            {
                //PVEս��
                iRet = InitPveBattlefield(stRoleObj, rstRequest.ilevelid());
                if(iRet)
                {
                    LOGERROR("Failed to init pve battle field, ret %d, uin %u\n", iRet, m_uiUin);
                    return iRet;
                }
            }
            break;

        case COMBAT_TYPE_PVP:
            {
                ;
            }
            break;

        default:
            {
                return -1;
            }
            break;
    }

     //��䲢�ذ����ͻ���
    pstResp->set_iresult(T_SERVER_SUCESS);

    if(bNeedResp)
    {
        SendSuccessResponse();
    }

    //����ս����ʼ����Ϣ
    SendBattlefieldInitNotify(stRoleObj);

    //�׳���ʼս���¼�
    CCombatEventManager::NotifyCombatBegin(stRoleObj);

    return T_SERVER_SUCESS;
}

//�������ս����λ�ƶ�������
int CCombatFramework::DoCombatMove(CGameRoleObj& stRoleObj, const Zone_CombatMove_Request& rstRequest)
{
    //��ʼ��
    m_uiUin = stRoleObj.GetUin();
    m_stGameMsg.Clear();

    CZoneMsgHelper::GenerateMsgHead(m_stGameMsg, MSGID_ZONE_COMBATMOVE_RESPONSE);

    //��ȡ��Ӧ����Ϣ��
    Zone_CombatMove_Response* pstResp = m_stGameMsg.mutable_m_stmsgbody()->mutable_m_stzone_combatmove_response();

    //��ȡս����Ϣ
    CBattlefieldObj* pstBattlefieldObj = CCombatUtility::GetBattlefiledObj(stRoleObj);
    if(!pstBattlefieldObj)
    {
        LOGERROR("Failed to do combat role unit move, no battlefield, uin %u\n", m_uiUin);

        CCombatFramework::FinCombat(stRoleObj);
        return T_ZONE_SYSTEM_PARA_ERR;
    }

    //�ж��Ƿ��������ƶ������ǵ�ǰ״̬�Ƿ���ȷ
    if((pstBattlefieldObj->GetActionUnitType()!=COMBAT_UNIT_TYPE_ROLE
       && pstBattlefieldObj->GetActionUnitType()!=COMBAT_UNIT_TYPE_PARTNER)
       ||pstBattlefieldObj->GetActionUnitStatus()!=ROLE_COMBAT_UNIT_STAT_MOVE)
    {
        LOGERROR("Failed to do combat role unit move, not role unit or invalid stat, uin %u\n", m_uiUin);

        CCombatFramework::FinCombat(stRoleObj);

        return T_ZONE_SYSTEM_PARA_ERR;
    }

    int iRet = pstBattlefieldObj->DoCombatUnitMove(rstRequest.stpath());
    if(iRet)
    {
        LOGERROR("Failed to do combat role unit move, ret %d, uin %u\n", iRet, m_uiUin);

        //CCombatFramework::FinCombat(stRoleObj);
        return iRet;
    }

    //��䲢�ذ����ͻ���
    pstResp->set_iresult(T_SERVER_SUCESS);

    SendSuccessResponse();

    //�׳�ս����λ��ʼ�ж��¼�
    CCombatEventManager::NotifyCombatUnitAction(stRoleObj);

    return T_SERVER_SUCESS;
}
    
//�������ս����λ�ж�������
int CCombatFramework::DoCombatAction(CGameRoleObj& stRoleObj, const Zone_CombatAction_Request& rstRequest)    
{
    //��ʼ��
    m_uiUin = stRoleObj.GetUin();
    m_stGameMsg.Clear();

    CZoneMsgHelper::GenerateMsgHead(m_stGameMsg, MSGID_ZONE_COMBATACTION_RESPONSE);

    //��ȡ��Ӧ����Ϣ��
    Zone_CombatAction_Response* pstResp = m_stGameMsg.mutable_m_stmsgbody()->mutable_m_stzone_combataction_response();

    //��ȡս����Ϣ
    CBattlefieldObj* pstBattlefieldObj = CCombatUtility::GetBattlefiledObj(stRoleObj);
    if(!pstBattlefieldObj)
    {
        LOGERROR("Failed to do combat role unit action, no battlefield, uin %u\n", m_uiUin);

        CCombatFramework::FinCombat(stRoleObj);
        return T_ZONE_SYSTEM_PARA_ERR;
    }

    //�ж��Ƿ������ж��Լ��ж�״̬
    if((pstBattlefieldObj->GetActionUnitType()!=COMBAT_UNIT_TYPE_ROLE
        && pstBattlefieldObj->GetActionUnitType()!=COMBAT_UNIT_TYPE_PARTNER)
       ||pstBattlefieldObj->GetActionUnitStatus()!=ROLE_COMBAT_UNIT_STAT_ACTION)
    {
        LOGERROR("Failed to do combat role unit action, not role unit or invalid stat, uin %u\n", m_uiUin);

        CCombatFramework::FinCombat(stRoleObj);
        return T_ZONE_SYSTEM_PARA_ERR;
    }

    //�жϲ������Ƿ���ȷ
    if(!pstBattlefieldObj->IsValidActionRole(m_uiUin))
    {
        LOGERROR("Failed to do combat role unit action, unit id %d, uin %u\n", pstBattlefieldObj->GetActionUnitID(), m_uiUin);

        CCombatFramework::FinCombat(stRoleObj);
        return T_ZONE_SYSTEM_PARA_ERR;
    }

    int iRet = T_SERVER_SUCESS;
    TUNITPOSITION stTargetPos(rstRequest.stpos().iposx(), rstRequest.stpos().iposy());
    switch(rstRequest.etype())
    {
        case COMBAT_ACTION_CASTSKILL:
            {
                //��Ŀ���ͷż���
                CCombatTrapObj* pstTrapObj = pstBattlefieldObj->GetTrapByPos(stTargetPos);
                if(pstTrapObj && pstBattlefieldObj->IsTrapVisibleToActionUnit(*pstTrapObj))
                {    
                    //�ǻ��ز��һ��ؿɼ�,��������
                    iRet = pstBattlefieldObj->AttackCombatTrap(stTargetPos);
                }
                else
                {    
                    //����ͨ��������
                    iRet = pstBattlefieldObj->DoCombatCastSkill(rstRequest.icastskillid(), stTargetPos, SKILL_USE_ACTIVE);
                }
            }
            break;

        case COMBAT_ACTION_USEITEM:
            {
                //ʹ����Ʒ
                iRet = pstBattlefieldObj->DoCombatUseItem(rstRequest.iitemid(), stTargetPos);
            }
            break;

        case COMBAT_ACTION_IDLE:
            {
                //����Э������Ҫ���÷���
                iRet = pstBattlefieldObj->SetActionUnitDirection(rstRequest.idirection());
            }
            break;

        case COMBAT_ACTION_TRIGGER_TRAP:
            {
                //��������
                iRet = pstBattlefieldObj->ManualTriggerTrap(stTargetPos);
            }
            break;

        case COMBAT_ACTION_ATTACK_TRAP:
            {
                //�������� 
                iRet = pstBattlefieldObj->AttackCombatTrap(stTargetPos);
            }
            break;

        default:
            {
                LOGERROR("Failed to do role unit action, invalid type %d, uin %u\n", rstRequest.etype(), m_uiUin);

                iRet = T_ZONE_SYSTEM_PARA_ERR;
            }
            break;
    }

    if(iRet)
    {
        LOGERROR("Failed to do role unit action, type %d, ret %d, uin %u\n", rstRequest.etype(), iRet, m_uiUin);

        CCombatFramework::FinCombat(stRoleObj);
        return iRet;
    }

    //��䲢�ذ����ͻ���
    pstResp->set_etype(rstRequest.etype());
    pstResp->set_iresult(T_SERVER_SUCESS);

    SendSuccessResponse();

    //�׳�ս����λ�ж��غϽ����¼�
    CCombatEventManager::NotifyActionRoundEnd(stRoleObj);

    return T_SERVER_SUCESS;
}

//�������
int CCombatFramework::DoUpdatePartnerAI(CGameRoleObj& stRoleObj, const Zone_UpdatePartnerAI_Request& rstRequest)
{
    //��ʼ��
    m_uiUin = stRoleObj.GetUin();
    m_stGameMsg.Clear();

    CZoneMsgHelper::GenerateMsgHead(m_stGameMsg, MSGID_ZONE_UPDATEPARTNERAI_RESPONSE);

    //��ȡ��Ӧ����Ϣ��
    Zone_UpdatePartnerAI_Response* pstResp = m_stGameMsg.mutable_m_stmsgbody()->mutable_m_stzone_updatepartnerai_response();

    //��ȡս����Ϣ
    CBattlefieldObj* pstBattlefieldObj = CCombatUtility::GetBattlefiledObj(stRoleObj);
    if(!pstBattlefieldObj)
    {
        LOGERROR("Failed to update partner ai, no battlefield, uin %u\n", m_uiUin);

        CCombatFramework::FinCombat(stRoleObj);
        return T_ZONE_SYSTEM_PARA_ERR;
    }

    //���û���AI
    int iRet = pstBattlefieldObj->SetPartnerAI(rstRequest.ipartneraiid());
    if(iRet)
    {
        LOGERROR("Failed to update partner AI, ret %d, uin %u\n", iRet, m_uiUin);

        CCombatFramework::FinCombat(stRoleObj);
        return iRet;
    }

    //��䲢�ذ����ͻ���
    pstResp->set_iresult(T_SERVER_SUCESS);

    SendSuccessResponse();

    //�������ս����λ�����ǣ���Ԥ�ȼ���AI
    if(pstBattlefieldObj->GetActionUnitType() != COMBAT_UNIT_TYPE_ROLE)
    {
        iRet = CCombatUnitAI::DoActionUnitAI(stRoleObj.GetBattlefieObjID(), pstBattlefieldObj->GetActionUnitID());
        if(iRet)
        {
            LOGERROR("Failed to do action unit AI, ret %d, uin %u\n", iRet, m_uiUin);
    
            //����,����ս��
            CCombatFramework::FinCombat(stRoleObj);
            return iRet;
        }
    }

    //�׳�ս����λ�ƶ��¼�
    CCombatEventManager::NotifyCombatUnitMove(stRoleObj);

    return T_SERVER_SUCESS;
}

//����PVPս��������
int CCombatFramework::StartPVPCombat(CGameRoleObj& stRoleObj, const Zone_StartPVPCombat_Request& rstRequest)
{
    //��ʼ��
    m_uiUin = stRoleObj.GetUin();
    m_stGameMsg.Clear();

    CZoneMsgHelper::GenerateMsgHead(m_stGameMsg, MSGID_ZONE_STARTPVPCOMBAT_RESPONSE);

    //��ȡ��Ӧ����Ϣ��
    Zone_StartPVPCombat_Response* pstResp = m_stGameMsg.mutable_m_stmsgbody()->mutable_m_stzone_startpvpcombat_response();

    //˫���Ƿ����ս��
    if(!CCombatUtility::CanDoPVPCombat(m_uiUin) || !CCombatUtility::CanDoPVPCombat(rstRequest.uipassiveuin()))
    {
        LOGERROR("Failed to do pvp combat, active:passive %u:%u\n", m_uiUin, rstRequest.uipassiveuin());
        return T_ZONE_SYSTEM_PARA_ERR;
    }

    //��䲢�ذ����ͻ���
    pstResp->set_iresult(T_SERVER_SUCESS);

    SendSuccessResponse();

    //������Ϣ���Է�
    static GameProtocolMsg stMsg;
    CZoneMsgHelper::GenerateMsgHead(stMsg, MSGID_ZONE_STARTPVPCOMBAT_NBOTIFY);

    Zone_StartPVPCombat_Notify* pstNotify = stMsg.mutable_m_stmsgbody()->mutable_m_stzone_startpvpcombat_notify();
    pstNotify->set_uiactiveuin(m_uiUin);
    pstNotify->set_stractivename(stRoleObj.GetNickName());

    CZoneMsgHelper::SendZoneMsgToRole(stMsg, CUnitUtility::GetRoleByUin(rstRequest.uipassiveuin()));

    return T_SERVER_SUCESS;
}

//����PVPս��������
int CCombatFramework::AcceptPVPCombat(CGameRoleObj& stRoleObj, const Zone_AcceptPVPCombat_Request& rstRequest)
{
    //��ʼ��
    m_uiUin = stRoleObj.GetUin();
    m_stGameMsg.Clear();

    CZoneMsgHelper::GenerateMsgHead(m_stGameMsg, MSGID_ZONE_ACCEPTPVPCOMBAT_RESPONSE);

    //��ȡ��Ӧ����Ϣ��
    Zone_AcceptPVPCombat_Response* pstResp = m_stGameMsg.mutable_m_stmsgbody()->mutable_m_stzone_acceptpvpcombat_response();

    if(!rstRequest.bisaccept())
    {
        //�Ǿܾ�ս��
        static GameProtocolMsg stMsg;
        CZoneMsgHelper::GenerateMsgHead(stMsg, MSGID_ZONE_REJECTPVPCOMBAT_NOTIFY);

        Zone_RejectPVPCombat_Notify* pstRejectNotify = stMsg.mutable_m_stmsgbody()->mutable_m_stzone_rejectpvpcombat_notify();
        pstRejectNotify->set_ipassiveuin(m_uiUin);
        pstRejectNotify->set_strpassivename(stRoleObj.GetNickName());

        //���;ܾ���֪ͨ���Է�
        CZoneMsgHelper::SendZoneMsgToRole(stMsg, CUnitUtility::GetRoleByUin(rstRequest.uiactiveuin()));
    }
    else
    {    
        //�ǽ���ս��

        //˫���Ƿ����ս��
        if(!CCombatUtility::CanDoPVPCombat(m_uiUin) || !CCombatUtility::CanDoPVPCombat(rstRequest.uiactiveuin()))
        {
            LOGERROR("Failed to do pvp combat, active:passive %u:%u\n", rstRequest.uiactiveuin(), m_uiUin);
            return T_ZONE_SYSTEM_PARA_ERR;
        }
    
        //��ʼPVPս��
        CGameRoleObj* pstActiveRoleObj = CUnitUtility::GetRoleByUin(rstRequest.uiactiveuin());
        if(!pstActiveRoleObj)
        {
            return T_ZONE_SYSTEM_PARA_ERR;
        }
    
        int iRet = InitPVPBattlefield(*pstActiveRoleObj, stRoleObj);
        if(iRet)
        {
            LOGERROR("Failed to init pvp battlefield, ret %d, active %u, passive %u\n", iRet, rstRequest.uiactiveuin(), m_uiUin);
            return iRet;
        }
    }

    //��䲢�ذ����ͻ���
    pstResp->set_iresult(T_SERVER_SUCESS);

    SendSuccessResponse();

    if(rstRequest.bisaccept())
    {
        //����ǽ���ս��

        //����ս����ʼ����Ϣ
        SendBattlefieldInitNotify(stRoleObj);
    
        //�׳���ʼս���¼�
        CCombatEventManager::NotifyCombatBegin(stRoleObj);
    }

    return T_SERVER_SUCESS;
}

//todo jasonxiong4 ������������
//����ս�����͵�����
int CCombatFramework::SetCombatForm(CGameRoleObj& stRoleObj, const Zone_SetCombatForm_Request& rstRequest)
{
    //��ʼ��
    m_uiUin = stRoleObj.GetUin();
    m_stGameMsg.Clear();

    CZoneMsgHelper::GenerateMsgHead(m_stGameMsg, MSGID_ZONE_SETCOMBATFORM_RESPONSE);

    //��ȡ��Ӧ����Ϣ��
    Zone_SetCombatForm_Response* pstResp = m_stGameMsg.mutable_m_stmsgbody()->mutable_m_stzone_setcombatform_response();

   //��ȡս��
    CBattlefieldObj* pstBattlefieldObj = CCombatUtility::GetBattlefiledObj(stRoleObj);
    if(!pstBattlefieldObj)
    {
        LOGERROR("Failed to get battlefield obj, uin %u\n", stRoleObj.GetUin());
        return T_ZONE_SYSTEM_PARA_ERR;
    }

    int iRet = pstBattlefieldObj->SetCombatForm(m_uiUin, rstRequest.stform());
    if(iRet)
    {
        LOGERROR("Failed to set combat form, ret %d, uin %u\n", iRet, m_uiUin);
        return iRet;
    }

    //��䲢�ذ����ͻ���
    pstResp->set_iresult(T_SERVER_SUCESS);

    SendSuccessResponse();

    if(!pstBattlefieldObj->IsNeedSetForm())
    {
        //�������õ�������Ϣ
        pstBattlefieldObj->SendCombatFormNotify();

        //���еĶ��Ѿ�������ɣ���ʼս��
        CCombatEventManager::NotifyRoundBegin(stRoleObj);
    }

    return T_SERVER_SUCESS;
}

//�������ս��
void CCombatFramework::FinCombat(CGameRoleObj& stRoleObj)
{
    CBattlefieldObj* pstBattlefieldObj = CCombatUtility::GetBattlefiledObj(stRoleObj);
    if(!pstBattlefieldObj)
    {
        return;
    }

    if(stRoleObj.GetUin() == pstBattlefieldObj->GetActiveUin())
    {
        //����������
        pstBattlefieldObj->SetCampActiveWin(false);
    }
    else
    {
        //����������
        pstBattlefieldObj->SetCampActiveWin(true);
    }

    //����ս��������֪ͨ
    CCombatEventManager::NotifyCombatEnd(stRoleObj);

    return;
}

//��ʼ��PVEս��ս����Ϣ
int CCombatFramework::InitPveBattlefield(CGameRoleObj& stRoleObj, int iLevelID)
{
    //����ս��
    int iBattlefieldObjIndex = -1;
    CBattlefieldObj* pstBattlefieldObj = CreateBattlefieldObj(iBattlefieldObjIndex);
    if(!pstBattlefieldObj)
    {
        LOGERROR("Failed to create battlefied, uin %u\n", m_uiUin);
        return T_ZONE_SYSTEM_PARA_ERR;
    }

    //��ʼ��ս��
    int iRet = pstBattlefieldObj->InitPveBattlefield(stRoleObj, iLevelID);
    if(iRet)
    {
        LOGERROR("Failed to init pve battlefield, cross id %d, ret %d, uin %u\n", iLevelID, iRet, m_uiUin);

        CCombatUtility::ClearBattlefield(iBattlefieldObjIndex);

        return iRet;
    }

    //����������ϵ�ս����Ϣ
    stRoleObj.SetBattlefieObjID(iBattlefieldObjIndex);

    return T_SERVER_SUCESS;
}

//��ʼ��PVPս��ս����Ϣ
int CCombatFramework::InitPVPBattlefield(CGameRoleObj& stActiveRoleObj, CGameRoleObj& stPassiveRoleObj)
{
    //����ս��
    int iBattlefieldObjIndex = -1;
    CBattlefieldObj* pstBattlefieldObj = CreateBattlefieldObj(iBattlefieldObjIndex);
    if(!pstBattlefieldObj)
    {
        LOGERROR("Failed to create battlefied, uin %u\n", m_uiUin);
        return T_ZONE_SYSTEM_PARA_ERR;
    }

    //��ʼ��ս��
    int iRet = pstBattlefieldObj->InitPVPBattlefield(stActiveRoleObj, stPassiveRoleObj);
    if(iRet)
    {
        LOGERROR("Failed to init pvp battlefield, ret %d, active uin %u, passive uin %u\n", iRet, stActiveRoleObj.GetUin(), m_uiUin);

        CCombatUtility::ClearBattlefield(iBattlefieldObjIndex);
        return iRet;
    }

    //����������ϵ�ս����Ϣ
    stActiveRoleObj.SetBattlefieObjID(iBattlefieldObjIndex);
    stPassiveRoleObj.SetBattlefieObjID(iBattlefieldObjIndex);

    return T_SERVER_SUCESS;
}

//����ս�����󣬷���CBattlefieldObj��index
CBattlefieldObj* CCombatFramework::CreateBattlefieldObj(int& iBattlefieldObjIndex)
{
    iBattlefieldObjIndex = GameType<CBattlefieldObj>::Create();
    if(iBattlefieldObjIndex < 0)
    {
        LOGERROR("Failed to create battlefield obj!\n");
        return NULL;
    }

    CBattlefieldObj* pstBattlefieldObj = GameType<CBattlefieldObj>::Get(iBattlefieldObjIndex);
    if(!pstBattlefieldObj)
    {
        LOGERROR("Failed to get battlefield obj!\n");
        return NULL;
    }

    pstBattlefieldObj->SetBattlefieldObjID(iBattlefieldObjIndex);

    return pstBattlefieldObj;
}

//����ս����ʼ����Ϣ
int CCombatFramework::SendBattlefieldInitNotify(CGameRoleObj& stRoleObj)
{
    static GameProtocolMsg stBattlefieldNotify;

    CZoneMsgHelper::GenerateMsgHead(stBattlefieldNotify, MSGID_ZONE_BATTLEFIELD_NOTIFY);

    Zone_Battlefield_Notify* pstNotify = stBattlefieldNotify.mutable_m_stmsgbody()->mutable_m_stzone_battlefield_notify();

    //��ȡ��ǰ��ս������
    CBattlefieldObj* pstBattlefieldObj = GameType<CBattlefieldObj>::Get(stRoleObj.GetBattlefieObjID());
    if(!pstBattlefieldObj)
    {
        LOGERROR("Failed to get battlefield obj, uin %u\n", m_uiUin);
        return T_ZONE_SYSTEM_PARA_ERR;
    }

    int iRet = pstBattlefieldObj->PackBattlefiledInfo(*pstNotify);
    if(iRet)
    {
        LOGERROR("Failed to send battlefield notify, ret %d, uin %u\n", iRet, m_uiUin);
        return iRet;
    }

    //����֪ͨ���ͻ���
    pstBattlefieldObj->SendNotifyToBattlefield(stBattlefieldNotify);

    return T_SERVER_SUCESS;
}

//���ʹ���ɹ��ķ���
int CCombatFramework::SendSuccessResponse()
{
    CGameRoleObj* pstRoleObj = GameTypeK32<CGameRoleObj>::GetByKey(m_uiUin);
    if(!pstRoleObj)
    {
        LOGERROR("Failed to get game role obj, uin %u\n", m_uiUin);
        return -1;
    }

    CZoneMsgHelper::SendZoneMsgToRole(m_stGameMsg, pstRoleObj);

    return T_SERVER_SUCESS;
}
