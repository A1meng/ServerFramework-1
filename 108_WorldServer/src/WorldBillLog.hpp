
#ifndef __BILL_LOG_HPP__
#define __BILL_LOG_HPP__

///////////////////////////////////////////////////////////////////////////////////// 
#include "LogAdapter.hpp"
#include "GameConfigDefine.hpp"

using namespace ServerLib;

const int MAX_BILL_LOG_LENGTH = 1024;

class CWorldRoleStatusWObj;

class CWorldBillLog
{
public:
    // ��ʼ��
    static int Initialize();

public:
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    // ��־�ӿ�

    // �ӿڱ���Ҫ����TEventId����
    // ��rstEventId.m_iEventTime��0ʱ, ��ʾ��������־����һ����־��ͬһ���¼�, ������ͬ��EventId		
    static void TraceAddMoney(const unsigned uin, int iLoginType, int64_t iAmount);
    	
    ///////////////////////////////////////////////////////////////////////////////////////////////////

private:
    static void GenerateLogHead(int iMsgID, TEventId& rstEventId, const unsigned char ucZoneID);
    static int WriteBillLog();

private:
    // ����Bill��־
    static CBillLogAdapter m_stBillLog;

private:
    // һ��Zone���¼���������ˮ��
    static const int MAX_BILL_EVENT_PER_SECOND;
    static int m_iLastEventTime;
    static unsigned short m_usEventSequence;
};

#endif
