#ifndef __APP_DEF_HPP__
#define __APP_DEF_HPP__


//���������һЩ�궨��
#ifdef _DEBUG_
const int MAX_SERVICEAPP_NUMBER = 2;                //��������ķ�����̵���Ŀ
const int APP_NAMEDB_MAX_SLEEP_USEC = 10 * 1000;    //�߳�sleepʱ��
#else
const int MAX_SERVICEAPP_NUMBER = 8;                //��������ķ�����̵���Ŀ
const int APP_NAMEDB_MAX_SLEEP_USEC = 10;           //�߳�sleepʱ��
#endif

const int CUBE_DB_UIN_MODE = 256; //��ѡ����ʱ��Uin��Ҫ��256��ģ

//ͳ��Cache����
const int STAT_CACHE_ID_TIMER      = 0;        //��ʱ��
const int STAT_CACHE_ID_TIMERINDEX = 1;        //��ʱ������
const int STAT_CHCHE_ID_TRANSFER   = 2;        //�ͻ�����Ϣͷ

#define STAT_CACHE_NAME_TIMER       "TimerCache"
#define STAT_CACHE_NAME_TIMERINDEX  "TimerIndexCache"
#define STAT_CACHE_NAME_TRANSFER    "TransferCache"

typedef enum enIOIndex
{
    EII_LoginProxy  = 0,
    EII_FloraProxy  = 1,
    EII_TTCProxy    = 2,
    EII_QueryProxy  = 3,
    EII_AvatarProxy = 4,

    //100 ����ΪProxyID
    EII_CodeQueue = 100,

} EIOIndex;

#define APP_CONFIG_FILE     "../conf/GameServer.tcm"

//���ӵ�MYSQL���ݿ���ص������ļ�
#define NAMEDBINFO_CONFIG_FILE "../conf/DBMSConf.xml"

//����MYSQL�ı���

//�������ݿ�ı���
#define MYSQL_NAMEINFO_TABLE "t_wulin_nameinfo"

//�������ݿ����������: nickname, nametype, nameid
#define MYSQL_NAMEINFO_FIELDS   3

#endif

