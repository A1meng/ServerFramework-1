
#include "GameProtocol.hpp"
#include "SectionConfig.hpp"
#include "ModuleHelper.hpp"
#include "AppDef.hpp"

#include "ConfigManager.hpp"

const char GAME_FIGHT_UNIT_CONFIG_FILE[] = "SFightUnitConfig_Server.bin";
const char GAME_FIGHT_MONSTER_CONFIG_FILE[] = "SMonsterConfig_Server.bin";
const char GAME_BATTLE_CROSS_CONFIG_FILE[] = "SBattleCrossConfig_Server.bin";
const char GAME_CROSS_MONSTER_CONFIG_FILE[] = "SCrossMonsterConfig_Server.bin";
const char GAME_FIGHT_SKILL_CONFIG_FILE[] = "SFightUnitSkillConfig_Server.bin";
const char GAME_SKILL_RANGE_CONFIG_FILE[] = "SSkillRangeConfig_Server.bin";
const char GAME_SKILL_AREA_CONFIG_FILE[] = "SSkillAreaConfig_Server.bin";
const char GAME_SKILL_SCORE_CONFIG_FILE[] = "SSkillScoreConfig_Server.bin";
const char GAME_SKILL_DAMAGE_CONFIG_FILE[] = "SSkillDamageConfig_Server.bin";
const char GAME_FIGHT_BUFF_CONFIG_FILE[] = "SFightBuffConfig_Server.bin";
const char GAME_DROP_REWARD_CONFIG_FILE[] = "SDropRewardConfig_Server.bin";
const char GAME_JOINT_ATTACK_CONFIG_FILE[] = "SJointAttackConfig_Server.bin";
const char GAME_COMBAT_TRAP_CONFIG_FILE[] = "STrapConfig_Server.bin";
const char GAME_FIGHTUNIT_AI_CONFIG_FILE[] = "SFightUnitAIConfig_Server.bin";
const char GAME_USER_ITEM_CONFIG_FILE[] = "SGameItemConfig_Server.bin";

int CConfigManager::Initialize(bool bResumeMode)
{
    int iRet = 0;

    //todo jasonxiong5 �ټ�װ��
    /*
    //��Ϸװ������
    iRet = m_stEquipmentConfigManager.LoadConfig();
    if(iRet)
    {
        return -2;
    } 
    */ 

    //ս����λ���ù�����
    iRet = m_stSFightUnitConfigManager.LoadConfig();
    if(iRet)
    {
        return -4;
    }

    //ս���ؿ�����
    iRet = m_stSBattleCrossConfigManager.LoadConfig();
    if(iRet)
    {
        return -8;
    }

    //ս���ؿ�����
    iRet = m_stCrossMonsterConfigManager.LoadConfig();
    if(iRet)
    {
        return -9;
    }

    //ս����λAI����
    iRet = m_stSFightUnitAIConfigManager.LoadConfig();
    if(iRet)
    {
        return -11;
    }

    //ս�����������
    iRet = m_stSMonsterConfigManager.LoadConfig();
    if(iRet)
    {
        return -12;
    }

    //ս����ͼ��Ϣ������
    iRet = m_stBattlefieldPathConfigManager.LoadAllBattlefield();
    if(iRet)
    {
        return -13;
    }

    //ս�����ܵ�����
    iRet = m_stFightSkillConfigManager.LoadConfig();
    if(iRet)
    {
        return -14;
    }

    //ս�������ͷž�������
    iRet = m_stSkillRangeConfigManager.LoadConfig();
    if(iRet)
    {
        return -15;
    }

    //ս�������ͷŷ�Χ����
    iRet = m_stSkillAreaConfigManager.LoadConfig();
    if(iRet)
    {
        return -16;
    }

    //ս������Ч����������
    iRet = m_stSkillScoreConfigManager.LoadConfig();
    if(iRet)
    {
        return -17;
    }

    //ս�������˺�����
    iRet = m_stSkillDamageConfigManager.LoadConfig();
    if(iRet)
    {
        return -18;
    }

    //ս��Buff����
    iRet = m_stFightBuffConfigManager.LoadConfig();
    if(iRet)
    {
        return -19;
    }

    /*
    //����������ù�����
    iRet = m_stDropRewardConfigManager.LoadConfig();
    if(iRet)
    {
        return -23;
    } 
    */ 

    //���ܻ��ϻ�������
    iRet = m_stJointAtkConfigManager.LoadConfig();
    if(iRet)
    {
        return -38;
    }

    //ս�����ص�����
    iRet = m_stTrapConfigManager.LoadConfig();
    if(iRet)
    {
        return -39;
    }

    //��Ϸ��Ʒ������
    iRet = m_stGameItemConfigManager.LoadConfig();
    if(iRet)
    {
        return -40;
    }

    //GMȨ���������
    iRet = m_stQMGMPrivConfigManager.LoadGMPrivConfig();
    if(iRet)
    {
        return -60;
    }

	//�����ԭ����Ϣ��������Ϣ
	CHandlerFactory::ClearAllDisableMsg();

	//load
	LoadMsgInterval();
	LoadMsgDiableList();

    // ��ӡ�������ڴ�ʹ�����
    PrintConfigMemoryStatics();

    return 0;
};

int CConfigManager::PrintConfigMemoryStatics()
{
    LOGDEBUG("********************** CONFIG MEMORY STATICS *********************\n");

    int iConfigTotalSize = 0;

    //todo jasonxiong1 ��ʱû��ʹ�ã������������ͳһ���
    /*
    // ������
    //û��ְҵ�������˵ĳ�����һ����
    int iBirthSize =  sizeof(QMHeroBirthConfig);
    iConfigTotalSize += iBirthSize;

    LOGDEBUG("BirthConfig: Number = %d, UnitSize = %lu, Total = %d\n",
        1, sizeof(QMHeroBirthConfig), iBirthSize);
    */

    // �ܼ�
    LOGDEBUG("Total Memory: %dB, %dMB\n", iConfigTotalSize, iConfigTotalSize/1024/1024);
    LOGDEBUG("*****************************************************************\n");

    return 0;
}

int CConfigManager::LoadMsgInterval()
{
    CSectionConfig stConfigFile;
    int iRet = stConfigFile.OpenFile(APP_CONFIG_FILE);
    if (iRet != 0)
    {
        return 0;
    }

    char szMsgLimitFile[MAX_FILENAME_LENGTH];
    stConfigFile.GetItemValue("MsgLimit", "MsgLimitFile", szMsgLimitFile, sizeof(szMsgLimitFile), "../conf/MsgLimitFile.conf");

    FILE* fp = fopen(szMsgLimitFile, "r");
    if (fp == NULL)
    {
        TRACESVR("can not open MsgLimitFile:%s for :%s\n", szMsgLimitFile, strerror(errno));
        printf("can not open MsgLimitFile:%s for: %s\n", szMsgLimitFile, strerror(errno));
        return -1;
    }

    char szLine[100];
    unsigned int uiMsgID;
    unsigned int uiMsgLimitInterval;
    while(fgets(szLine, sizeof(szLine), fp))
    {
        //���л��߸�ʽ����
        if (sscanf(szLine, "%u %u", &uiMsgID, &uiMsgLimitInterval) != 2)
        {
            continue;
        }
        if (uiMsgID >= MAX_HANDLER_NUMBER)
        {
            TRACESVR("file :%s msg id is too bigger than MAX_HANDLER_NUMBER", szMsgLimitFile);
            fclose(fp);
            return -20;
        }

        if (uiMsgLimitInterval >= (unsigned int)MAX_MSG_INTERVAL_TIME)
        {
            uiMsgLimitInterval = MAX_MSG_INTERVAL_TIME;
        }

        TZoneMsgConfig* pstMsgConfig = CHandlerFactory::GetMsgConfig(uiMsgID);
        if (!pstMsgConfig)
        {
            continue;
        }

        TRACESVR("msgid:%u  MsgLimitInterval:%u\n", uiMsgID, uiMsgLimitInterval);
        pstMsgConfig->m_iMsgInterval = uiMsgLimitInterval;
    }

    fclose(fp);

    return 0;
}

int CConfigManager::LoadMsgDiableList()
{
    CSectionConfig stConfigFile;
    int iRet = stConfigFile.OpenFile(APP_CONFIG_FILE);
    if (iRet != 0)
    {
        return 0;
    }

    char szMsgDisableFile[MAX_FILENAME_LENGTH];
    stConfigFile.GetItemValue("MsgDisable", "MsgDisableFile", szMsgDisableFile, sizeof(szMsgDisableFile), "../conf/MsgDisableFile.conf");

    //û�о�����
    FILE* fp = fopen(szMsgDisableFile, "r");
    if (fp == NULL)
    {
        TRACESVR("can not open MsgLimitFile:%s for :%s\n", szMsgDisableFile, strerror(errno));
        return 0;
    }

    char szLine[100];
    unsigned int uiMsgID;
    while(fgets(szLine, sizeof(szLine), fp))
    {
        //���л��߸�ʽ����
        if (sscanf(szLine, "%u", &uiMsgID) != 1)
        {
            continue;
        }

        CHandlerFactory::DisabledMsg(uiMsgID);
        TRACESVR("msgid:%u is disabled", uiMsgID);
    }

    fclose(fp);

    return 0;
}

const int CConfigManager::GetMsgInterval(const unsigned int uiMsgID)const
{
    if(uiMsgID >= MAX_HANDLER_NUMBER)
    {
        TRACESVR("NULL handler, msg id: %d\n", uiMsgID);
        return 0;
    }

    TZoneMsgConfig* pstMsgConfig = CHandlerFactory::GetMsgConfig(uiMsgID);
    if (!pstMsgConfig)
    {
        return 0;
    }

    return pstMsgConfig->m_iMsgInterval;
}
