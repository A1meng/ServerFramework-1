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

#define APP_CONFIG_FILE "../conf/GameServer.tcm"

#define GM_PRIV_CONFIG_FILE "../conf/GMPrivConfig.xml"

#endif
