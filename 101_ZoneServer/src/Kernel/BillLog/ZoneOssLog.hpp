#ifndef __ZONE_OSS_LOG_HPP__
#define __ZONE_OSS_LOG_HPP__

///////////////////////////////////////////////////////////////////////////////////// 
#include "LogAdapter.hpp"
#include "GameConfigDefine.hpp"
#include "ModuleHelper.hpp"

using namespace ServerLib;

class CGameRoleObj;

//OSS��Ӫϵͳ��־������

//todo jasonxiong Ŀǰ�����ڼ䣬ֱ�����������������Ŀ¼
//todo jasonxiong �Ⱥ���Խ�ƽ̨���޸�Ϊ��Ӧ��ƽ̨

//�����Ӫ��־������
enum TOssLogType
{
    OSS_LOG_TYPE_INVALID = 0,           //�Ƿ�����־��������
    OSS_LOG_TYPE_LOGIN = 10001,         //��ҵ�¼�ʺ�
    OSS_LOG_TYPE_LOGOUT = 10002,        //��ҵǳ��ʺ�
    OSS_LOG_TYPE_DOCOMBAT = 10003,      //��ҽ���ս������־
    OSS_LOG_TYPE_ITEMTRADE = 10004,     //��ҵ���Ʒ������־
    OSS_LOG_TYPE_ADDLIFESKILL = 10005,  //���ѧϰ������䷽����־
    OSS_LOG_TYPE_BEGINMINIGAME = 10006, //��ҿ�ʼС��Ϸ����־
    OSS_LOG_TYPE_ENDMINIGAME = 10007,   //��ҽ���С��Ϸ����־
    OSS_LOG_TYPE_UNITFEED = 10008,      //���С��������־
    OSS_LOG_TYPE_FIGHTUNIT = 10009,     //���ս����λ��¼����־
};

//��Ʒ���׵Ĳ�������
enum TItemTradeType
{
    ITEM_TRADE_TYPE_BUY = 1,        //������Ʒ
    ITEM_TRADE_TYPE_SELL = 2,       //������Ʒ
};

class CZoneOssLog
{
public:
    // ��ʼ��
    static int Initialize();

public:
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    //Zone��־��ӡ�ӿ�

    //��¼��¼�����־
    static void TraceLogin(CGameRoleObj& stRoleObj);
    static void TraceLogout(CGameRoleObj& stRoleObj);

    //��¼ս������־
    static void TraceRoleCombat(unsigned uin, int iCrossID, int iStartTime, bool bIsSelfWin);

    //���NPC������־
    static void TraceItemTrade(unsigned uin, int iOpType, int iResChange, int iItemID, int iItemNum);

    //���ѧϰ������䷽����־
    static void TraceAddLifeSkill(unsigned uin, int iSkillType, int iSkillID, int iAddTime);

    //��ҿ�ʼС��Ϸ����־
    static void TraceBeginMiniGame(unsigned uin, int iMiniGameType);

    //��ҽ���С��Ϸ����־
    static void TraceEndMiniGame(unsigned uin, int iMiniGameType);

    //���С��������־
    static void TraceUnitFeed(unsigned uin, int iUnitConfigID, int iNewLevel);
};

#endif
