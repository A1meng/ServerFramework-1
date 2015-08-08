#ifndef __APP_DEF_HPP__
#define __APP_DEF_HPP__


//���������һЩ�궨��
#ifdef _DEBUG_
const int MAX_SERVICEAPP_NUMBER = 2; //��������ķ�����̵���Ŀ
#else
const int MAX_SERVICEAPP_NUMBER = 8; //��������ķ�����̵���Ŀ
#endif

//ͳ��Cache����
const int STAT_CACHE_ID_TIMER      = 0;        //��ʱ��
const int STAT_CACHE_ID_TIMERINDEX = 1;        //��ʱ������
const int STAT_CHCHE_ID_TRANSFER   = 2;        //�ͻ�����Ϣͷ

#define STAT_CACHE_NAME_TIMER       "TimerCache"
#define STAT_CACHE_NAME_TIMERINDEX  "TimerIndexCache"

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

#endif

