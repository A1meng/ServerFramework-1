#include <fstream>
#include <sstream>

#include "GameProtocol.hpp"
#include "ModuleHelper.hpp"

#include "ConfigManager.hpp"

using namespace std;

const char GAME_ROLE_BIRTH_CONFIG_FILE[] = "SRoleBirthConfig_Server.bin";
const char GAME_FIGHT_UNIT_CONFIG_FILE[] = "SFightUnitConfig_Server.bin";

int CConfigManager::Initialize(bool bResume)
{
    int iRet = 0;

    //���ؽ�ɫ����������
    iRet = m_stBirthConfigManager.LoadConfig();
    if(iRet)
    {
        TRACESVR("Fail to load game role birth config!\n");
        return -1;
    }

    //�������ս����λ������
    iRet = m_stFightUnitConfigManager.LoadConfig();
    if(iRet)
    {
        TRACESVR("Failed to load game fight unit config!\n");
        return -2;
    }

    //����������������
    iRet = LoadZoneConf("../conf/zone_conf.txt");
    if (iRet != 0)
    {
        TRACESVR("failed to load zone config\n");
        return -60;
    }

    return 0;
}

int CConfigManager::LoadZoneConf(const char* pszConfFile)
{
    ASSERT_AND_LOG_RTN_INT(pszConfFile);

    m_stZoneConfList.m_ushLen = 0;

    ifstream inFile(pszConfFile);
    if (!inFile.good())
    {
        TRACESVR("failed to open config file: %s\n", pszConfFile);

        return -1;
    }

    int i = 0;
    char szLine[1024];
    TZoneConf stZoneConfTmp;
    memset(&m_stZoneConfList, 0, sizeof(m_stZoneConfList));

    while (inFile.getline(szLine, sizeof(szLine)))
    {
        if (('\r' == szLine[0]) || (0 == strlen(szLine))) // ���У�vi����һ��ʱ��������\r
        {
            break;
        }

        stringstream strStream(szLine);

        strStream >> stZoneConfTmp.m_szHostIP
            >> stZoneConfTmp.m_ushPort
            >> stZoneConfTmp.m_ushZoneID
            >> stZoneConfTmp.m_szZoneName;

        int iZoneID = stZoneConfTmp.m_ushZoneID;
        if (iZoneID >= MAX_ZONE_PER_WORLD)
        {
            continue;
        }

        memcpy(&m_stZoneConfList.m_astZoneConf[i], &stZoneConfTmp, sizeof(TZoneConf));

        m_stZoneConfList.m_ushLen++;
        i++;
    }

    inFile.close();

    return 0;
}
