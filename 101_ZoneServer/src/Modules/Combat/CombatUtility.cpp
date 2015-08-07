
#include "GameRole.hpp"
#include "BattlefieldObj.hpp"
#include "ZoneObjectHelper.hpp"
#include "CombatUnitObj.hpp"
#include "ModuleShortcut.hpp"
#include "UnitUtility.hpp"

#include "CombatUtility.hpp"

CBattlefieldObj* CCombatUtility::GetBattlefiledObj(CGameRoleObj& stRoleObj)
{
    return GameType<CBattlefieldObj>::Get(stRoleObj.GetBattlefieObjID());
}

CBattlefieldObj* CCombatUtility::GetBattlefiledObj(int iBattlefieldIndex)
{
    return GameType<CBattlefieldObj>::Get(iBattlefieldIndex);
}

//����ս����λID��ȡCCombatUnitObj
CCombatUnitObj* CCombatUtility::GetCombatUnitObj(int iCombatUnitID)
{
    return GameType<CCombatUnitObj>::Get(iCombatUnitID);
}

//����ս��
void CCombatUtility::ClearBattlefield(int iBattlefieldIndex)
{
    if(iBattlefieldIndex < 0)
    {
        return;
    }

    //��ȡս����Ϣ
    CBattlefieldObj* pstBattlefieldObj = CCombatUtility::GetBattlefiledObj(iBattlefieldIndex);
    if(!pstBattlefieldObj)
    {
        LOGERROR("Failed to get battlefield obj, index %d\n", iBattlefieldIndex);
        return;
    }

    //����������ϵ�ս��ID
    CGameRoleObj* pstActiveRoleObj = CUnitUtility::GetRoleByUin(pstBattlefieldObj->GetActiveUin());
    if(pstActiveRoleObj)
    {
        pstActiveRoleObj->SetBattlefieObjID(-1);
    }

    CGameRoleObj* pstPassiveRoleObj = CUnitUtility::GetRoleByUin(pstBattlefieldObj->GetPassiveUin());
    if(pstPassiveRoleObj)
    {
        pstPassiveRoleObj->SetBattlefieObjID(-1);
    }

    //����ս��
    pstBattlefieldObj->ClearBattlefield();

    //ɾ��ս��
    GameType<CBattlefieldObj>::Del(iBattlefieldIndex);

    return;
}

//������ʽ����
int CCombatUtility::GetSkillHit(CCombatUnitObj& stCastUnitObj, const SFightUnitSkillConfig& stSkillConfig, int iDistance)
{
    //��ȡ���ܵ�����Score��
    const SSkillScoreConfig* pstScoreConfig = SkillScoreCfgMgr().GetConfig(stSkillConfig.iHitModifierID);
    if(!pstScoreConfig)
    {
        LOGERROR("Failed to get skill score config, invalid hit modifier id %d\n", stSkillConfig.iHitModifierID);
        return 0;
    }

    //��ȡ���ܵ�����Range��
    const SSkillRangeConfig* pstRangeConfig = SkillRangeCfgMgr().GetConfig(stSkillConfig.iTargetRangeID);
    if(!pstRangeConfig)
    {
        LOGERROR("Failed to get skill range config, invalid range id %d\n", stSkillConfig.iTargetRangeID);
        return 0;
    }

    //�����������
    int iBaseHit = CCombatUtility::GetSkillScoreNum(stCastUnitObj, *pstScoreConfig);

    //���㹥�����ط��ľ���
    int iRangeIndex = iDistance;
    if(iRangeIndex >= MAX_SKILL_RANGE_INFO_NUM)
    {
        //����MAX_SKILL_RANGE_INFO_NUM���⺬�壬������MAX_SKILL_RANGE_INFO_NUM-1
        iRangeIndex = MAX_SKILL_RANGE_INFO_NUM-1;
    }

    //����ת�����±���Ҫ��1
    iRangeIndex = iRangeIndex - 1;

    //������ʽ����

    return iBaseHit*pstRangeConfig->aiRangeInfo[iRangeIndex]/10000;
}

//������ʽ����
int CCombatUtility::GetSkillDodge(CCombatUnitObj& stCastUnitObj, CCombatUnitObj& stTargetUnitObj, const SFightUnitSkillConfig& stSkillConfig)
{
    //��ȡ���ܵ�����Score��
    const SSkillScoreConfig* pstScoreConfig = SkillScoreCfgMgr().GetConfig(stSkillConfig.iDodgeModifierID);
    if(!pstScoreConfig)
    {
        LOGERROR("Failed to get skill score config, invalid hit modifier id %d\n", stSkillConfig.iDodgeModifierID);
        return 0;
    }

    //�����������
    int iBaseDodge = CCombatUtility::GetSkillScoreNum(stTargetUnitObj, *pstScoreConfig);

    return (iBaseDodge);
}

//���㹥��ǿ��
int CCombatUtility::GetAttackStrength(CCombatUnitObj& stCastUnitObj, const SFightUnitSkillConfig& stSkillConfig)
{
    //�˺����ñ�
    const SSkillDamageConfig* pstDamageConfig = SkillDamageCfgMgr().GetConfig(stSkillConfig.iDamageID);
    if(!pstDamageConfig)
    {
        LOGERROR("Failed to get skill damage config, invalid damage id %d\n", stSkillConfig.iDamageID);

        return 0;
    }

    //Score��
    const SSkillScoreConfig* pstScoreConfig = SkillScoreCfgMgr().GetConfig(pstDamageConfig->iAttack);
    if(!pstScoreConfig)
    {
        LOGERROR("Failed to get skill score config, invalid score id %d\n", pstDamageConfig->iAttack);

        return 0;
    }
    
    //����ǿ��

    return CCombatUtility::GetSkillScoreNum(stCastUnitObj, *pstScoreConfig);
}

//�������ǿ��
int CCombatUtility::GetDefenceStrength(CCombatUnitObj& stCastUnitObj, CCombatUnitObj& stTargetUnitObj, const SFightUnitSkillConfig& stSkillConfig)
{
    //�˺����ñ�
    const SSkillDamageConfig* pstDamageConfig = SkillDamageCfgMgr().GetConfig(stSkillConfig.iDamageID);
    if(!pstDamageConfig)
    {
        LOGERROR("Failed to get skill damage config, invalid damage id %d\n", stSkillConfig.iDamageID);

        return 0;
    }

    //Score��
    const SSkillScoreConfig* pstScoreConfig = SkillScoreCfgMgr().GetConfig(pstDamageConfig->iDefence);
    if(!pstScoreConfig)
    {
        LOGERROR("Failed to get skill score config, invalid score id %d\n", pstDamageConfig->iDefence);

        return 0;
    }

    //����ǿ��
    int iDefenceStrength = CCombatUtility::GetSkillScoreNum(stTargetUnitObj, *pstScoreConfig);

    return (iDefenceStrength);
}

//��ȡ�жԵ�λ�Ĺ������룬Ϊ0��ʾ���ܹ���
int CCombatUtility::GetAttackDistance( const TUNITPOSITION& stCastPos, const TUNITPOSITION& stTargetPos, int iTargetRangeID)
{
    //�ȼ��㹥�ط���X,Y����
    int iDeltaX = stTargetPos.iPosX - stCastPos.iPosX;
    int iDeltaY = stTargetPos.iPosY - stCastPos.iPosY;

    //��ȡRange����
    const SSkillRangeConfig* pstRangeConfig = SkillRangeCfgMgr().GetConfig(iTargetRangeID);
    if(!pstRangeConfig)
    {
        LOGERROR("Failed to get target range config, invalid target range id %d\n", iTargetRangeID);
        return 0;
    }

    //����Range����
    for(int i=0; i<pstRangeConfig->iRangeNum; ++i)
    {
        if(pstRangeConfig->aiRangeInfo[i] == 0)
        {
            //��λ�õ�Ŀ��򲻵�
            continue;
        }

        int iAreaID = i+1+pstRangeConfig->iRangeAreaParam;

        //��ȡArea������
        const SSkillAreaConfig* pstAreaConfig = SkillAreaCfgMgr().GetConfig(iAreaID);
        if(!pstAreaConfig)
        {
            LOGERROR("Failed to get skill area config, invalid area id %d\n", iAreaID);
            return 0;
        }

        //����Area�е�����λ��
        for(int j=0; j<pstAreaConfig->iAreaPosNum; ++j)
        {
            if(pstAreaConfig->astTargetPosInfo[j].iPosX==iDeltaX
               && pstAreaConfig->astTargetPosInfo[j].iPosY==iDeltaY)
            {
                return (i+1);
            }
        }
    }

    return 0;
}

//����Score���������ֵ
int CCombatUtility::GetSkillScoreNum(CCombatUnitObj& stCombatUnitObj, const SSkillScoreConfig& stScoreConfig)
{
    //�������Score
    int iBaseScore = stScoreConfig.iScoreBaseNum;
    for(int i=0; i<FIGHT_ATTR_MAX; ++i)
    {
        //�˵Ĳ����ǰٷֱȣ�������Ҫ��100
        iBaseScore += stScoreConfig.aiAttribute[i] * stCombatUnitObj.GetFightAttr(i);
    }

    //iScoreParam�ǰٷֱȣ���Ҫ��100
    return iBaseScore*stScoreConfig.iScoreParam/(100*100);
}

//����ս����λ�ķ���
void CCombatUtility::UpdateUnitDirection(CCombatUnitObj& stCastUnitObj, const TUNITPOSITION& stTargetPos)
{
    if(stCastUnitObj.GetUnitPosition().iPosX==stTargetPos.iPosX
       && stCastUnitObj.GetUnitPosition().iPosY==stTargetPos.iPosY)
    {
        //���Ŀ����������ת��
        return;
    }

    int iNewDirection = GetAttackDirection(stCastUnitObj, stTargetPos);
    if(iNewDirection > 0)
    {
        stCastUnitObj.SetUnitDirection(iNewDirection);
    }

    return;
}

//��ȡս����λ�����ķ���, <0 ��ʾ����
int CCombatUtility::GetAttackDirection(CCombatUnitObj& stCastUnitObj, const TUNITPOSITION& stTargetPos)
{
    const TUNITPOSITION& stCastPos = stCastUnitObj.GetUnitPosition();

    int iDeltaX = stTargetPos.iPosX - stCastPos.iPosX;
    int iDeltaY = stTargetPos.iPosY - stCastPos.iPosY;

    if(iDeltaX==0 && iDeltaY==0)
    {
        return -1;
    }
    else if(iDeltaY>0 && ABS(iDeltaY,0)>=ABS(iDeltaX,0))
    {
        return COMBAT_DIRECTION_WEST;
    }
    else if(iDeltaY<0 && ABS(iDeltaY,0)>=ABS(iDeltaX,0))
    {
        return COMBAT_DIRECTION_EAST;
    }
    else if(iDeltaX>0 && ABS(iDeltaX,0)>ABS(iDeltaY,0))
    {
        return COMBAT_DIRECTION_NORTH;
    }
    else if(iDeltaX<0 && ABS(iDeltaX,0)>ABS(iDeltaY,0))
    {
        return COMBAT_DIRECTION_SOUTH;
    }

    return -1;
}

//���������˺���Χ������
void CCombatUtility::FixSkillAreaPos(int iCastDirection, const SFightPosInfo& stTargetPos, TUNITPOSITION& stFixedPos)
{
    switch(iCastDirection)
    {
        case COMBAT_DIRECTION_EAST:
            {
                stFixedPos.iPosX = stTargetPos.iPosY;
                stFixedPos.iPosY = -stTargetPos.iPosX;
            }
            break;

        case COMBAT_DIRECTION_NORTH:
            {
                stFixedPos.iPosX = stTargetPos.iPosX;
                stFixedPos.iPosY = stTargetPos.iPosY;
            }
            break;

        case COMBAT_DIRECTION_WEST:
            {
                stFixedPos.iPosX = stTargetPos.iPosY;
                stFixedPos.iPosY = stTargetPos.iPosX;
            }
            break;

        case COMBAT_DIRECTION_SOUTH:
            {
                stFixedPos.iPosX = -stTargetPos.iPosX;
                stFixedPos.iPosY = stTargetPos.iPosY;
            }
            break;

        default:
            {
                stFixedPos.iPosX = 0;
                stFixedPos.iPosY = 0;
            }
            break;
    }

    return;
}

//����ս����λ������λ��
int CCombatUtility::ProcessUnitMove(CBattlefieldObj& stBattlefieldObj, CCombatUnitObj& stUnitObj, UnitPosition& stPosNotify, int iAreaID, int iDirection, bool bStop)
{
    if(stUnitObj.GetUnitSize() != 0)
    {
        //�������λ��֧������λ��
        return 0;
    }

    const SSkillAreaConfig* pstAreaConfig = SkillAreaCfgMgr().GetConfig(iAreaID);
    if(!pstAreaConfig)
    {
        LOGERROR("Failed to get skill area config, invalid id %d\n", iAreaID);
        return -1;
    }
    
    bool bCanMove = false;
    TUNITPOSITION stTmpPos;
    TUNITPOSITION stCanMovePos;
    for(int i=0; i<pstAreaConfig->iAreaPosNum; ++i)
    {
        //���ݷ����������õ�����
        CCombatUtility::FixSkillAreaPos(iDirection,pstAreaConfig->astTargetPosInfo[i], stTmpPos);

        stTmpPos.iPosX += stUnitObj.GetUnitPosition().iPosX;
        stTmpPos.iPosY += stUnitObj.GetUnitPosition().iPosY;

        //��λ�ò������߲�����Ҫͣ����
        if(!stBattlefieldObj.IsPosCanWalk(stTmpPos,-1) && bStop)
        {
            //��������λ��
            break;
        }

        if(stBattlefieldObj.IsPosCanWalk(stTmpPos,-1))
        {
            stCanMovePos.iPosX = stTmpPos.iPosX;
            stCanMovePos.iPosY = stTmpPos.iPosY;

            bCanMove = true;
        }
    }

    if(bCanMove)
    {
        stUnitObj.SetUnitPosition(stCanMovePos);

        stPosNotify.set_iposx(stCanMovePos.iPosX);
        stPosNotify.set_iposy(stCanMovePos.iPosY);
    }
    else
    {
        //�ƶ�����ǰλ��
        stPosNotify.set_iposx(stUnitObj.GetUnitPosition().iPosX);
        stPosNotify.set_iposy(stUnitObj.GetUnitPosition().iPosY);
    }

    //��ǰλ������������赲����ɱ����λ
    if(stBattlefieldObj.IsPosCanWalk(stUnitObj.GetUnitPosition(), stUnitObj.GetCombatUnitID()) 
       && !stBattlefieldObj.IsPosCanWalk(stUnitObj.GetUnitPosition(), stUnitObj.GetCombatUnitID()))
    {
        //�����赲λ��,ɱ����λ
        stBattlefieldObj.KillCombatUnit(stUnitObj.GetCombatUnitID(), false);
    }

    return T_SERVER_SUCESS;
}

//��ҵ�ǰ�Ƿ���Խ���PVPս��
bool CCombatUtility::CanDoPVPCombat(unsigned int uin)
{
    CGameRoleObj* pstRoleObj = CUnitUtility::GetRoleByUin(uin);
    if(!pstRoleObj)
    {
        return false;
    }

    //��ҵ�ǰ�Ƿ���ս����
    if(pstRoleObj->GetBattlefieObjID() >= 0)
    {
        return false;
    }

    return true;
}

//todo jasonxiong5 ������Ҫ�޸�Ϊ��Ʒ
//����������Ч״̬��������Ч����bIsAram��ʾ�Ƿ���Ч����
void CCombatUtility::ProcessWeaponEffect(CCombatUnitObj& stUnitObj, bool bIsAram)
{
    CGameRoleObj* pstRoleObj = CUnitUtility::GetRoleByUin(stUnitObj.GetUin());
    if(!pstRoleObj)
    {
        return;
    }

    //��ȡս����λ��Ϣ
    CFightUnitObj* pstFightUnitObj = pstRoleObj->GetFightUnitManager().GetFightUnitByID(stUnitObj.GetConfigID());
    if(!pstFightUnitObj)
    {
        return;
    }

    /*
    //��ȡ������Ϣ
    const SEquipmentConfig* pstEquipConfig = EquipmentCfgMgr().GetConfig(pstFightUnitObj->GetItemInfo(0).iItemID);
    if(!pstEquipConfig)
    {
        return;
    }

    //��ȡս��
    CBattlefieldObj* pstBattlefiledObj = CCombatUtility::GetBattlefiledObj(*pstRoleObj);
    if(!pstBattlefiledObj)
    {
        return;
    }

    //��������Ч��
    if(bIsAram)
    {
        for(int i=0; i<FIGHT_ATTR_MAX; ++i)
        {
            pstBattlefiledObj->AddCombatUnitAttr(stUnitObj, i, pstEquipConfig->aiAttribute[i]);
        }
    }
    else
    {
        for(int i=0; i<FIGHT_ATTR_MAX; ++i)
        {
            pstBattlefiledObj->AddCombatUnitAttr(stUnitObj, i, -pstEquipConfig->aiAttribute[i]);
        }
    }
    */

    return;
}

//��ȡ���귶Χ��ĳ��Ӫ�ĵ�λ
int CCombatUtility::GetAreaTargetUnits(unsigned uin, const TUNITPOSITION& stPos, int iAreaID, int iCamp, int iDirection, std::vector<int>& vUnits)
{
    vUnits.clear();

    CGameRoleObj* pstRoleObj = CUnitUtility::GetRoleByUin(uin);
    if(!pstRoleObj)
    {
        LOGERROR("Failed to get game role obj, uin %u\n", uin);
        return -1;
    }

    CBattlefieldObj* pstBattlefieldObj = GetBattlefiledObj(*pstRoleObj);
    if(!pstBattlefieldObj)
    {
        LOGERROR("Failed to get battlefield obj, uin %u\n", uin);
        return -2;
    }

    //��ȡArea��
    const SSkillAreaConfig* pstAreaConfig = SkillAreaCfgMgr().GetConfig(iAreaID);
    if(!pstAreaConfig)
    {
        LOGERROR("Failed to get skill area config, invalid area id %d\n", iAreaID);
        return -3;
    }

    //����Area�е�����λ��
    TUNITPOSITION stTmpPos;
    for(int j=0; j<pstAreaConfig->iAreaPosNum; ++j)
    {
        //���ݷ����������õ�����
        CCombatUtility::FixSkillAreaPos(iDirection, pstAreaConfig->astTargetPosInfo[j], stTmpPos);
        stTmpPos.iPosX += stPos.iPosX;
        stTmpPos.iPosY += stPos.iPosY;

        //��ȡս����λ
        CCombatUnitObj* pstUnitObj = pstBattlefieldObj->GetCombatUnitByPos(stTmpPos);
        if(pstUnitObj && pstBattlefieldObj->GetCombatUnitCamp(pstUnitObj->GetCombatUnitID())==iCamp)
        {
            vUnits.push_back(pstUnitObj->GetCombatUnitID());
        }
    }

    return 0;
}

