
#include "GameProtocol.hpp"
#include "LogAdapter.hpp"
#include "ZoneErrorNumDef.hpp"
#include "CombatUtility.hpp"
#include "BattlefieldObj.hpp"
#include "CombatEventManager.hpp"
#include "GameRole.hpp"
#include "ZoneObjectHelper.hpp"
#include "CombatUnitAI.hpp"
#include "ZoneMsgHelper.hpp"
#include "ModuleHelper.hpp"
#include "CombatReward.hpp"
#include "DropRewardUtility.hpp"
#include "ZoneOssLog.hpp"
#include "CombatUnitObj.hpp"
#include "UnitUtility.hpp"

#include "CombatEventListener.hpp"

using namespace ServerLib;

CCombatEventListener::CCombatEventListener()
{
    m_uiUin = 0;
}

CCombatEventListener::~CCombatEventListener()
{

}

void CCombatEventListener::OnCombatBeginEvent(CGameRoleObj& stRoleObj)
{
    m_uiUin = stRoleObj.GetUin();

    //��ȡս����Ϣ
    CBattlefieldObj* pstBattlefieldObj = CCombatUtility::GetBattlefiledObj(stRoleObj);
    if(!pstBattlefieldObj)
    {
        LOGERROR("Failed to get battlefield obj, uin %u\n", m_uiUin);

        //����,����ս��
        CCombatEventManager::NotifyCombatEnd(stRoleObj);
        return;
    }

    //��ջغ���
    pstBattlefieldObj->SetCombatRoundNum(0);

    //todo jasonxiong4 �ű�����
    //����ս���ű�
    //CModuleHelper::GetStoryFramework()->DoCombatScript(m_uiUin, pstBattlefieldObj->GetCrossID(), 1);

    //�ж�ս��ʤ��
    if(pstBattlefieldObj->CanEndCombat())
    {
        //�ܹ�����ս�����׳�ս�������¼�
        CCombatEventManager::NotifyCombatEnd(stRoleObj);

        return;
    }

    if(pstBattlefieldObj->IsNeedSetForm())
    {
        //��Ҫ��������,�����������͵���Ϣ
        pstBattlefieldObj->NotifyRoleUnitDirective(UNIT_DIRECTIVE_SETFORM);

        pstBattlefieldObj->SetActionUnitStatus(ROLE_COMBAT_UNIT_STAT_SETFORM);
    }
    else
    {
        //�׳��غϿ�ʼ�¼�
        CCombatEventManager::NotifyRoundBegin(stRoleObj);
    }

    return;
}

void CCombatEventListener::OnCombatEndEvent(CGameRoleObj& stRoleObj)
{
    m_uiUin = stRoleObj.GetUin();

    //��ȡս����Ϣ
    CBattlefieldObj* pstBattlefieldObj = CCombatUtility::GetBattlefiledObj(stRoleObj);
    if(!pstBattlefieldObj)
    {
        LOGERROR("Failed to get battlefield obj, uin %u\n", m_uiUin);

        //����,����ս��
        return;
    }

    //����ս�����������Ϣ���罱����
    static GameProtocolMsg stFinCombatNotify;
    CZoneMsgHelper::GenerateMsgHead(stFinCombatNotify, MSGID_ZONE_FINCOMBAT_NOTIFY);

    Zone_FinCombat_Notify* pstNotify = stFinCombatNotify.mutable_m_stmsgbody()->mutable_m_stzone_fincombat_notify();
    pstNotify->set_bisactivewin(pstBattlefieldObj->GetIsActiveWin());

    //����������ϵ�ս��ʤ�����
    unsigned uiWinUin = pstBattlefieldObj->GetIsActiveWin() ? pstBattlefieldObj->GetActiveUin() : pstBattlefieldObj->GetPassiveUin();
    unsigned uiLoseUin = pstBattlefieldObj->GetIsActiveWin() ? pstBattlefieldObj->GetPassiveUin() : pstBattlefieldObj->GetActiveUin();

    CGameRoleObj* pstWinRoleObj = CUnitUtility::GetRoleByUin(uiWinUin);
    CGameRoleObj* pstLoseRoleObj = CUnitUtility::GetRoleByUin(uiLoseUin);

    //����������ϵ�ս�����
    if(pstWinRoleObj)
    {
        pstWinRoleObj->SetCombatResult(true);
    }

    if(pstLoseRoleObj)
    {
        pstLoseRoleObj->SetCombatResult(false);
    }

    //����ʤ���ӽ���
    if(pstWinRoleObj)
    {
        CCombatReward::GetCombatReward(pstBattlefieldObj->GetCrossID(), *pstWinRoleObj, *pstNotify->mutable_streward());
    }

    pstBattlefieldObj->SendNotifyToBattlefield(stFinCombatNotify);

    //�����PVEս��
    if(pstBattlefieldObj->GetPassiveUin() == 0)
    {
        //����Ѫ��������
        pstBattlefieldObj->SaveCombatUnitInfo();
    }

    //��ӡս����Ӫ��־
    CZoneOssLog::TraceRoleCombat(m_uiUin, pstBattlefieldObj->GetCrossID(), pstBattlefieldObj->GetCombatStartTime(), pstBattlefieldObj->GetIsActiveWin());

    //����ս��
    CCombatUtility::ClearBattlefield(stRoleObj.GetBattlefieObjID());

    return;
}

void CCombatEventListener::OnRoundBeginEvent(CGameRoleObj& stRoleObj)
{
    //����uin
    m_uiUin = stRoleObj.GetUin();

    //��ȡս����Ϣ
    CBattlefieldObj* pstBattlefieldObj = CCombatUtility::GetBattlefiledObj(stRoleObj);
    if(!pstBattlefieldObj)
    {
        LOGERROR("Failed to get battlefield obj, uin %u\n", m_uiUin);

        //����,����ս��
        CCombatEventManager::NotifyCombatEnd(stRoleObj);
        return;
    }

    //����ս���Ļغ���
    pstBattlefieldObj->SetCombatRoundNum(pstBattlefieldObj->GetCombatRoundNum()+1);

    //��ʼ�����ж��б�
    pstBattlefieldObj->InitRoundActionUnits();

    //����غϿ�ʼǰ��Ч��buff
    int iRet = pstBattlefieldObj->DoBuffEffectByType(BUFF_TRIGGER_ROUNDBEGIN);
    if(iRet)
    {
        LOGERROR("Failed to do round begin buff effect, ret %d, uin %u\n", iRet, m_uiUin);

        //����,����ս��
        CCombatEventManager::NotifyCombatEnd(stRoleObj);
        return;
    }

    //�ж�ս��ʤ��
    if(pstBattlefieldObj->CanEndCombat())
    {
        //�ܹ�����ս�����׳�ս�������¼�
        CCombatEventManager::NotifyCombatEnd(stRoleObj);

        return;
    }

    //�׳�ս����λ��ʼ�ж��غ��¼�
    CCombatEventManager::NotifyActionRoundBegin(stRoleObj);

    return;
}

void CCombatEventListener::OnRoundEndEvent(CGameRoleObj& stRoleObj)
{
    m_uiUin = stRoleObj.GetUin();

    //��ȡս����Ϣ
    CBattlefieldObj* pstBattlefieldObj = CCombatUtility::GetBattlefiledObj(stRoleObj);
    if(!pstBattlefieldObj)
    {
        LOGERROR("Failed to get battlefiled obj, uin  %u\n", m_uiUin);

        //����,����ս��
        CCombatEventManager::NotifyCombatEnd(stRoleObj);
        return;
    }

    //����غϽ�����Ч��buff
    int iRet = pstBattlefieldObj->DoBuffEffectByType(BUFF_TRIGGER_ROUNDEND);
    if(iRet)
    {
        LOGERROR("Failed to do round begin buff effect, ret %d, uin %u\n", iRet, m_uiUin);

        //����,����ս��
        CCombatEventManager::NotifyCombatEnd(stRoleObj);
        return;
    }

    //�ж�ս��ʤ��
    if(pstBattlefieldObj->CanEndCombat())
    {
        //�ܹ�����ս�����׳�ս�������¼�
        CCombatEventManager::NotifyCombatEnd(stRoleObj);

        return;
    }

    //�׳��غϿ�ʼ�¼�����ʼ�µ�ս���غ�
    CCombatEventManager::NotifyRoundBegin(stRoleObj);

    return;
}

void CCombatEventListener::OnActionRoundBeginEvent(CGameRoleObj& stRoleObj)
{
    //���ý�ɫuin
    m_uiUin = stRoleObj.GetUin();

    //��ȡս����Ϣ
    CBattlefieldObj* pstBattlefieldObj = CCombatUtility::GetBattlefiledObj(stRoleObj);
    if(!pstBattlefieldObj)
    {
        LOGERROR("Failed to get battlefiled obj, uin  %u\n", m_uiUin);

        //����,����ս��
        CCombatEventManager::NotifyCombatEnd(stRoleObj);
        return;
    }

    //�ж�ս��ʤ��
    if(pstBattlefieldObj->CanEndCombat())
    {
        //�ܹ�����ս�����׳�ս�������¼�
        CCombatEventManager::NotifyCombatEnd(stRoleObj);

        return;
    }

    //�жϵ�ǰ�Ƿ���δ���ֵ�λ�����û�����׳��غϽ����¼�
    if(!pstBattlefieldObj->HasActionCombatUnit())
    {
        //û��δ���ֵ�λ���׳�ս���غϽ����¼�
        CCombatEventManager::NotifyRoundEnd(stRoleObj);

        return;
    }

    //���͵�ǰս��ս����λ������Ϣ
    pstBattlefieldObj->SendActionUnitNotify();

    //���õ�ǰ���ֵ�ս����λ
    pstBattlefieldObj->SetActionCombatUnit();

    //���ٵ�λ����CD�غ�
    pstBattlefieldObj->DecreaseActionSkillCDRound();

    //�ж��غϿ�ʼʱ�������ж���λ���ж��غϿ�ʼ����
    int iRet = pstBattlefieldObj->DoRoundActionSkill();
    if(iRet)
    {
        LOGERROR("Failed to do round action begin skill, ret %d, uin %u\n", iRet, m_uiUin);
        
        //��������ս��
        CCombatEventManager::NotifyCombatEnd(stRoleObj);
        return;
    }

    //����ս����λ�ж�ǰ��Ч��buff
    int iActionUnitID = pstBattlefieldObj->GetActionUnitID();
    iRet = pstBattlefieldObj->DoBuffEffectByType(BUFF_TRIGGER_ACTIONROUNDBEGIN, iActionUnitID, iActionUnitID);
    if(iRet)
    {
        LOGERROR("Failed to do action round begin buff effect, ret %d, uin %u\n", iRet, m_uiUin);

        //����,����ս��
        CCombatEventManager::NotifyCombatEnd(stRoleObj);
        return;
    }

    //��ǰ���ֵ�����ǻ�飬֪ͨ�ͻ��˸��»��AI
    /*
    if(pstBattlefieldObj->GetActionUnitType() == COMBAT_UNIT_TYPE_PARTNER)
    {
        pstBattlefieldObj->NotifyRoleUnitDirective(UNIT_DIRECTIVE_PARTNERAI);

        return;
    }
    */

    //��ȡ��ǰ�ж���λ
    CCombatUnitObj* pstActionUnitObj = CCombatUtility::GetCombatUnitObj(iActionUnitID);
    if(!pstActionUnitObj)
    {
        //����,����ս��
        CCombatEventManager::NotifyCombatEnd(stRoleObj);
        return;
    }

    //ս����λ�Ƿ���Կ�ʼ�غ�
    if(pstActionUnitObj->GetCombatUnitStatus(COMBAT_UNIT_STATUS_NOTROUND))
    {
        pstActionUnitObj->SetCombatUnitStatus(COMBAT_UNIT_STATUS_NOTROUND, false);

        //���ܿ�ʼ�غϣ�ֱ�ӽ����غ�
        CCombatEventManager::NotifyActionRoundEnd(stRoleObj);
        return;
    }

    //�������ս����λ�����Ƿǻ�飬��Ԥ�ȼ���AI
    if(pstBattlefieldObj->GetActionUnitType() != COMBAT_UNIT_TYPE_ROLE &&
       pstBattlefieldObj->GetActionUnitType() != COMBAT_UNIT_TYPE_PARTNER)
    {
        iRet = CCombatUnitAI::DoActionUnitAI(stRoleObj.GetBattlefieObjID(), pstBattlefieldObj->GetActionUnitID());
        if(iRet)
        {
            LOGERROR("Failed to do action unit AI, ret %d, uin %u\n", iRet, m_uiUin);
    
            //����,����ս��
            CCombatEventManager::NotifyCombatEnd(stRoleObj);
            return;
        }
    }

    if(pstActionUnitObj->IsCombatUnitDead())
    {
        //�ж���ս����λ�Ѿ�������ֱ���ж��غϽ���
        CCombatEventManager::NotifyActionRoundEnd(stRoleObj);
        return;
    }

    //�׳�ս����λ�ƶ��¼�
    CCombatEventManager::NotifyCombatUnitMove(stRoleObj);

    return;
}

void CCombatEventListener::OnActionRoundEndEvent(CGameRoleObj& stRoleObj)
{
    m_uiUin = stRoleObj.GetUin();

    //��ȡս����Ϣ
    CBattlefieldObj* pstBattlefieldObj = CCombatUtility::GetBattlefiledObj(stRoleObj);
    if(!pstBattlefieldObj)
    {
        LOGERROR("Failed to get battlefiled obj, uin  %u\n", m_uiUin);

        //����,����ս��
        CCombatEventManager::NotifyCombatEnd(stRoleObj);
        return;
    }

    //����ս����λ�ж�����Ч��buff
    int iActionUnitID = pstBattlefieldObj->GetActionUnitID();
    int iRet = pstBattlefieldObj->DoBuffEffectByType(BUFF_TRIGGER_ACTIONROUNDEND, iActionUnitID, iActionUnitID);
    if(iRet)
    {
        LOGERROR("Failed to do action round end buff effect, ret %d, uin %u\n", iRet, m_uiUin);

        //����,����ս��
        CCombatEventManager::NotifyCombatEnd(stRoleObj);
        return;
    }

    //�ж�ս��ʤ��
    if(pstBattlefieldObj->CanEndCombat())
    {
        //�ܹ�����ս�����׳�ս�������¼�
        CCombatEventManager::NotifyCombatEnd(stRoleObj);

        return;
    }

    //����BUFF�Ļغ���
    iRet = pstBattlefieldObj->DecreaseBuffRound();
    if(iRet)
    {
        LOGERROR("Failed to decrease buff round, ret %d, uin %u\n", iRet, m_uiUin);

        //����,����ս��
        CCombatEventManager::NotifyCombatEnd(stRoleObj);
        return;
    }

    if(pstBattlefieldObj->GetActionUnitType() == COMBAT_UNIT_TYPE_ROLE ||
       pstBattlefieldObj->GetActionUnitType() == COMBAT_UNIT_TYPE_PARTNER)
    {
        //��������ս����λ�ĵ�ǰ״̬
        pstBattlefieldObj->SetActionUnitStatus(ROLE_COMBAT_UNIT_STAT_IDLE);
    }

    //�����ǰ���ֵ�ս����λ
    pstBattlefieldObj->ClearActionCombatUnit();

    //todo jasonxiong4 �ű�����
    //����ս���ű�
    //CModuleHelper::GetStoryFramework()->DoCombatScript(m_uiUin, pstBattlefieldObj->GetCrossID(), 2);

    //�׳�ս����λ�ж��غϿ�ʼ�¼�
    CCombatEventManager::NotifyActionRoundBegin(stRoleObj);

    return;
}

void CCombatEventListener::OnCombatUnitMoveEvent(CGameRoleObj& stRoleObj)
{
    m_uiUin = stRoleObj.GetUin();

    //��ȡս��
    CBattlefieldObj* pstBattlefieldObj = CCombatUtility::GetBattlefiledObj(stRoleObj);
    if(!pstBattlefieldObj)
    {
        LOGERROR("Failed to get battlefiled obj, uin %u\n", m_uiUin);

        //����,����ս��
        CCombatEventManager::NotifyCombatEnd(stRoleObj);
        return;
    }

    //���Դ����뿪λ�õĻ���
    int iRet = pstBattlefieldObj->TriggerPitfallTrap(TRAP_TRIGGER_LEAVE);
    if(iRet)
    {
        LOGERROR("Failed to trigger leave trap, ret %d, uin %u\n", iRet, m_uiUin);
        
        //��������ս��
        CCombatEventManager::NotifyCombatEnd(stRoleObj);
        return;
    }

    //�����ƶ�
    if(pstBattlefieldObj->GetActionUnitType() == COMBAT_UNIT_TYPE_ROLE ||
       pstBattlefieldObj->GetActionUnitType() == COMBAT_UNIT_TYPE_PARTNER)
    {
        //���Ϳͻ��������ƶ���ָ��
        pstBattlefieldObj->NotifyRoleUnitDirective(UNIT_DIRECTIVE_MOVE);
        
        pstBattlefieldObj->SetActionUnitStatus(ROLE_COMBAT_UNIT_STAT_MOVE);
    }
    else
    {
        //ִ���ƶ�AI
        int iRet = pstBattlefieldObj->DoCombatUnitMove(CCombatUnitAI::GetMoveTargetPath());
        if(iRet)
        {
            LOGERROR("Failed to do comat unit move AI, ret %d, uin %u\n", iRet, m_uiUin);

            //����,����ս��
            CCombatEventManager::NotifyCombatEnd(stRoleObj);

            return;
        }

        //�׳�ս����λ�ж��¼�
        CCombatEventManager::NotifyCombatUnitAction(stRoleObj);
    }

    return;
}

void CCombatEventListener::OnCombatUnitActionEvent(CGameRoleObj& stRoleObj)
{
    m_uiUin = stRoleObj.GetUin();

    //��ȡս����Ϣ
    CBattlefieldObj* pstBattlefieldObj = CCombatUtility::GetBattlefiledObj(stRoleObj);
    if(!pstBattlefieldObj)
    {
        LOGERROR("Failed to get battlefield obj, uin %u\n", m_uiUin);

        //����,����ս��
        CCombatEventManager::NotifyCombatEnd(stRoleObj);
        return;
    }

    if(pstBattlefieldObj->IsCombatUnitDead(pstBattlefieldObj->GetActionUnitID()))
    {
        //�ж��ĵ�λ�Ѿ�����,�׳�ս����λ�����ж��¼�
        CCombatEventManager::NotifyActionRoundEnd(stRoleObj);

        return;
    }

    //���Դ���ֹͣ��λ�õĻ���
    int iRet = pstBattlefieldObj->TriggerPitfallTrap(TRAP_TRIGGER_MOVEON);
    if(iRet)
    {
        LOGERROR("Failed to trigger moveon trap, ret %d, uin %u\n", iRet, m_uiUin);
        
        //��������ս��
        CCombatEventManager::NotifyCombatEnd(stRoleObj);
        return;
    }

    //����ս����λ�ж�
    if(pstBattlefieldObj->GetActionUnitType() == COMBAT_UNIT_TYPE_ROLE ||
       pstBattlefieldObj->GetActionUnitType() == COMBAT_UNIT_TYPE_PARTNER)
    {
        //�������ж���֪ͨ�ͻ��˽����ж�
        pstBattlefieldObj->NotifyRoleUnitDirective(UNIT_DIRECTIVE_ACTION);

        pstBattlefieldObj->SetActionUnitStatus(ROLE_COMBAT_UNIT_STAT_ACTION);
    }
    else
    {
        //������Թ�����ִ�й���AI
        if(CCombatUnitAI::CanDoAttack())
        {
            int iRet = pstBattlefieldObj->DoCombatCastSkill(CCombatUnitAI::GetUseSkill(), CCombatUnitAI::GetTargetUnitPos(), SKILL_USE_AI);
            if(iRet)
            {
                LOGERROR("Failed to do combat cast skill AI, ret %d, skill %d, uin %u\n", iRet, CCombatUnitAI::GetUseSkill(), m_uiUin);

                //����,����ս��
                CCombatEventManager::NotifyCombatEnd(stRoleObj);
                return;
            }
        }

        //�׳�ս����λ�����ж��¼�
        CCombatEventManager::NotifyActionRoundEnd(stRoleObj);
    }

    return;
}

