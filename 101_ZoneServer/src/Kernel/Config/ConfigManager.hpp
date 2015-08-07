#ifndef __CONIFG_MANAGER_HPP__
#define __CONIFG_MANAGER_HPP__

#include "HandlerFactory.hpp"
#include "LogAdapter.hpp"
#include "TemplateConfigManager.hpp"
#include "QMGMPrivConfigManager.hpp"
#include "BattlefieldPathConfigManager.hpp"

using namespace ServerLib;

// ��Ϣ���CDʱ�� 2000ms
const int MAX_MSG_INTERVAL_TIME = 2000;

// Ĭ�ϲ�����CD
const int DEFAULT_MSG_INTERVAL = 0;

//ս����λ���ù�����
extern const char GAME_FIGHT_UNIT_CONFIG_FILE[];
typedef CTemplateConfigManager<SFightUnitConfig, MAX_FIGHT_UNIT_NUM, GAME_FIGHT_UNIT_CONFIG_FILE> CSFightUnitConfigManager;

//ս���������ù�����
extern const char GAME_FIGHT_MONSTER_CONFIG_FILE[];
typedef CTemplateConfigManager<SMonsterConfig, MAX_MONSTER_CONFIG_NUM, GAME_FIGHT_MONSTER_CONFIG_FILE> CSMonsterConfigManager;

//ս���ؿ����ù�����
extern const char GAME_BATTLE_CROSS_CONFIG_FILE[];
typedef CTemplateConfigManager<SBattleCrossConfig, MAX_BATTLE_CROSS_NUM, GAME_BATTLE_CROSS_CONFIG_FILE> CSBattleCrossConfigManager;

//�ؿ����������ù�����
extern const char GAME_CROSS_MONSTER_CONFIG_FILE[];
typedef CTemplateConfigManager<SCrossMonsterConfig, MAX_CROSS_MONSTER_CONFIG_NUM, GAME_CROSS_MONSTER_CONFIG_FILE> CSCrossMonsterConfigManager;

//ս���������ù���
extern const char GAME_FIGHT_SKILL_CONFIG_FILE[];
typedef CTemplateConfigManager<SFightUnitSkillConfig, MAX_FIGHT_SKILL_CONFIG_NUM, GAME_FIGHT_SKILL_CONFIG_FILE> CFightSkillConfigManager;

//�����ͷž������ù���
extern const char GAME_SKILL_RANGE_CONFIG_FILE[];
typedef CTemplateConfigManager<SSkillRangeConfig, MAX_SKILL_RANGE_CONFIG_NUM, GAME_SKILL_RANGE_CONFIG_FILE> CSkillRangeConfigManager;

//�����ͷŷ�Χ���ù���
extern const char GAME_SKILL_AREA_CONFIG_FILE[];
typedef CTemplateConfigManager<SSkillAreaConfig, MAX_SKILL_AREA_CONFIG_NUM, GAME_SKILL_AREA_CONFIG_FILE> CSkillAreaConfigManager;

//����Ч����������
extern const char GAME_SKILL_SCORE_CONFIG_FILE[];
typedef CTemplateConfigManager<SSkillScoreConfig, MAX_SKILL_SCORE_CONFIG_NUM, GAME_SKILL_SCORE_CONFIG_FILE> CSkillScoreConfigManager;

//�����˺�Ч������
extern const char GAME_SKILL_DAMAGE_CONFIG_FILE[];
typedef CTemplateConfigManager<SSkillDamageConfig, MAX_SKILL_DAMAGE_CONFIG_NUM, GAME_SKILL_DAMAGE_CONFIG_FILE> CSkillDamageConfigManager;

//����Buff������
extern const char GAME_FIGHT_BUFF_CONFIG_FILE[];
typedef CTemplateConfigManager<SFightBuffConfig, MAX_FIGHT_BUFF_CONFIG_NUM, GAME_FIGHT_BUFF_CONFIG_FILE> CSFightBuffConfigManager;

//������������
extern const char GAME_DROP_REWARD_CONFIG_FILE[];
typedef CTemplateConfigManager<SDropRewardConfig, MAX_DROP_REWARD_CONFIG_NUM, GAME_DROP_REWARD_CONFIG_FILE> CSDropRewardConfigManager;

//ս����λAI����
extern const char GAME_FIGHTUNIT_AI_CONFIG_FILE[];
typedef CTemplateConfigManager<SFightUnitAIConfig, MAX_FIGHTUNIT_AI_CONFIG_NUM, GAME_FIGHTUNIT_AI_CONFIG_FILE> CSFightUnitAIConfigManager;

//ս�����ܻ��ϻ�������
extern const char GAME_JOINT_ATTACK_CONFIG_FILE[];
typedef CTemplateConfigManager<SJointAttackConfig, MAX_JOINT_ATTACK_CONFIG_NUM, GAME_JOINT_ATTACK_CONFIG_FILE> CSJointAtkConfigManager;

//ս��ս�����ص�����
extern const char GAME_COMBAT_TRAP_CONFIG_FILE[];
typedef CTemplateConfigManager<STrapConfig, MAX_COMBAT_TRAP_CONFIG_NUM, GAME_COMBAT_TRAP_CONFIG_FILE> CSTrapConfigManager;

//��Ϸ�����Ʒ������
extern const char GAME_USER_ITEM_CONFIG_FILE[];
typedef CTemplateConfigManager<SGameItemConfig, MAX_GAME_ITEM_CONFIG_NUM, GAME_USER_ITEM_CONFIG_FILE> CSGameItemConfigManager;

class CConfigManager
{
public:
    int Initialize(bool bResumeMode);

    //GMȨ������
    CQMGMPrivConfigManager& GetGMPrivConfigManager() { return m_stQMGMPrivConfigManager; };

    const int GetMsgInterval(const unsigned int uiMsgID)const;
    int LoadMsgInterval();
    int LoadMsgDiableList();

    int GetServerStartTime(int& iServerStartTime);

    //ս����λ���ù�����
    CSFightUnitConfigManager& GetFightUnitConfigManager() { return m_stSFightUnitConfigManager; };

    //ս���ؿ����ù�����
    CSBattleCrossConfigManager& GetBattleCrossConfigManager() { return m_stSBattleCrossConfigManager; };

    //�ؿ��������ù�����
    CSCrossMonsterConfigManager& GetCrossMonsterConfigManager() { return m_stCrossMonsterConfigManager; };

    //ս����λAI���ù�����
    CSFightUnitAIConfigManager& GetFightUnitAIConfigManager() { return m_stSFightUnitAIConfigManager; };

    //ս���������ù�����
    CSMonsterConfigManager& GetMonsterConfigManager() { return m_stSMonsterConfigManager; };

    //ս����ͼ��Ϣ���ù�����
    CBattlefieldPathConfigManager& GetBattlefieldPathConfigManager() { return m_stBattlefieldPathConfigManager; };

    //ս���������ù�����
    CFightSkillConfigManager& GetFightSkillConfigManager() { return m_stFightSkillConfigManager; };

    //�����ͷž��������
    CSkillRangeConfigManager& GetSkillRangeConfigManager() { return m_stSkillRangeConfigManager; };

    //�����ͷŷ�Χ������
    CSkillAreaConfigManager& GetSkillAreaConfigManager() { return m_stSkillAreaConfigManager; };

    //����Ч������������
    CSkillScoreConfigManager& GetSkillScoreConfigManager() { return m_stSkillScoreConfigManager; };

    //�����˺����ù�����
    CSkillDamageConfigManager& GetSkillDamageConfigManager() { return m_stSkillDamageConfigManager; };

    //ս��Buff���ù�����
    CSFightBuffConfigManager& GetFightBuffConfigManager() { return m_stFightBuffConfigManager; };

    //����������ù�����
    CSDropRewardConfigManager& GetDropRewardConfigManager() { return m_stDropRewardConfigManager; };

    //ս�����ܻ��ϻ����ù�����
    CSJointAtkConfigManager& GetJointAtkConfigManager() { return m_stJointAtkConfigManager; };

    //ս��ս���������ù�����
    CSTrapConfigManager& GetTrapConfigManager() { return m_stTrapConfigManager; };

    //��Ϸ��Ʒ���ù�����
    CSGameItemConfigManager& GetGameItemConfigManager() { return m_stGameItemConfigManager; };

private:
    int PrintConfigMemoryStatics();

private:

    //GM����Ȩ�����ù�����
    CQMGMPrivConfigManager m_stQMGMPrivConfigManager;

    //ս����λ���ù�����
    CSFightUnitConfigManager m_stSFightUnitConfigManager;

    //ս���ؿ����ù�����
    CSBattleCrossConfigManager m_stSBattleCrossConfigManager;

    //�ؿ����������ù�����
    CSCrossMonsterConfigManager m_stCrossMonsterConfigManager;

    //ս����λAI���ù�����
    CSFightUnitAIConfigManager  m_stSFightUnitAIConfigManager;

    //ս���������ù�����
    CSMonsterConfigManager  m_stSMonsterConfigManager;

    //ս����ͼ��Ϣ���ù�����
    CBattlefieldPathConfigManager m_stBattlefieldPathConfigManager;

    //ս���������ù�����
    CFightSkillConfigManager m_stFightSkillConfigManager;

    //ս�������ͷž������ù�����
    CSkillRangeConfigManager m_stSkillRangeConfigManager;

    //ս�������ͷŷ�Χ���ù�����
    CSkillAreaConfigManager m_stSkillAreaConfigManager;

    //ս������Ч���������ù�����
    CSkillScoreConfigManager m_stSkillScoreConfigManager;

    //ս�������˺����ù�����
    CSkillDamageConfigManager m_stSkillDamageConfigManager;

    //ս��Buff���ù�����
    CSFightBuffConfigManager m_stFightBuffConfigManager;

    //ս���������ù�����
    CSDropRewardConfigManager m_stDropRewardConfigManager;

    //���ܻ��ϻ����ù�����
    CSJointAtkConfigManager m_stJointAtkConfigManager;

    //ս���ؿ����ù�����
    CSTrapConfigManager m_stTrapConfigManager;

    //��Ϸ��Ʒ���ù�����
    CSGameItemConfigManager m_stGameItemConfigManager;
};

#endif
