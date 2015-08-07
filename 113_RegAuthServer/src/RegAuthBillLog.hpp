
#ifndef __BILL_LOG_HPP__
#define __BILL_LOG_HPP__

/////////////////////////////////////////////////////////////////////////////////////
#include "LogAdapter.hpp"

using namespace ServerLib;

#define OSS_LOG_UID "kingnetregauth"

const int MAX_BILL_LOG_LENGTH = 1024;

class CRegAuthBillLog
{
public:
    // ��ʼ��
    static int Initialize();

public:
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    // ��־�ӿ�

    // �ӿڱ���Ҫ����TEventId����
    // ��rstEventId.m_iEventTime��0ʱ, ��ʾ��������־����һ����־��ͬһ���¼�, ������ͬ��EventId

    //���ע���ʺ�
    static void TraceRegAccount(const char* szAccountID, int iAccountType, unsigned uin, int iGender, const char* szPhone, 
                                unsigned uClientIP, TEventId& rstEventID);

    //��ҵ�¼��֤�ʺ�
    static void TraceAuthAccount(const char* szAccountID, int iAccountType, unsigned uin, TEventId& rstEventID);

    //����޸��ʺ���Ϣ
    static void TraceUpdateAccount(const char* szAccountID, int iAccountType, unsigned int uin, TEventId& rstEventID);

    ///////////////////////////////////////////////////////////////////////////////////////////////////

private:
    static void GenerateLogHead(TEventId& rstEventId);
    static int WriteBillLog();

private:
    static char m_szSendBuff[1024*5];

    // ����Bill��־
    static CBillLogAdapter m_stBillLog;

private:
    // һ��Zone���¼���������ˮ��
    static const int MAX_BILL_EVENT_PER_SECOND;
    static int m_iLastEventTime;
    static unsigned short m_usEventSequence;
};

#endif
