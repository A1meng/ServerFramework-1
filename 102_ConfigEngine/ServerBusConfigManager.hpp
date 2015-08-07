#ifndef __SERVER_BUS_CONFIG_MANAGER_HPP__
#define __SERVER_BUS_CONFIG_MANAGER_HPP__

#include <stdint.h>

#include "GameConfigDefine.hpp"

using namespace GameConfig;

//�� conf/GameServer.tcm �ļ��ж�ȡ������ͨ��BUS������

struct ServerBusConfig
{
    uint64_t ullBusClientID;                    //ZMQ ͨ��ͨ��Client��ID
    uint64_t ullBusServerID;                    //ZMQͨ��ͨ��Server��ID
    char szBusAddr[MAX_SERVER_BUS_ADDR_LEN];    //ZMQͨ��ͨ����ʵ�ʵ�ַ����ʽΪ ip:port

    ServerBusConfig()
    {
        memset(this, 0, sizeof(*this));
    }
};

class CServerBusConfigManager
{
public:
    CServerBusConfigManager() { memset(this, 0, sizeof(*this)); };
    ~CServerBusConfigManager() { };

public:
    int LoadServerBusConfig();

    const ServerBusConfig* GetServerBusConfig(uint64_t ullClientID, uint64_t ullServerID);

private:

    //�����������ܹ���Ҫ֧�ֵ�ͨ��Bus����
    int m_iTotalBusNumber;

    //����������֧�ֵ�ͨ��Bus������
    ServerBusConfig m_astServerBusConfig[MAX_SERVER_BUS_NUMBER];
};

#endif
