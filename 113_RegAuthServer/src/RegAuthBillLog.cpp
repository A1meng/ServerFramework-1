#include "RegAuthApp.hpp"
#include "TimeValue.hpp"
#include "TimeUtility.hpp"
#include "RegAuthBillLog.hpp"
#include <time.h>

const int CRegAuthBillLog::MAX_BILL_EVENT_PER_SECOND = 65535;
int CRegAuthBillLog::m_iLastEventTime;
unsigned short CRegAuthBillLog::m_usEventSequence;

// ����Bill��־
CBillLogAdapter CRegAuthBillLog::m_stBillLog;

//������д��־���ݵĻ�����
char CRegAuthBillLog::m_szSendBuff[1024*5];

using namespace ServerLib;

///////////////////////////////////////////////////////////////////////////////////////////////////
//���ע���ʺ�
void CRegAuthBillLog::TraceRegAccount(const char* szAccountID, int iAccountType, unsigned uin, int iGender, const char* szPhone, 
                     unsigned uClientIP, TEventId& rstEventID)
{
    GenerateLogHead(rstEventID);

    char szDateTime[256] = {0};

    CTimeUtility::ConvertUnixTimeToTimeString((int)time(NULL), szDateTime);

    //��ӡ������ɫ��־����ʽ: LOG_TYPE|uin|accountID|eventID|iAccountType|iGender|szPhone|uClientIP
    //OSSLogWrapper::Instance()->LOGBinDebugEx(OSS_LOG_UID, "Register", m_szSendBuff, sizeof(m_szSendBuff)-1, 
    //                                         "LOG_REGACCOUNT|%u|%s|%lld|%d|%d|%s|%u|", uin, szAccountID, *(long long*)&rstEventID, iAccountType,
    //                                         iGender, szPhone, uClientIP);

    WriteBillLog();
}

//��ҵ�¼��֤�ʺ�
void CRegAuthBillLog::TraceAuthAccount(const char* szAccountID, int iAccountType, unsigned uin, TEventId& rstEventID)
{
    GenerateLogHead(rstEventID);

    char szDateTime[256] = {0};

    CTimeUtility::ConvertUnixTimeToTimeString((int)time(NULL), szDateTime);

    //��ӡ������ɫ��־����ʽ: LOG_TYPE|uin|accountID|eventID|iAccountType|iGender|szPhone|uClientIP
    //OSSLogWrapper::Instance()->LOGBinDebugEx(OSS_LOG_UID, "Auth", m_szSendBuff, sizeof(m_szSendBuff)-1, 
    //                                         "LOG_AUTHACCOUNT|%u|%s|%lld|%d", uin, szAccountID, *(long long*)&rstEventID, iAccountType);

    WriteBillLog();
}

//����޸��ʺ���Ϣ
void CRegAuthBillLog::TraceUpdateAccount(const char* szAccountID, int iAccountType, unsigned int uin, TEventId& rstEventID)
{
    GenerateLogHead(rstEventID);

    char szDateTime[256] = {0};

    CTimeUtility::ConvertUnixTimeToTimeString((int)time(NULL), szDateTime);

    //��ӡ������ɫ��־����ʽ: LOG_TYPE|uin|accountID|eventID|iAccountType|iGender|szPhone|uClientIP
    //OSSLogWrapper::Instance()->LOGBinDebugEx(OSS_LOG_UID, "Update", m_szSendBuff, sizeof(m_szSendBuff)-1, 
    //                                         "LOG_UPDATEACOUNT|%u|%s|%lld|%d", uin, szAccountID, *(long long*)&rstEventID, iAccountType);

    WriteBillLog();
}

///////////////////////////////////////////////////////////////////////////////////////////////////

int CRegAuthBillLog::Initialize()
{
    m_usEventSequence = 0;
    m_iLastEventTime = 0;

    //��ʼ��OSSLog
    /*
    int iRet = OSSLogWrapper::Instance()->OSSInit("../conf/OSSLog.xml");
    if(iRet)
    {
        TRACESVR("Failed to inti regauth oss log, ret %d\n", iRet);
        return iRet;
    }
    */

    return 0;
}

void CRegAuthBillLog::GenerateLogHead(TEventId& rstEventId)
{
    if (rstEventId.m_iEventTime == 0)
    {
        // ��ʼ���¼���ˮ��

        CTimeValue tvNow;
        tvNow.RefreshTime();
        int iEventTime = tvNow.GetTimeValue().tv_sec;
        m_usEventSequence++;

        if (iEventTime == m_iLastEventTime)
        {
            if (m_usEventSequence >= MAX_BILL_EVENT_PER_SECOND)
            {
                // �¼�����̫Ƶ��!
                TRACESVR("Alert: Event Too Frequently!\n");
                m_usEventSequence = 0;
            }
        }
        else
        {
            m_iLastEventTime = iEventTime;
            m_usEventSequence = 0;
        }

        rstEventId.m_iEventTime = iEventTime;
        rstEventId.m_ucWorldID = CRegAuthApp::GetWorldID();
        rstEventId.m_ucZoneID = 0;
        rstEventId.m_usEventSeq = m_usEventSequence;
    }
}

int CRegAuthBillLog::WriteBillLog()
{
    // д�뱾��BillLog��־�ļ�
    m_stBillLog.CheckBillPath();
    if (m_stBillLog.m_stLogFile.OpenLogFile())
    {
        TRACESVR("Cannot Open BillLog!\n");
        return -2;
    }

    FILE* pBillLogFile = m_stBillLog.m_stLogFile.GetFilePointer();
    if (!pBillLogFile)
    {
        TRACESVR("Cannot Get BillLog FILE!\n");
        return -3;
    }

    // д����־
    fprintf(pBillLogFile, "%s", m_szSendBuff);

    // ˢ����־
    fflush(pBillLogFile);

    // �ر���־�ļ�
    m_stBillLog.m_stLogFile.CloseFile();

    return 0;
}
