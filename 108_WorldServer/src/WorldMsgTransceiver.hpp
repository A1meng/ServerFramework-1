#ifndef __KYLIN_MSG_TRANSCEIVER_HPP__
#define __KYLIN_MSG_TRANSCEIVER_HPP__

#include "ZmqBus.hpp"
#include "ConfigHelper.hpp"
#include "CodeQueue.hpp"

using namespace ServerLib;

class CWorldMsgTransceiver
{
public:
    // ��ʼ��ͨ�ŵ�ZMQͨ��
    int Initialize(bool bResumeMode);

public:
    // ���ͺͽ�����Ϣ
    int SendOneMsg(const char* pszMsg, int iMsgLength, EGameServerID enMsgPeer, int iInstance = 1);

    //���Դ����ӵ�����ͨ������ѯ������Ϣ
    int RecvOneMsg(char* pszMsg, int iMaxOutMsgLen, int& riMsgLength, EGameServerID& enMsgPeer, int& iInstance);

private:
    ZmqBus* GetZmqBus(EGameServerID enMsgPeer, int iInstance);

private:

    //Account Server ��World֮���ͨ���ŵ�
    ZmqBus m_oAccount2WorldSvr;

    //Zone Server ��World֮���ͨ���ŵ�
    int m_iZone2WorldBusNum;
    //ע�⣺�����±���ZoneID��Ӧ���±�Ϊ0��λ��Ϊ�գ���ʹ��
    ZmqBus m_szZone2WorldSvrs[MAX_ZONE_PER_WORLD];

    //World �� ROLEDB֮���ͨ���ŵ�
    ZmqBus m_oWorld2RoleDBClient;

    //World �� Cluster֮���ͨ���ŵ�
    //ZmqBus m_oWorld2ClusterClient;

    //World �� NameDBServer ֮���ͨ���ŵ�
    ZmqBus m_oWorld2NameDBClient;
};

#endif

