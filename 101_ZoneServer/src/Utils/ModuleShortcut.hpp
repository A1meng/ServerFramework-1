#ifndef __EASY_USE_HPP
#define __EASY_USE_HPP

#include "ConfigManager.hpp"
#include "LogAdapter.hpp"

using namespace ServerLib;

//��Щ��������ֵ��������û���ָ�� ������const���ã�ָ��
//������кܴ���������

//��ȡս����λ������
CSFightUnitConfigManager& FightUnitCfgMgr();

//��ȡս���ؿ�����
CSBattleCrossConfigManager& BattleCrossCfgMgr();

//��ȡ�ؿ�����������
CSCrossMonsterConfigManager& CrossMonterCfgMgr();

//��ȡս����λAI������
CSFightUnitAIConfigManager& FightUnitAICfgMgr();

//��ȡս�����������
CSMonsterConfigManager& MonsterCfgMgr();

//��ȡս���赲������
CBattlefieldPathConfigManager& BattlefieldPathCfgMgr();

//��ȡս�����ܵ�����
CFightSkillConfigManager& FightSkillCfgMgr();

//��ȡ�����ͷž�������
CSkillRangeConfigManager& SkillRangeCfgMgr();

//��ȡ�����ͷŷ�Χ����
CSkillAreaConfigManager& SkillAreaCfgMgr();

//��ȡ������������
CSkillScoreConfigManager& SkillScoreCfgMgr();

//��ȡ�����˺�����
CSkillDamageConfigManager& SkillDamageCfgMgr();

//��ȡս��Buff����
CSFightBuffConfigManager& FightBuffCfgMgr();

//��ȡ������������
CSDropRewardConfigManager& DropRewardCfgMgr();

//���ܻ��ϻ�������
CSJointAtkConfigManager& JointAtkCfgMgr();

//ս�����ص�����
CSTrapConfigManager& TrapCfgMgr();

//��Ϸ��Ʒ������
CSGameItemConfigManager& ItemCfgMgr();

#endif
