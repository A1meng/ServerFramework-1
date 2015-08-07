#include <time.h>

#include "GameProtocol.hpp"
#include "WorldBillLog.hpp"
#include "ConfigHelper.hpp"
#include "StringUtility.hpp"
#include "WorldRoleStatus.hpp"
#include "TimeValue.hpp"
#include "ModuleHelper.hpp"

const int CWorldBillLog::MAX_BILL_EVENT_PER_SECOND = 65535;
int CWorldBillLog::m_iLastEventTime;
unsigned short CWorldBillLog::m_usEventSequence;

// ����Bill��־
CBillLogAdapter CWorldBillLog::m_stBillLog;

///////////////////////////////////////////////////////////////////////////////////////////////////
// ��־�ӿ�
void CWorldBillLog::TraceAddMoney(const unsigned uin, int iLoginType, int64_t iAmount)
{
    /*
    <!-- ����һ���־ -->
    <struct name="AddMoney"           id="LOG_ADDMONEY"       desc="����һ���־" version="1" >
    <entry name="iEventId"          type="biguint"          desc="�¼�ID, ���ڱ�ʶͬ����Ϣ��Ψһ��" />
    <entry name="iSrcUin"           type="uint"             desc="��ֵ��QQ��" />
    <entry name="iDstUin"           type="uint"             desc="�����õ�QQ��" />
    <entry name="dtEventTime"       type="datetime"         desc="�¼�ʱ��" />
    <entry name="iClientIp"         type="uint"             desc="������IP" />
    <entry name="iSource"           type="int"              desc="��ֵ����" />
    <entry name="iAmount"           type="bigint"           desc="���(����)" />
    <entry name="iRemark"           type="biguint"          desc="��ˮ" />
    <entry name="iDesc"             type="string "          size="64"   desc="��ע��Ϣ"/>
    </struct>
    */

    //todo jasonxiong ���еļ���ϵͳ������Ҫ���¿���
    /*
    TEventId stEventId;
    GenerateLogHead(LOG_ADDMONEY, stEventId, 0);

    ADDMONEY& rstAddMoney = m_stLogPkg.detail.addMoney;
    rstAddMoney.iEventId = *(tdr_ulonglong*)&stEventId;
    rstAddMoney.dtEventTime = TdrDateTime(time(NULL));
    rstAddMoney.iSrcUin = rstRoleID.m_uiUin;
    rstAddMoney.iDstUin = rstRoleID.m_uiUin;
    rstAddMoney.iClientIp = 0;
    rstAddMoney.iAmount = iAmount;
	rstAddMoney.iLoginType = iLoginType;
    WriteBillLog();
    */
    return;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

int CWorldBillLog::Initialize()
{
    //todo jasonxiong �������¿�������ϵͳ
    /*
    // ��ʼ��TDR���
    char szProtocolPath[256];

    GetCommonResourcePath(szProtocolPath, sizeof(szProtocolPath), KYLINPROTOCOL_FILE);

    ///����xml�ļ�����Ԫ���ݿ�
    int iRet = tdr_load_metalib(&m_pstWorldLib, szProtocolPath);
    if (TDR_ERR_IS_ERROR(iRet))
    {
        printf("tdr_load_metalib failed by file %s, for %s\n", szProtocolPath, tdr_error_string(iRet));
        ASSERT_AND_LOG_RTN_INT(0);
    }
    m_pstMsgMeta = tdr_get_meta_by_name(m_pstWorldLib, "LogPkg");
    if (TDR_ERR_IS_ERROR(iRet))
    {
        printf("tdr_get_meta_by_name GameProtocolMsg, for %s\n", tdr_error_string(iRet));
        ASSERT_AND_LOG_RTN_INT(0);
    }

    // ��ʼ��TLOG��ʩ
    m_pstTLogCtx = NULL;
    m_pstTLogCategory = NULL;

    m_pstTLogCtx = tlog_init_from_file(TLOG_CONFIG_FILE);
    if (m_pstTLogCtx)
    {
        m_pstTLogCategory = tlog_get_category(m_pstTLogCtx, "StarStory");
    }

    m_usEventSequence = 0;
    m_iLastEventTime = 0;
    */
    return 0;
}

void CWorldBillLog::GenerateLogHead(int iMsgID, TEventId& rstEventId, const unsigned char ucZoneID)
{
    //todo jasonxiong ��ʱ��ע�͵����������¿�������ϵͳ
    /*
    memset(&m_stLogPkg, 0, sizeof(m_stLogPkg));
    m_stLogPkg.cmd = iMsgID;

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
        rstEventId.m_ucWorldID = CModuleHelper::GetWorldID();
        rstEventId.m_ucZoneID = ucZoneID;
        rstEventId.m_usEventSeq = m_usEventSequence;
    }
    */

    return;
}

int CWorldBillLog::WriteBillLog()
{
    //todo jasonxiong ��ʱ��ע�͵����������¿�������ϵͳ

    /*
    TDRDATA stHost;
    TDRDATA stNet;
    TDRPRINTFORMAT stFormat;

    stFormat.chIndentChar = 0;
    stFormat.iNoVarName = 1;
    stFormat.pszSepStr = "|";

    stHost.pszBuff = (char *)&m_stLogPkg;
    stHost.iBuff = sizeof(LOGPKG);

    static char m_szSendBuff[1024*5];
    stNet.pszBuff = m_szSendBuff;
    stNet.iBuff = sizeof(m_szSendBuff);

    int iRet = tdr_sprintf_ex(m_pstMsgMeta, &stNet, &stHost,0, &stFormat);
    if(iRet != 0)
    {
        return iRet;
    }

    // д��ԶTLog��־
    if (m_pstTLogCategory)
    {
        tlog_info(m_pstTLogCategory, 0, 0, m_szSendBuff);
    }

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
    */
    
    return 0;
}



	
