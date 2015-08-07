
#ifndef __GAME_MSG_TRANSCEIVER_HPP__
#define __GAME_MSG_TRANSCEIVER_HPP__

// ��װCodeQueue/zmq��ͨ���Ĳ���, ͳһ����Ϣ����ȡ�ͷ��ͽ���
#include "ConfigHelper.hpp"
#include "CodeQueueManager.hpp"
#include "ZmqBus.hpp"

using namespace ServerLib;

const int MAX_FD_NUMBER = 1000000;

class CGameMsgTransceiver
{
public:
    // ��ʼ�����е���Դ, ����CodeQueue, zmq��
    int Initialize(bool bResumeMode);

public:
    // ���ͺͽ�����Ϣ
    int SendOneMsg(const char* pszMsg, int iMsgLength, EGameServerID enMsgPeer, int iInstanceID = 0);
    int RecvOneMsg(char* pszMsg, int iMaxOutMsgLen, int& riMsgLength, EGameServerID enMsgPeer, int iInstanceID = 0);

	int GetCodeQueueNum() const
	{
		return m_stCodeQueueManager.GetCodeQueueNum();
	}

private:

    //ZoneServer �� LotusServer֮��ͨ��CodeQueueͨ��
	CCodeQueueManager m_stCodeQueueManager;
	
private:

    //ZoneServer �� WorldServer֮��ͨ��ZMQ����ͨ��
    ZmqBus m_oZone2WorldClient;
};

#endif

