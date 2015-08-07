#ifndef __KYLIN_MSG_TRANSCEIVER_HPP__
#define __KYLIN_MSG_TRANSCEIVER_HPP__

#include "ZmqBus.hpp"

#include "SingletonTemplate.hpp"
#include "CodeQueueManager.hpp"
#include "ConfigHelper.hpp"

using namespace ServerLib;

const int MAX_FD_NUMBER = 1000000;
class CRegAuthMsgTransceiver
{
private:
    friend class CSingleton<CRegAuthMsgTransceiver>;
    CRegAuthMsgTransceiver();

public:
    virtual ~CRegAuthMsgTransceiver();

   // ��ʼ��CodeQueue��ZMQͨ��ͨ��
    int Initialize(bool bResumeMode, int iWorldID);

public:
    // ���ͺͽ�����Ϣ
    int SendOneMsg(const char* pszMsg, int iMsgLength, EGameServerID enMsgPeer, int iInstance = 0);
    int RecvOneMsg(char* pszMsg, int iMaxOutMsgLen, int& riMsgLength, EGameServerID enMsgPeer, int iInstance = 0);

    int GetCodeQueueNum() const
    {
        return m_stCodeQueueManager.GetCodeQueueNum();
    }

private:

    //����ͨ�Ų��֣�ʹ��ZMQ����ͨ�ţ������RegAuth��AccountDB��ͨ��
    ZmqBus m_oZmqRegAuth2AccountDBClient;

    // ��Lotus֮�仹��CodeQueueͨ��
    CCodeQueueManager m_stCodeQueueManager;
};

#endif

