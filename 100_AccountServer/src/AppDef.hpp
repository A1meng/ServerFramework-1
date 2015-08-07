#ifndef __APP_DEF_HPP__
#define __APP_DEF_HPP__


//���������һЩ�궨��
#ifdef _DEBUG_
const int MAX_SERVICEAPP_NUMBER = 2; //��������ķ�����̵���Ŀ
#else
const int MAX_SERVICEAPP_NUMBER = 8; //��������ķ�����̵���Ŀ
#endif

const int CUBE_DB_UIN_MODE = 256; //��ѡ����ʱ��Uin��Ҫ��256��ģ


//ͳ��Cache����
const int STAT_CACHE_ID_TIMER      = 0;        //��ʱ��
const int STAT_CACHE_ID_TIMERINDEX = 1;        //��ʱ������
const int STAT_CHCHE_ID_TRANSFER   = 2;        //�ͻ�����Ϣͷ

#define STAT_CACHE_NAME_TIMER       "TimerCache"
#define STAT_CACHE_NAME_TIMERINDEX  "TimerIndexCache"
#define STAT_CACHE_NAME_TRANSFER    "TransferCache"

#define APP_CONFIG_FILE     "../conf/GameServer.tcm"

#endif

