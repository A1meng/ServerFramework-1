
#ifndef __ACCOUNTDB_MSG_TRANSCEIVER_HPP__
#define __ACCOUNTDB_MSG_TRANSCEIVER_HPP__

#include "ZmqBus.hpp"
#include "ConfigHelper.hpp"

class CAccountDBMsgTransceiver
{
public:
    // ��ʼ��ZMQ Busϵͳ
    int Initialize(bool bResumeMode, int iWorldID);

public:
    // ���ͺͽ�����Ϣ
    int SendOneMsg(const char* pszMsg, int iMsgLength, EGameServerID enMsgPeer);
    int RecvOneMsg(char* pszMsg, int iMaxOutMsgLen, int& riMsgLength, EGameServerID enMsgPeer);

private:

    //RegAuthServer �� AccountDB��ZMQͨ��ͨ��
    ZmqBus m_oRegauth2AccountDBSvr;
};

#endif

