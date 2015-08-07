#include "GameProtocol.hpp"
#include "ModuleShortcut.hpp"
#include "ModuleHelper.hpp"

//��ȡս����λ������
CSFightUnitConfigManager& FightUnitCfgMgr()
{
    return CModuleHelper::GetConfigManager()->GetFightUnitConfigManager();
}

//��ȡս���ؿ�����
CSBattleCrossConfigManager& BattleCrossCfgMgr()
{
    return CModuleHelper::GetConfigManager()->GetBattleCrossConfigManager();
}

//��ȡ�ؿ�����������
CSCrossMonsterConfigManager& CrossMonterCfgMgr()
{
    return CModuleHelper::GetConfigManager()->GetCrossMonsterConfigManager();
}

//��ȡս����λAI������
CSFightUnitAIConfigManager& FightUnitAICfgMgr()
{
    return CModuleHelper::GetConfigManager()->GetFightUnitAIConfigManager();
}

//��ȡս�����������
CSMonsterConfigManager& MonsterCfgMgr()
{
    return CModuleHelper::GetConfigManager()->GetMonsterConfigManager();
}

//��ȡս���赲������
CBattlefieldPathConfigManager& BattlefieldPathCfgMgr()
{
    return CModuleHelper::GetConfigManager()->GetBattlefieldPathConfigManager();
}

//��ȡս�����ܵ�����
CFightSkillConfigManager& FightSkillCfgMgr()
{
    return CModuleHelper::GetConfigManager()->GetFightSkillConfigManager();
}

//��ȡ�����ͷž�������
CSkillRangeConfigManager& SkillRangeCfgMgr()
{
    return CModuleHelper::GetConfigManager()->GetSkillRangeConfigManager();
}

//��ȡ�����ͷŷ�Χ����
CSkillAreaConfigManager& SkillAreaCfgMgr()
{
    return CModuleHelper::GetConfigManager()->GetSkillAreaConfigManager();
}

//��ȡ������������
CSkillScoreConfigManager& SkillScoreCfgMgr()
{
    return CModuleHelper::GetConfigManager()->GetSkillScoreConfigManager();
}

//��ȡ�����˺�����
CSkillDamageConfigManager& SkillDamageCfgMgr()
{
    return CModuleHelper::GetConfigManager()->GetSkillDamageConfigManager();
}

//��ȡս��Buff����
CSFightBuffConfigManager& FightBuffCfgMgr()
{
    return CModuleHelper::GetConfigManager()->GetFightBuffConfigManager();
}

//��ȡ������������
CSDropRewardConfigManager& DropRewardCfgMgr()
{
    return CModuleHelper::GetConfigManager()->GetDropRewardConfigManager();
}

//���ܻ��ϻ�������
CSJointAtkConfigManager& JointAtkCfgMgr()
{
    return CModuleHelper::GetConfigManager()->GetJointAtkConfigManager();
}

//ս�����ص�����
CSTrapConfigManager& TrapCfgMgr()
{
    return CModuleHelper::GetConfigManager()->GetTrapConfigManager();
}

//��Ϸ��Ʒ������
CSGameItemConfigManager& ItemCfgMgr()
{
    return CModuleHelper::GetConfigManager()->GetGameItemConfigManager();
}
