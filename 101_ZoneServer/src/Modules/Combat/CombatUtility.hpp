
#ifndef __COMBAT_UTILITY_HPP__
#define __COMBAT_UTILITY_HPP__

#include "GameConfigDefine.hpp"

using namespace GameConfig;

//ս�����ߺ�����
class CGameRoleObj;
class CBattlefieldObj;
class CCombatUnitObj;
class TUNITPOSITION;
class CCombatUtility
{
public:
    
    static CBattlefieldObj* GetBattlefiledObj(CGameRoleObj& stRoleObj);

    static CBattlefieldObj* GetBattlefiledObj(int iBattlefieldIndex);

    //����ս����λID��ȡCCombatUnitObj
    static CCombatUnitObj* GetCombatUnitObj(int iCombatUnitID); 

    //����ս��
    static void ClearBattlefield(int iBattlefieldIndex);

    //������ʽ����
    static int GetSkillHit(CCombatUnitObj& stCastUnitObj, const SFightUnitSkillConfig& stSkillConfig, int iDistance);

    //������ʽ����
    static int GetSkillDodge(CCombatUnitObj& stCastUnitObj, CCombatUnitObj& stTargetUnitObj, const SFightUnitSkillConfig& stSkillConfig);

    //���㹥��ǿ��
    static int GetAttackStrength(CCombatUnitObj& stCastUnitObj, const SFightUnitSkillConfig& stSkillConfig);

    //�������ǿ��
    static int GetDefenceStrength(CCombatUnitObj& stCastUnitObj, CCombatUnitObj& stTargetUnitObj, const SFightUnitSkillConfig& stSkillConfig);

    //��ȡ�жԵ�λ�Ĺ������룬Ϊ0��ʾ���ܹ���
    static int GetAttackDistance(const TUNITPOSITION& stCastPos, const TUNITPOSITION& stTargetPos, int iTargetRangeID);

    //����Score���������ֵ
    static int GetSkillScoreNum(CCombatUnitObj& stCombatUnitObj, const SSkillScoreConfig& stScoreConfig);

    //����ս����λ�ķ���
    static void UpdateUnitDirection(CCombatUnitObj& stCastUnitObj, const TUNITPOSITION& stTargetPos);

    //��ȡս����λ�����ķ���, <0 ��ʾ����
    static int GetAttackDirection(CCombatUnitObj& stCastUnitObj, const TUNITPOSITION& stTargetPos);

    //���������˺���Χ������
    static void FixSkillAreaPos(int iCastDirection, const SFightPosInfo& stTargetPos, TUNITPOSITION& stFixedPos);

    //����ս����λ������λ��,bStop��ʾ�Ƿ���;���赲ͣ��
    static int ProcessUnitMove(CBattlefieldObj& stBattlefieldObj, CCombatUnitObj& stUnitObj, UnitPosition& stPosNotify, int iAreaID, int iDirection, bool bStop = true);

    //��ҵ�ǰ�Ƿ���Խ���PVPս��
    static bool CanDoPVPCombat(unsigned int uin);

    //����������Ч״̬��������Ч����bIsAram��ʾ�Ƿ���Ч����
    static void ProcessWeaponEffect(CCombatUnitObj& stUnitObj, bool bIsAram);

    //��ȡ���귶Χ��ĳ��Ӫ�ĵ�λ
	static int GetAreaTargetUnits(unsigned uin, const TUNITPOSITION& stPos, int iAreaID, int iCamp, int iDirection, std::vector<int>& vUnits);
};

#endif
