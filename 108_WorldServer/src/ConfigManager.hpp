#ifndef __CONIFG_MANAGER_HPP__
#define __CONIFG_MANAGER_HPP__

#include "TemplateConfigManager.hpp"

typedef struct
{
    char m_szHostIP[24];
    unsigned short m_ushPort;
    unsigned short m_ushZoneID;
    char m_szZoneName[64];
} TZoneConf;

typedef struct
{
    unsigned short m_ushLen;

    // ������ͨ�͵���2��IP��Ϣ
    TZoneConf m_astZoneConf[MAX_ZONE_PER_WORLD*2];
} TZoneConfList;

//��ɫ�����������
extern const char GAME_ROLE_BIRTH_CONFIG_FILE[];
typedef CTemplateConfigManager<SRoleBirthConfig, MAX_ROLE_BIRTH_NUM, GAME_ROLE_BIRTH_CONFIG_FILE> CSRoleBirthConfigManager;

//ս����λ���ù�����
extern const char GAME_FIGHT_UNIT_CONFIG_FILE[];
typedef CTemplateConfigManager<SFightUnitConfig, MAX_FIGHT_UNIT_NUM, GAME_FIGHT_UNIT_CONFIG_FILE> CSFightUnitConfigManager;

class CConfigManager
{
public:
    int Initialize(bool bResume);

    //��ɫ����������
    CSRoleBirthConfigManager& GetBirthConfigManager() { return m_stBirthConfigManager; };

    //���ս����λ������
    CSFightUnitConfigManager& GetFightUnitConfigManager() { return m_stFightUnitConfigManager; };

    int LoadZoneConf(const char* pszConfFile);
    const TZoneConfList& GetZoneConfList()
    {
        return m_stZoneConfList;
    }

private:
    TZoneConfList m_stZoneConfList;

    //��ɫ����������
    CSRoleBirthConfigManager m_stBirthConfigManager;

    //���ս����λ������
    CSFightUnitConfigManager m_stFightUnitConfigManager;
};

#endif
