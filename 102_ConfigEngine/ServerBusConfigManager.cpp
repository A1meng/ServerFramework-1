
#include "ErrorNumDef.hpp"
#include "LogAdapter.hpp"
#include "StringUtility.hpp"
#include "SectionConfig.hpp"
#include "ConfigHelper.hpp"

#include "ServerBusConfigManager.hpp"

using namespace ServerLib;

//Ĭ�϶��� conf/GameServer.tcm �ж�ȡ

#define SERVER_BUS_CONFIG_FILE  "../conf/GameServer.tcm"

int CServerBusConfigManager::LoadServerBusConfig()
{
    CSectionConfig stConfigFile;
    int iRet = stConfigFile.OpenFile(SERVER_BUS_CONFIG_FILE);
    if(iRet)
    {
        LOGERROR("Failed to load server bus config file: %s!\n", SERVER_BUS_CONFIG_FILE);
        return iRet;
    }

    //����ServerBus��������
    stConfigFile.GetItemValue("ServerBus", "ServerBusNum", m_iTotalBusNumber);

    //���ؾ���ServerBus������
    char szBusClientID[SERVER_BUSID_LEN] = {0};
    char szBusServerID[SERVER_BUSID_LEN] = {0};
    char szBusIPAddr[SERVER_BUSID_LEN] = {0};
    int iBusPort = 0;

    //Bus����������Key
    char szBusClientKey[SERVER_BUSID_LEN] = {0};
    char szBusServerKey[SERVER_BUSID_LEN] = {0};
    char szBusIPAddrKey[SERVER_BUSID_LEN] = {0};
    char szBusPortKey[SERVER_BUSID_LEN] = {0};

    unsigned short usWorldID = 0;
    unsigned short usServerID = 0;
    unsigned short usInstanceID = 0;
    unsigned short usZoneID = 0;

    for(int i=0; i<m_iTotalBusNumber; ++i)
    {
        //��װBus��Key
        SAFE_SPRINTF(szBusClientKey, SERVER_BUSID_LEN-1, "ServerBusClientID_%d", i);
        SAFE_SPRINTF(szBusServerKey, SERVER_BUSID_LEN-1, "ServerBusServerID_%d", i);
        SAFE_SPRINTF(szBusIPAddrKey, SERVER_BUSID_LEN-1, "ServerBusIP_%d", i);
        SAFE_SPRINTF(szBusPortKey, SERVER_BUSID_LEN-1, "ServerBusPort_%d", i);

        //��ȡ�����е�ֵ
        stConfigFile.GetItemValue("ServerBus", szBusClientKey, szBusClientID, sizeof(szBusClientID)-1, "0.0.0.0");
        stConfigFile.GetItemValue("ServerBus", szBusServerKey, szBusServerID, sizeof(szBusServerID)-1, "0.0.0.0");
        stConfigFile.GetItemValue("ServerBus", szBusIPAddrKey, szBusIPAddr, sizeof(szBusIPAddr)-1, "0.0.0.0");
        stConfigFile.GetItemValue("ServerBus", szBusPortKey, iBusPort, 0);

        //����ͨ��Bus��ClientID
        sscanf(szBusClientID, "%hu.%hu.%hu.%hu", &usWorldID, &usServerID, &usInstanceID, &usZoneID);
        m_astServerBusConfig[i].ullBusClientID = GetServerBusID(usWorldID, (EGameServerID)usServerID, usInstanceID, usZoneID);

        //����ͨ��Bus��ServerID
        sscanf(szBusServerID, "%hu.%hu.%hu.%hu", &usWorldID, &usServerID, &usInstanceID, &usZoneID);
        m_astServerBusConfig[i].ullBusServerID = GetServerBusID(usWorldID, (EGameServerID)usServerID, usInstanceID, usZoneID);

        //��װʵ��ͨ��Bus�ĵ�ַ,��ʽΪ IP:PORT
        sprintf(m_astServerBusConfig[i].szBusAddr, "%s:%d", szBusIPAddr, iBusPort);
    }

    return 0;
}

const ServerBusConfig* CServerBusConfigManager::GetServerBusConfig(uint64_t ullClientID, uint64_t ullServerID)
{
    for(int i=0; i<m_iTotalBusNumber; ++i)
    {
        if(m_astServerBusConfig[i].ullBusClientID == ullClientID &&
           m_astServerBusConfig[i].ullBusServerID == ullServerID)
        {
            return &m_astServerBusConfig[i];
        }
    }

    return NULL;
}
