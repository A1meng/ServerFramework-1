#include <math.h>

#include "GameProtocol.hpp"
#include "MathHelper.hpp"
#include "ZoneErrorNumDef.hpp"
#include "LogAdapter.hpp"
#include "BattlefieldObj.hpp"
#include "CombatUnitObj.hpp"
#include "ModuleShortcut.hpp"
#include "Random.hpp"
#include "CombatUtility.hpp"
#include "ZoneMsgHelper.hpp"
#include "UnitUtility.hpp"
#include "FightUnitUtility.hpp"
#include "GameRole.hpp"
#include "ModuleHelper.hpp"

#include "CombatSkill.hpp"

using namespace ServerLib;

int CCombatSkill::m_iBattlefiledObjID = -1;
int CCombatSkill::m_iCastUnitID = -1;
int CCombatSkill::m_iTargetUnitID = -1;
int CCombatSkill::m_iSkillID = 0;
unsigned CCombatSkill::m_uiActiveUin = 0;
unsigned CCombatSkill::m_uiPassiveUin = 0;
int CCombatSkill::m_iDistance = 0;
int CCombatSkill::m_iChiefTargetDodge = SKILL_HIT_INVALID;
bool CCombatSkill::m_bIsScriptCast = false;
int CCombatSkill::m_iUseType = SKILL_USE_ACTIVE;
CGameRoleObj* CCombatSkill::m_pstCastRoleObj = NULL;

GameProtocolMsg CCombatSkill::m_stCombatActionNotify;
GameProtocolMsg CCombatSkill::m_stCombatAddBuffNotify;

CCombatSkill::CCombatSkill()
{

}

CCombatSkill::~CCombatSkill()
{

}

//ʹ��ս������
int CCombatSkill::CastSkill(int iBattlefieldObjID, int iCastUnitID, const TUNITPOSITION& stTargetPos, int iSkillID, int iSkillUseType, bool bIsScriptCast)
{
    //���ô���������Ϣ
    m_iBattlefiledObjID = iBattlefieldObjID;
    m_iCastUnitID = iCastUnitID;
    m_iSkillID = iSkillID;
    m_uiActiveUin = 0;
    m_uiPassiveUin = 0;
    m_iDistance = 0;
    m_iChiefTargetDodge = SKILL_HIT_INVALID;
    m_bIsScriptCast = bIsScriptCast;
    m_iUseType = iSkillUseType;
    m_pstCastRoleObj = NULL;

    //��ȡս��BattlefieldObj
    CBattlefieldObj* pstBattlefieldObj = CCombatUtility::GetBattlefiledObj(m_iBattlefiledObjID);
    if(!pstBattlefieldObj)
    {
        LOGERROR("Failed to get battlefield obj, invalid obj id %d\n", m_iBattlefiledObjID);
        return T_ZONE_SYSTEM_PARA_ERR;
    }

    m_uiActiveUin = pstBattlefieldObj->GetActiveUin();
    m_uiPassiveUin = pstBattlefieldObj->GetPassiveUin();

    //�ж�����uin
    unsigned uiCastUin = (pstBattlefieldObj->GetCombatUnitCamp(iCastUnitID)==FIGHT_CAMP_ACTIVE) ? m_uiActiveUin : m_uiPassiveUin;
    m_pstCastRoleObj = CUnitUtility::GetRoleByUin(uiCastUin);
    if(uiCastUin!=0 && !m_pstCastRoleObj)
    {
        LOGERROR("Failed to get cast role obj, uin %u\n", uiCastUin);
        return T_ZONE_GAMEROLE_NOT_EXIST;
    }

    //����ս����λ��ʼ�ж���֪ͨ
    static GameProtocolMsg stBeginActionMsg;
    CZoneMsgHelper::GenerateMsgHead(stBeginActionMsg, MSGID_ZONE_BEGINCOMBATACTION_NOTIFY);

    pstBattlefieldObj->SendNotifyToBattlefield(stBeginActionMsg);

    //��ȡʩ���߶���
    CCombatUnitObj* pstCastUnitObj = CCombatUtility::GetCombatUnitObj(iCastUnitID);
    if(!pstCastUnitObj)
    {
        LOGERROR("Failed to get cast unit obj, invalid unit id %d\n", iCastUnitID);
        return T_ZONE_SYSTEM_PARA_ERR;
    }

    //��ȡĿ�����
    int iTargetUnitID = -1;
    CCombatUnitObj* pstTargetUnitObj = pstBattlefieldObj->GetCombatUnitByPos(stTargetPos);
    if(pstTargetUnitObj)
    {
        iTargetUnitID = pstTargetUnitObj->GetCombatUnitID();
    }

    if(pstCastUnitObj->IsCombatUnitDead() || (pstTargetUnitObj&&pstTargetUnitObj->IsCombatUnitDead()))
    {
        //���������ط�������ֱ�ӷ���
        return T_SERVER_SUCESS;
    }

    //��ȡ���ܱ������
    const SFightUnitSkillConfig* pstSkillConfig = FightSkillCfgMgr().GetConfig(iSkillID);
    if(!pstSkillConfig)
    {
        LOGERROR("Failed to get fight skill config, invalid skill id %d\n", iSkillID);
        return T_ZONE_SYSTEM_INVALID_CFG;
    }

    //����Ƿǽű�������ʹ��,����CD����߼�
    if(m_iUseType==SKILL_USE_ACTIVE && !bIsScriptCast)
    {
        if(pstCastUnitObj->GetSkillCDRound(iSkillID) != 0)
        {
            //���ܻ���CD״̬
            LOGERROR("Failed to cast active skill, active uin %u, unit id %d, skill %d\n", m_uiActiveUin, iCastUnitID, iSkillID);
            return T_ZONE_SYSTEM_PARA_ERR;
        }

        //���ü���CD״̬
        static GameProtocolMsg stSkillCDMsg;
        CZoneMsgHelper::GenerateMsgHead(stSkillCDMsg, MSGID_ZONE_SKILLCDROUND_NOTIFY);
        Zone_SkillCDRound_Notify* pstCDNotify = stSkillCDMsg.mutable_m_stmsgbody()->mutable_m_stzone_skillcdround_notify();

        pstCastUnitObj->SetSkillCDRound(iSkillID, pstSkillConfig->iCDRound);
        pstCDNotify->set_iunitid(iCastUnitID);
        SkillCDInfo* pstInfo = pstCDNotify->add_stcdinfos();
        pstInfo->set_iskillid(iSkillID);
        pstInfo->set_icdround(pstSkillConfig->iCDRound);

        pstBattlefieldObj->SendNotifyToBattlefield(stSkillCDMsg);
    }

    //�жϹ���˫��λ���Ƿ�Ϸ�
    std::vector<TUNITPOSITION> vCheckPos;
    if(pstTargetUnitObj)
    {
        for(int x=0; x<=pstTargetUnitObj->GetUnitSize(); ++x)
        {
            for(int y=0; y<=pstTargetUnitObj->GetUnitSize(); ++y)
            {
                vCheckPos.push_back(TUNITPOSITION(stTargetPos.iPosX+x, stTargetPos.iPosY-y));
            }
        }
    }
    else
    {
        vCheckPos.push_back(stTargetPos);
    }

    for(unsigned i=0; i<vCheckPos.size(); ++i)
    {
        m_iDistance = CCombatUtility::GetAttackDistance(pstCastUnitObj->GetUnitPosition(), vCheckPos[i], pstSkillConfig->iTargetRangeID);
        if(m_iDistance != 0)
        {
            break;
        }
    }

    if(m_iDistance == 0)
    {
        if(m_iUseType!=SKILL_USE_LIANJI &&
           m_iUseType!=SKILL_USE_JIAJI &&
           m_iUseType!=SKILL_USE_FANJI &&
           m_iUseType!=SKILL_USE_HEJI &&
           m_iUseType!=SKILL_USE_AI)
        {
            //��Ȼ���еļ��ܣ�������
            LOGERROR("Failed to cast skill, invalid position, skill id %d, uin %u\n", iSkillID, m_uiActiveUin);
            return T_ZONE_SYSTEM_PARA_ERR;
        }

        return T_SERVER_SUCESS;
    }

    //���������ķ������
    CCombatUtility::UpdateUnitDirection(*pstCastUnitObj, stTargetPos);

    //��ʼ�����͵���Ϣ
    CZoneMsgHelper::GenerateMsgHead(m_stCombatActionNotify, MSGID_ZONE_COMBATACTION_NOTIFY);
    CZoneMsgHelper::GenerateMsgHead(m_stCombatAddBuffNotify, MSGID_ZONE_COMBATADDBUFF_NOTIFY);

    //��ȡ����BUFF���͵���Ϣ��
    Zone_CombatAddBuff_Notify* pstAddBuffNotify = m_stCombatAddBuffNotify.mutable_m_stmsgbody()->mutable_m_stzone_combataddbuff_notify();

    //��ȡ����Ч������Ϣ��
    Zone_CombatAction_Notify* pstNotify = m_stCombatActionNotify.mutable_m_stmsgbody()->mutable_m_stzone_combataction_notify();

    //���ü����ͷ������Ϣ
    pstNotify->set_iactionunitid(m_iCastUnitID);
    pstNotify->set_etype(COMBAT_ACTION_CASTSKILL);
    pstNotify->set_icastskillid(m_iSkillID);
    pstNotify->set_iuseskilltype(m_iUseType);
    pstNotify->mutable_sttargetpos()->set_iposx(stTargetPos.iPosX);
    pstNotify->mutable_sttargetpos()->set_iposy(stTargetPos.iPosY);

    //���Ӹ��Լ���buff
    int iRet = pstCastUnitObj->AddUnitBuff(m_uiActiveUin, pstBattlefieldObj->GetCrossID(), pstSkillConfig->iSelfBuff, iCastUnitID, *pstAddBuffNotify);
    if(iRet)
    {
        LOGERROR("Failed to add self unit buff, ret %d, buff id %d, uin %u\n", iRet, pstSkillConfig->iSelfBuff, m_uiActiveUin);
        return iRet;
    }

    //��ȡ�˺���Χ��
    const SSkillAreaConfig* pstAreaConfig = SkillAreaCfgMgr().GetConfig(pstSkillConfig->iTargetAreaID);
    if(!pstAreaConfig)
    {
        LOGERROR("Failed to get skill area config, invalid area id %d\n", pstSkillConfig->iTargetAreaID);
        return T_ZONE_SYSTEM_INVALID_CFG;
    }

    TUNITPOSITION stTmpPos;
    TUNITPOSITION stTmpTargetPos;
    stTmpTargetPos.iPosX = stTargetPos.iPosX;
    stTmpTargetPos.iPosY = stTargetPos.iPosY;

    bool bIsChiefTarget = false;
    std::vector<int> vProcessedTargets;

    for(int i=0; i<pstAreaConfig->iAreaPosNum; ++i)
    {
        //���ݷ��������߻����õ�����
        CCombatUtility::FixSkillAreaPos(pstCastUnitObj->GetUnitDirection(), pstAreaConfig->astTargetPosInfo[i], stTmpPos);

        stTmpPos.iPosX += stTmpTargetPos.iPosX;
        stTmpPos.iPosY += stTmpTargetPos.iPosY;

        CCombatUnitObj* pstOneUnitObj = pstBattlefieldObj->GetCombatUnitByPos(stTmpPos);
        if(!pstOneUnitObj || pstOneUnitObj->IsCombatUnitDead())
        {
            //Ŀ��λ���޵�λ��������
            continue;
        }

        //�ж��Ƿ���ȷ���ͷ�Ŀ��
        if(!CheckIsValidTarget(*pstBattlefieldObj, pstSkillConfig->iTargetType, iCastUnitID, pstOneUnitObj->GetCombatUnitID()))
        {
            continue;
        }

        //�ж�Ŀ���Ƿ��Ѿ�������������ڴ������λ
        if(std::find(vProcessedTargets.begin(),vProcessedTargets.end(), pstOneUnitObj->GetCombatUnitID()) != vProcessedTargets.end())
        {
            //�Ѿ������
            continue;
        }

        vProcessedTargets.push_back(pstOneUnitObj->GetCombatUnitID());

        if(pstOneUnitObj->GetCombatUnitID() == iTargetUnitID)
        {
            bIsChiefTarget = true;
        }
        else
        {
            bIsChiefTarget = false;
        }

        //���㼼��Ч��
        iRet = CastSkillToOneTarget(iCastUnitID, pstOneUnitObj->GetCombatUnitID(), iSkillID, bIsChiefTarget);
        if(iRet)
        {
            LOGERROR("Failed to cast skill to one target, skill id %d, ret %d, uin %u\n", iSkillID, iRet, m_uiActiveUin);
            return iRet;
        }
    }

    //�����˺�����Ϣ
    pstBattlefieldObj->SendNotifyToBattlefield(m_stCombatActionNotify);

    //��������BUFF����Ϣ
    if(pstAddBuffNotify->staddbuffs_size() != 0)
    {
        pstBattlefieldObj->SendNotifyToBattlefield(m_stCombatAddBuffNotify);
    }

    //���ܴ����������ýű�
    //int iExecuteType = (m_iChiefTargetDodge!=SKILL_HIT_CHIEFNODODGE) ? SKILL_SCRIPT_CALL_CHIEFDODGE : SKILL_SCRIPT_CALL_SKILLDONE;

    //todo jasonxiong4 �ű�����
    //CModuleHelper::GetStoryFramework()->DoSkillScript(m_uiActiveUin, pstBattlefieldObj->GetCrossID(), iSkillID, iCastUnitID, 
                                                      //stTmpTargetPos.iPosX, stTmpTargetPos.iPosY, iExecuteType);

    return T_SERVER_SUCESS;
}

//�Ե���Ŀ��ʹ��ս������
int CCombatSkill::CastSkillToOneTarget(int iCastUnitID, int iTargetUnitID, int iSkillID, bool bIsChiefTarget)
{
    m_iTargetUnitID = iTargetUnitID;

    //��ȡ����Ч������Ϣ��
    Zone_CombatAction_Notify* pstNotify = m_stCombatActionNotify.mutable_m_stmsgbody()->mutable_m_stzone_combataction_notify();

    //��ȡ����BUFF����Ϣ��
    Zone_CombatAddBuff_Notify* pstAddBuffNotify = m_stCombatAddBuffNotify.mutable_m_stmsgbody()->mutable_m_stzone_combataddbuff_notify();

    //���ù���Ŀ��������Ϣ
    ActionTarget* pstTargets = pstNotify->add_sttargets();
    pstTargets->set_itargetunitid(m_iTargetUnitID);

    //��ʼ��״̬����
    for(int i=ACTION_EFFECT_STATUS_MINZHONG; i<ACTION_EFFECT_STATUS_MAX; ++i)
    {
        pstTargets->add_bisstatset(false);
    }

    //��ʼ����λ���Ե�Ч��Ӱ��
    for(int i=0; i<FIGHT_ATTR_MAX; ++i)
    {
        pstTargets->add_iattreffect(0);
        pstTargets->add_icastattreffect(0);
    }

    //��ȡս��
    CBattlefieldObj* pstBattlefieldObj = CCombatUtility::GetBattlefiledObj(m_iBattlefiledObjID);
    if(!pstBattlefieldObj)
    {
        LOGERROR("Failed to get battlefield obj, invalid battlefield index %d\n", m_iBattlefiledObjID);
        return T_ZONE_SYSTEM_PARA_ERR;
    }

    //��ȡ���ܵ�����
    const SFightUnitSkillConfig* pstSkillConfig = FightSkillCfgMgr().GetConfig(iSkillID);
    if(!pstSkillConfig)
    {
        LOGERROR("Failed to cast skill, invalid skill id %d\n", iSkillID);
        return T_ZONE_SYSTEM_INVALID_CFG;
    }

    //��ȡʹ�ü��ܵ�ս����λ
    CCombatUnitObj* pstCastUnitObj = CCombatUtility::GetCombatUnitObj(iCastUnitID);
    if(!pstCastUnitObj)
    {
        LOGERROR("Failed to get cast skill combat unit obj, index %d, uin %u\n", iCastUnitID, m_uiActiveUin);
        return T_ZONE_SYSTEM_PARA_ERR;
    }

    //��ȡ�����ͷŵ�Ŀ�굥λ
    CCombatUnitObj* pstTargetUnitObj = CCombatUtility::GetCombatUnitObj(iTargetUnitID);
    if(!pstTargetUnitObj)
    {
        LOGERROR("Failed to get skill target combat unit obj, index %d, uin %u\n", iTargetUnitID, m_uiActiveUin);
        return T_ZONE_SYSTEM_PARA_ERR;
    }

    int iRet = T_SERVER_SUCESS;

    //�жϼ�����������
    if(pstSkillConfig->iHitModifierID == 0)
    {
        //����û����������

        //�������ҪĿ�꣬��������ҪĿ�겻�������
        if(bIsChiefTarget)
        {
            m_iChiefTargetDodge = SKILL_HIT_NOHIT;
        }

        //�����˺�
        int iRealDamageNum = 0;
        iRet = pstTargetUnitObj->AddFightAttr(FIGHT_ATTR_HP, -pstSkillConfig->iDamageBaseNum, &iRealDamageNum);
        if(iRet)
        {
            LOGERROR("Failed to add fight attr, ret %d, uin %u\n", iRet, m_uiActiveUin);
            return iRet;
        }

        //���ͻ��˵�Ѫ�ķ���
        pstTargets->set_iattreffect(FIGHT_ATTR_HP, iRealDamageNum);

        //��Ŀ������buff
        iRet = pstTargetUnitObj->AddUnitBuff(m_uiActiveUin, pstBattlefieldObj->GetCrossID(), pstSkillConfig->iTargetBuff, iCastUnitID, *pstAddBuffNotify);
        if(iRet)
        {
            LOGERROR("Failed to add target unit buff, ret %d, buff id %d, uin %u\n", iRet, pstSkillConfig->iTargetBuff, m_uiActiveUin);
            return iRet;
        }

        //��������˺����򴥷�Ŀ�����˺����͵�BUFF
        if(pstSkillConfig->iDamageBaseNum > 0)
        {
            iRet = pstBattlefieldObj->DoBuffEffectByType(BUFF_TRIGGER_UNDERDAMAGE, iTargetUnitID, iCastUnitID);
            if(iRet)
            {
                LOGERROR("Failed to do under damage buff effect, target unit %d, trigger unit %d, uin %u\n", iTargetUnitID, iCastUnitID, m_uiActiveUin);
                return iRet;
            }
        }
    }
    else
    {
        //�����Ƿ�����
        bool bIsHit = CheckCanHit(*pstCastUnitObj, *pstTargetUnitObj, *pstSkillConfig);
        if(!bIsHit)
        {
            //δ����,��Ϊ����״̬
            pstTargets->set_bisstatset(ACTION_EFFECT_STATUS_SHANBI, true);

            //�������ҪĿ�꣬��������ҪĿ������
            if(bIsChiefTarget)
            {
                m_iChiefTargetDodge = SKILL_HIT_CHIEFDODGE;
            }

            //����Ŀ��ת��Ϊ�泯�������ķ���
            TargetUnderAttackDirection(*pstTargetUnitObj, pstCastUnitObj->GetUnitDirection());

            //�������ܣ������������͵�BUFF
            iRet = pstBattlefieldObj->DoBuffEffectByType(BUFF_TRIGGER_DODGE, iTargetUnitID, iCastUnitID);
            if(iRet)
            {
                LOGERROR("Failed to do dodge buff effect, ret %d, target unit %d, cast unit %d, uin %u\n", iRet, iTargetUnitID, iCastUnitID, m_uiActiveUin);
                return iRet;
            }
        }
        else
        {
            //����
            pstTargets->set_bisstatset(ACTION_EFFECT_STATUS_MINZHONG, true);

            //�������ҪĿ�꣬��������ҪĿ��δ����
            if(bIsChiefTarget)
            {
                m_iChiefTargetDodge = SKILL_HIT_CHIEFNODODGE;
            }

            //������ط����ܻ�buff
            iRet = pstBattlefieldObj->DoBuffEffectByType(BUFF_TRIGGER_UNDERATTACK, iTargetUnitID, iCastUnitID);
            if(iRet)
            {
                LOGERROR("Failed to do under attack buff effect, ret %d, target unit %d, cast unit %d, uin %u\n", iRet, iTargetUnitID, iCastUnitID, m_uiActiveUin);
                return iRet;
            }

            int iDamageNum = 0;
            if(pstSkillConfig->iDamageID != 0)
            {
                //�ж��Ƿ��
                bool bIsBlock = CheckCanBlock(*pstCastUnitObj, *pstTargetUnitObj, *pstSkillConfig);
    
                //�ж��Ƿ񱩻�
                bool bIsCrit = CheckCanCrit(*pstCastUnitObj, *pstTargetUnitObj, *pstSkillConfig);
    
                //�����˺�
                iDamageNum = GetSkillHurtNum(*pstCastUnitObj, *pstTargetUnitObj, *pstSkillConfig, bIsChiefTarget);

                //���ӱ�������
                if(bIsCrit)
                {
                    iDamageNum = iDamageNum * pstSkillConfig->iCritEffect / 10000;
                    pstTargets->set_bisstatset(ACTION_EFFECT_STATUS_BAOJI, true);
                }
    
                //���Ӹ�����
                if(bIsBlock)
                {
                    iDamageNum = iDamageNum * pstSkillConfig->iParryEffect / 10000;
                    pstTargets->set_bisstatset(ACTION_EFFECT_STATUS_GEDANG, true);
    
                    //����Ŀ��ת��Ϊ�泯�������ķ���
                    TargetUnderAttackDirection(*pstTargetUnitObj, pstCastUnitObj->GetUnitDirection());
                }
    
                iDamageNum = (iDamageNum<=0) ? 1 : iDamageNum;
    
                //����Э�����߼�
                float fCoordEffectParam = ProcessCoordDefense(*pstBattlefieldObj, *pstTargetUnitObj);
                iDamageNum = (int)(iDamageNum*fCoordEffectParam);
    
                //�������˺��Ľű��߼�
                //const TUNITPOSITION& stTargetPos = pstTargetUnitObj->GetUnitPosition();
                //todo jasonxiong4 �ű�����
                /*
                int iDamageRate = CModuleHelper::GetStoryFramework()->DoSkillScript(m_uiActiveUin, pstBattlefieldObj->GetCrossID(), iSkillID, 
                                                                                iCastUnitID, stTargetPos.iPosX, stTargetPos.iPosY, SKILL_SCRIPT_CALL_DAMAGE);
                if(iDamageRate != 0)
                {
                    iDamageNum = iDamageNum*iDamageRate/100;
                }
                */
                    
                //�˺��Ĵ�������Ŀ����˺��͹������ķ���
                iRet = ProcessRealDamage(*pstBattlefieldObj, *pstCastUnitObj, *pstTargetUnitObj, iDamageNum, *pstTargets, *pstSkillConfig);
                if(iRet)
                {
                    LOGERROR("Failed to process real damage, skill id %d, uin %u\n", pstSkillConfig->iConfigID, m_uiActiveUin);
                    return iRet;
                }
            }

            //��������ɵĹ���˫����λ��,ֻ����ҪĿ����Ч
            if(bIsChiefTarget)
            {
                iRet = ProcessSkillMove(*pstBattlefieldObj, *pstCastUnitObj, *pstTargetUnitObj, *pstTargets, *pstSkillConfig);
                if(iRet)
                {
                    LOGERROR("Failed to process skill move, skill id %d, ret %d, uin %u\n", iSkillID, iRet, m_uiActiveUin);
                    return iRet;
                }
            }

            //Ŀ������Targetbuff
            iRet = pstTargetUnitObj->AddUnitBuff(m_uiActiveUin, pstBattlefieldObj->GetCrossID(), pstSkillConfig->iTargetBuff, iCastUnitID, *pstAddBuffNotify);
            if(iRet)
            {
                LOGERROR("Failed to add unit buff, ret %d, uin %u\n", iRet, m_uiActiveUin);
                return iRet;
            }

            //���ط��������˺�buff
            if(iDamageNum > 0)
            {
                //�����˺���Ŀ�괥�����˺�BUFF
                iRet = pstBattlefieldObj->DoBuffEffectByType(BUFF_TRIGGER_UNDERDAMAGE, iTargetUnitID, iCastUnitID);
                if(iRet)
                {
                    LOGERROR("Failed to do under damage buff effect, ret %d, target unit %d, cast unit %d, uin %u\n", iRet, iTargetUnitID, iCastUnitID, m_uiActiveUin);
                    return iRet;
                }
            }
        }

        //����Ŀ��Ϊ�Լ���󹥻���λ
        pstCastUnitObj->SetLastAttackUnitID(iTargetUnitID);
    }

    //����ҪĿ�겢�Ҽ��ܻ��������Ч������������Ч��
    if(bIsChiefTarget && pstSkillConfig->iSpecialFuncType!=0)
    {
        iRet = ProcessSkillSpecailFunc(*pstCastUnitObj, *pstTargetUnitObj, *pstTargets, *pstSkillConfig);
        if(iRet)
        {
            LOGERROR("Failed to process skill specail func, ret %d, skill id %d, uin %u\n", iRet, iSkillID, m_uiActiveUin);
            return iRet;
        }
    }

    return T_SERVER_SUCESS;
}

//�����ж�
bool CCombatSkill::CheckCanHit(CCombatUnitObj& stCastUnitObj, CCombatUnitObj& stTargetUnitObj, const SFightUnitSkillConfig& stSkillConfig)
{
    //������Ŀ�������״̬,��Ŀ��HP < 60%*MAXHP ʱ�޷�����
    if(stTargetUnitObj.GetFightAttr(FIGHT_ATTR_HP) < stTargetUnitObj.GetFightAttr(FIGHT_ATTR_HPMAX)*60/100)
    {
        return true;
    }

    //������ʽ����
    int iSkillHit = CCombatUtility::GetSkillHit(stCastUnitObj, stSkillConfig, m_iDistance);

    //������ʽ����
    int iSkillDodge = CCombatUtility::GetSkillDodge(stCastUnitObj, stTargetUnitObj, stSkillConfig);

    //�ж���ʽ RAND(0,1) <= (��ʽ����)/(��ʽ����+��ʽ����)
    float fHitRate = (float)iSkillHit/(float)(iSkillDodge+iSkillHit)*100;

    int iRandNum = CRandomCalculator::GetRandomNumberInRange(100);
    if(iRandNum <= (int)fHitRate)
    {
        return true;
    }

    return false;
}

//���ж�
bool CCombatSkill::CheckCanBlock(CCombatUnitObj& stCastUnitObj, CCombatUnitObj& stTargetUnitObj, const SFightUnitSkillConfig& stSkillConfig)
{
    //����Ŀ�������״̬,��Ŀ�� HP < 30%*MAXHP ʱ�޷���
    if(stTargetUnitObj.GetFightAttr(FIGHT_ATTR_HP) < stTargetUnitObj.GetFightAttr(FIGHT_ATTR_HPMAX)*30/100)
    {
        return false;
    }

    //��ʽ��׼
    const SSkillScoreConfig* pstScoreConfig = SkillScoreCfgMgr().GetConfig(stSkillConfig.iPrecisionModifierID);
    if(!pstScoreConfig)
    {
        LOGERROR("Failed to get skill score config, invalid precision id %d\n", stSkillConfig.iPrecisionModifierID);
        return false;
    }

    int iSkillPrecision = CCombatUtility::GetSkillScoreNum(stCastUnitObj, *pstScoreConfig);

    //��ʽ��
    pstScoreConfig = SkillScoreCfgMgr().GetConfig(stSkillConfig.iCopeModifierID);
    if(!pstScoreConfig)
    {
        LOGERROR("Failed to get skill score config, invalid cope id %d\n", stSkillConfig.iCopeModifierID);
        return false;
    }

    int iSkillBlock = CCombatUtility::GetSkillScoreNum(stTargetUnitObj, *pstScoreConfig);

    //���ж�
    int iRandomNum = CRandomCalculator::GetRandomNumberInRange(100);

    //���㹫ʽ�� RAND(0,1) < ��ʽ��׼/(��ʽ��׼+��ʽ��)  ��ʱΪ��ʧ��
    if(iRandomNum*iSkillBlock >= (100-iRandomNum)*iSkillPrecision)
    {
        return true;
    }

    return false;
}

//�����ж�
bool CCombatSkill::CheckCanCrit(CCombatUnitObj& stCastUnitObj, CCombatUnitObj& stTargetUnitObj, const SFightUnitSkillConfig& stSkillConfig)
{
    //��ʽ����
    const SSkillScoreConfig* pstScoreConfig = SkillScoreCfgMgr().GetConfig(stSkillConfig.iCritModifierID);
    if(!pstScoreConfig)
    {
        LOGERROR("Failed to get skill score config, invalid crit id %d\n", stSkillConfig.iCritModifierID);
        return false;
    }

    int iSkillCrit = CCombatUtility::GetSkillScoreNum(stCastUnitObj, *pstScoreConfig);

    //��ʽ����
    pstScoreConfig = SkillScoreCfgMgr().GetConfig(stSkillConfig.iToughModifierID);
    if(!pstScoreConfig)
    {
        LOGERROR("Failed to get skill score config, invalid tough id %d\n", stSkillConfig.iToughModifierID);
        return false;
    }

    int iSkillTough = CCombatUtility::GetSkillScoreNum(stTargetUnitObj, *pstScoreConfig);

    //�����ж�
    int iRandomNum = CRandomCalculator::GetRandomNumberInRange(100);

    //���㹫ʽ�� RAND(0,1) < ��ʽ��׼/(��ʽ��׼+��ʽ��)  ��ʱΪ��ʧ��
    if(iRandomNum*iSkillTough <= (100-iRandomNum)*iSkillCrit)
    {
        return true;
    }

    return false;
}

//�����˺�
int CCombatSkill::GetSkillHurtNum(CCombatUnitObj& stCastUnitObj, CCombatUnitObj& stTargetUnitObj, const SFightUnitSkillConfig& stSkillConfig, bool bIsChiefTarget)
{
    //��������
    int iBaseHurt = stSkillConfig.iDamageBaseNum;

    if(stSkillConfig.iDamageModifierID != 0)
    {
        //�����˺�����
        const SSkillScoreConfig* pstScoreConfig = SkillScoreCfgMgr().GetConfig(stSkillConfig.iDamageModifierID);
        if(!pstScoreConfig)
        {
            LOGERROR("Failed to get score config, damage modifier score id %d\n", stSkillConfig.iDamageModifierID);
            return T_ZONE_SYSTEM_INVALID_CFG;
        }
    
        int iDamageModifier = CCombatUtility::GetSkillScoreNum(stCastUnitObj, *pstScoreConfig);

        iBaseHurt += iDamageModifier;
    }

    //�������� = ��������*range[�˺��ֲ�].Rx	//xΪĿ�����
    int iRangeIndex = m_iDistance;
    if(iRangeIndex >= MAX_SKILL_RANGE_INFO_NUM)
    {
        iRangeIndex = MAX_SKILL_RANGE_INFO_NUM - 1;
    }

    //����ת��Ϊ�±���Ҫ��1
    iRangeIndex = iRangeIndex - 1;
    
    //��ȡRange��
    const SSkillRangeConfig* pstRangeConfig = SkillRangeCfgMgr().GetConfig(stSkillConfig.iDamageRangeID);
    if(!pstRangeConfig)
    {
        LOGERROR("Failed to get skill range config, invalid range id %d\n", stSkillConfig.iDamageRangeID);

        return 0;
    }

    int iAttackHurt = iBaseHurt*pstRangeConfig->aiRangeInfo[iRangeIndex]/10000;
    
    //����ǿ��
    int iAttackStrength = CCombatUtility::GetAttackStrength(stCastUnitObj, stSkillConfig);

    //����ǿ��
    int iDefenceStrength = CCombatUtility::GetDefenceStrength(stCastUnitObj, stTargetUnitObj, stSkillConfig);

	int iDenominator = iAttackStrength+iDefenceStrength;
	if(iDenominator <= 0)
	{
		iDenominator = 1;
	}

    if(!bIsChiefTarget)
    {
        //��ҪĿ��
        return (iAttackHurt * iAttackStrength /iDenominator)*stSkillConfig.iSecondaryTarget/10000;
    }

    //��ҪĿ��
    return (iAttackHurt * iAttackStrength /iDenominator);
}

//����Ƿ���Ч�ļ����ͷ�Ŀ��
bool CCombatSkill::CheckIsValidTarget(CBattlefieldObj& stBattlefieldObj, int iTargetType, int iCastUnitID, int iTargetUnitID)
{
    //���ս����λ����Ӫ��Ϣ
    FightCampType eCastCampType = (FightCampType)stBattlefieldObj.GetCombatUnitCamp(iCastUnitID);
    FightCampType eTargetCampType = (FightCampType)stBattlefieldObj.GetCombatUnitCamp(iTargetUnitID);
    if(eCastCampType == FIGHT_CAMP_INVALID || eTargetCampType == FIGHT_CAMP_INVALID)
    {
        LOGERROR("Failed to cast skill, invalid camp type, active uin %u\n", m_uiActiveUin);
        return false;
    }

    //�ж�Ŀ���Ƿ��ǵ���
    bool bIsEnemy = (eCastCampType != eTargetCampType);
	
	switch(iTargetType)
    {
    case SKILL_TARGET_SELF:
        {
            //ֻ�ܶ��Լ�ʹ��
            if(iTargetUnitID != iCastUnitID)
            {
                return false;
            }
        }
        break;

    case SKILL_TARGET_CAMP:
        {
            //ֻ�ܶ��ѷ�ʹ��
            if((iTargetUnitID==iCastUnitID) || bIsEnemy)
            {
                return false;
            }
        }
        break;

    case SKILL_TARGET_NOT_ENEMY:
        {
            //���ԶԷǵз�ʹ��
            if(bIsEnemy)
            {
                return false;
            }
        }
        break;

    case SKILL_TARGET_ENEMY:
        {
            //���ԶԵ���ʹ��
            if(!bIsEnemy)
            {
                return false;
            }
        }
        break;

    case SKILL_TARGET_ALL:
        {
            //���Զ�������ʹ��
            return true;
        }
        break;

    default:
        {
            return false;
        }
        break;
    }

    return true;
}

//�����ܻ���λ��ת��
void CCombatSkill::TargetUnderAttackDirection(CCombatUnitObj& stTargetUnitObj, int iCastUnitDirection)
{
    switch(iCastUnitDirection)
    {
    case COMBAT_DIRECTION_EAST:
        {
            stTargetUnitObj.SetUnitDirection(COMBAT_DIRECTION_WEST);
        }
        break;

    case COMBAT_DIRECTION_NORTH:
        {
            stTargetUnitObj.SetUnitDirection(COMBAT_DIRECTION_SOUTH);
        }
        break;

    case COMBAT_DIRECTION_WEST:
        {
            stTargetUnitObj.SetUnitDirection(COMBAT_DIRECTION_EAST);
        }
        break;

    case COMBAT_DIRECTION_SOUTH:
        {
            stTargetUnitObj.SetUnitDirection(COMBAT_DIRECTION_NORTH);
        }
        break;

    default:
        {
            ;
        }
        break;
    }

    return;
}

//����Ŀ�굥λ���˺�
int CCombatSkill::ProcessRealDamage(CBattlefieldObj& stBattlefieldObj, CCombatUnitObj& stCastUnitObj, CCombatUnitObj& stTargetUnitObj, int iDamageNum, ActionTarget& stNotify, const SFightUnitSkillConfig& stSkillConfig)
{
    //��ȡ�˺���
    const SSkillDamageConfig* pstDamageConfig = SkillDamageCfgMgr().GetConfig(stSkillConfig.iDamageID);
    if(!pstDamageConfig)
    {
        LOGERROR("Failed to get skill damage config, skill id %d, damage id %d\n", stSkillConfig.iConfigID, stSkillConfig.iDamageID);
        return T_ZONE_SYSTEM_INVALID_CFG;
    }

    //�ȴ��������ķ���
    int iRealAddHP = 0;

    stCastUnitObj.AddFightAttr(FIGHT_ATTR_HP, iDamageNum*pstDamageConfig->aiVampireAttr[FIGHT_ATTR_HP]/100, &iRealAddHP);

    //����������Ϣ�ķ���
    stNotify.set_icastattreffect(FIGHT_ATTR_HP, iRealAddHP);

    //�����ػ����͵�BUFF,��������л��޸�iDamageNum��ֵ
    stBattlefieldObj.DoBuffEffectByType(BUFF_TRIGGER_PROCESSDAMAGE, stTargetUnitObj.GetCombatUnitID(), stCastUnitObj.GetCombatUnitID(), &iDamageNum);

    //�����������˺�
    ProcessTargetDamage(stTargetUnitObj, *pstDamageConfig, stNotify, FIGHT_ATTR_HP, iDamageNum);

    return T_SERVER_SUCESS;
}

//����Ŀ�굥λ�������Ե��˺�
void CCombatSkill::ProcessTargetDamage(CCombatUnitObj& stTargetUnitObj, const SSkillDamageConfig& stDamageConfig, ActionTarget& stNotify, int iAttrType, int& iDamageNum)
{
    if(iDamageNum<=0 || iAttrType<0 || iAttrType>=FIGHT_ATTR_MAX || stDamageConfig.aiSpreadAttr[iAttrType]<=0)
    {
        return;
    }

    int iAttrDamage = iDamageNum*stDamageConfig.aiSpreadAttr[iAttrType]*stDamageConfig.aiStrengthAttr[iAttrType]/100/100;
    iAttrDamage = stTargetUnitObj.GetFightAttr(iAttrType)>iAttrDamage ? iAttrDamage : stTargetUnitObj.GetFightAttr(iAttrType);

    int iRealDamage = 0;
    stTargetUnitObj.AddFightAttr(iAttrType, -iAttrDamage, &iRealDamage);

    iDamageNum -= (iAttrDamage*100/stDamageConfig.aiStrengthAttr[iAttrType]);

    stNotify.set_iattreffect(iAttrType, iRealDamage);

    return;
}

//�����ܻ����Ķ���Э��,����ֵΪЭ����Ч��ϵ��
float CCombatSkill::ProcessCoordDefense(CBattlefieldObj& stBattlefieldObj, CCombatUnitObj& stTargetUnitObj)
{
    int iNearByPartnerNum = stBattlefieldObj.GetNearByUnitNum(stTargetUnitObj, false);
    if(iNearByPartnerNum == 0)
    {
        return 1.0f;
    }

    int iAngleNearByPartnerNum = stBattlefieldObj.GetAngleNearByUnitNum(stTargetUnitObj, false);

    return pow(0.8f, iNearByPartnerNum+iAngleNearByPartnerNum*0.5f);
}

//������������ɵĵ�λλ��
int CCombatSkill::ProcessSkillMove(CBattlefieldObj& stBattlefieldObj, CCombatUnitObj& stCastUnitObj, 
                                   CCombatUnitObj& stTargetUnitObj, ActionTarget& stNotify, const SFightUnitSkillConfig& stSkillConfig)
{
    int iRet = T_SERVER_SUCESS;

    //�ȴ���������
    if(stSkillConfig.iSelfMoveID != 0)
    {
        iRet = CCombatUtility::ProcessUnitMove(stBattlefieldObj, stCastUnitObj, *stNotify.mutable_stcastpos(), stSkillConfig.iSelfMoveID, stCastUnitObj.GetUnitDirection());
        if(iRet)
        {
            LOGERROR("Failed to process one skill move, skill id %d, ret %d, uin %u\n", stSkillConfig.iConfigID, iRet, m_uiActiveUin);
            return iRet;
        }
    }

    //�ٴ����ܻ�����
    if(stSkillConfig.iTargetMoveID != 0)
    {
        iRet = CCombatUtility::ProcessUnitMove(stBattlefieldObj, stTargetUnitObj, *stNotify.mutable_sttargetpos(), stSkillConfig.iTargetMoveID, stCastUnitObj.GetUnitDirection());
        if(iRet)
        {
            LOGERROR("Failed to process one skill move, skill id %d, ret %d, uin %u\n", stSkillConfig.iConfigID, iRet, m_uiActiveUin);
            return iRet;
        }
    }

    return T_SERVER_SUCESS;
}

//�����ܵ�����Ч��
int CCombatSkill::ProcessSkillSpecailFunc(CCombatUnitObj& stCastUnitObj, CCombatUnitObj& stTargetUnitObj, ActionTarget& stNotify, const SFightUnitSkillConfig& stSkillConfig)
{
    if(stCastUnitObj.GetUnitSize()!=0 || stTargetUnitObj.GetUnitSize()!=0)
    {
        //�������λ��֧��λ��
        return 0;
    }

    stNotify.set_ispecialfunctype(stSkillConfig.iSpecialFuncType);

    switch(stSkillConfig.iSpecialFuncType)
    {
    case SKILL_SPECIAL_FUNC_EXCHANGEPOS:
        {
            //���л�λ����Ŀ��λ�û���
            TUNITPOSITION stCastOldPos = stCastUnitObj.GetUnitPosition();
            stCastUnitObj.SetUnitPosition(stTargetUnitObj.GetUnitPosition());
            stTargetUnitObj.SetUnitPosition(stCastOldPos);

            stNotify.mutable_stcastpos()->set_iposx(stCastUnitObj.GetUnitPosition().iPosX);
            stNotify.mutable_stcastpos()->set_iposy(stCastUnitObj.GetUnitPosition().iPosY);
            stNotify.mutable_sttargetpos()->set_iposx(stTargetUnitObj.GetUnitPosition().iPosX);
            stNotify.mutable_sttargetpos()->set_iposy(stTargetUnitObj.GetUnitPosition().iPosY);
        }
        break;

    default:
        {
            return T_ZONE_SYSTEM_INVALID_CFG;
        }
        break;
    }

    return 0;
}

