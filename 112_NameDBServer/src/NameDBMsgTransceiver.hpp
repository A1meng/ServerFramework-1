#ifndef __NAMEDB_MSG_TRANSCEIVER_HPP__
#define __NAMEDB_MSG_TRANSCEIVER_HPP__

#include "ZmqBus.hpp"
#include "ConfigHelper.hpp"

class CNameDBMsgTransceiver
{
public:
    // ��ʼ��ZMQ Busϵͳ
    int Initialize(bool bResumeMode, int iWorldID);

public:
    // ���ͺͽ�����Ϣ
    int SendOneMsg(const char* pszMsg, int iMsgLength, EGameServerID enMsgPeer);
    int RecvOneMsg(char* pszMsg, int iMaxOutMsgLen, int& riMsgLength, EGameServerID enMsgPeer);

private:

    //World �� NameDB��ZMQͨ��ͨ��
    ZmqBus m_oWorld2NameDBSvr;
};

#endif

