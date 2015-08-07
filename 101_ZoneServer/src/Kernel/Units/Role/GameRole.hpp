#ifndef __GAME_ROLE_HPP__
#define __GAME_ROLE_HPP__

#include <vector>
#include <string.h>

#include "GameProtocol.hpp"
#include "SessionDefine.hpp"
#include "RttUtility.hpp"
#include "ObjAllocator.hpp"
#include "TimeUtility.hpp"
#include "LogAdapter.hpp"
#include "AppTick.hpp"
#include "GameObjCommDef.hpp"
#include "RepThingsManager.hpp"
#include "FightUnitManager.hpp"

using namespace ServerLib;

// ������״̬
typedef enum tagEnumAAStatus
{
    EAS_HEALTHY     = 0,        // ����0 ~ 3Сʱ
    EAS_TIRED       = 1,        // ƣ��3 ~ 5Сʱ
    EAS_UNHEALTHY   = 2,        // ������ >5Сʱ
} TAAStatus;

// ��Ϣ��¼
typedef struct
{
    unsigned short m_wMsgID;    //��ϢID
    unsigned char m_ucSource;   //��Դ
    timeval m_stTimeval;        //ʱ��
}MsgInfo;

const int MAX_MSG_QUEUE_LEN = 400;

typedef struct tagMsgInfoQueue
{
    int m_iMsgQueueIndex;
    MsgInfo m_astMsgInfo[MAX_MSG_QUEUE_LEN];

    tagMsgInfoQueue()
    {
        memset(this, 0, sizeof(*this));
    };
}MsgInfoQueue;

// ģ����
typedef enum
{
    MAIL_LOCK = 0,
    MARKET_LOCK = 1,
    MAX_LOCK_NUM = 2
}ENUM_LOCK;

class CGameSessionObj;
class CGameSceneObj;

class CGameRoleObj : public CObj
{
public:
    CGameRoleObj();
    virtual ~CGameRoleObj();
    virtual int Initialize();
    virtual int Resume();
    DECLARE_DYN

///////////////////////////////////////////////////////////////////////
public:

    // ��ʼ���������
    int InitRole(const RoleID& stRoleID);

    void SetRoleID(const RoleID& stRoleID) { m_stRoleInfo.stRoleID.CopyFrom(stRoleID); };
    const RoleID& GetRoleID() { return m_stRoleInfo.stRoleID; };
    unsigned int GetUin() const { return m_stRoleInfo.stRoleID.uin(); };
    void SetUin(unsigned int uiUin) { m_stRoleInfo.stRoleID.set_uin(uiUin); };

    TROLEINFO& GetRoleInfo() { return m_stRoleInfo; };
    TUNITINFO& GetUnitInfo() { return m_stRoleInfo.stUnitInfo; };

    int GetSessionID();
    void SetSessionID(int iSessionID);
    CGameSessionObj* GetSession();

    //sznickname ����
    void SetNickName(const char* strNickName);
    const char* GetNickName();

    //uStatus ״̬
    unsigned GetRoleStatus() { return m_stRoleInfo.stUnitInfo.uiUnitStatus; };
    void SetRoleStatus(int uStatus) { m_stRoleInfo.stUnitInfo.uiUnitStatus = uStatus; };

    //iLastLogin �ϴε�¼ʱ��
    int GetLastLoginTime() ;
    void SetLastLoginTime(int iLastLoginTime) ;

    //iLastLogout �ϴ�����ʱ��
    int GetLastLogoutTime(){ return m_stRoleInfo.stBaseInfo.iLastLogout;}
    void SetLastLogoutTime(int iLastLogoutTime){m_stRoleInfo.stBaseInfo.iLastLogout = iLastLogoutTime; LOGDEBUG("Debug: set lastlogouttime:%d, uin:%d\n", m_stRoleInfo.stBaseInfo.iLastLogout, m_stRoleInfo.stRoleID.uin());}
    bool IsFirstLoginToday();

    //iCreateTime �ʺŴ�����ʱ��
    void SetCreateTime(int iCreateTime);
    int GetCreateTime();
    
    //������ʱ��
    void SetOnlineTotalTime(int iOnlineTime);
    int GetOnlineTotalTime();
    int GetOnlineThisTime()const;

    //iLoginCount ����ܵĵ�¼����
    int GetLoginCount() {return m_stRoleInfo.stBaseInfo.iLoginCount;}
    void SetLoginCount(int iCount) { m_stRoleInfo.stBaseInfo.iLoginCount = iCount;}

    //iForbidTalkingTime ��ֹ��ҷ��Ե�ʱ��
    void SetForbidTalkingTime(int iForbidTalkingTime)
    {
        m_stRoleInfo.stBaseInfo.iForbidTalkingTime = iForbidTalkingTime;
    }

    int GetForbidTalkingTime() const
    {
        return m_stRoleInfo.stBaseInfo.iForbidTalkingTime;
    }

    //iLoginTime ���ε�¼��ʱ��
    void InitLoginTime();
    int GetLoginTime() ;

    //iLogoutTime ���εǳ���Ϸ��ʱ��
    int GetLogoutTime() ;
    void SetLogoutTime(int iLogoutTime) ;

    //todo jasonxiong1 �Ⱥ��濪��GMģ��ʱ��ͳһ����
    bool IsGM() { return false; };

    //��ҵ�ս����Ϣ
    void SetBattlefieObjID(int iBattlefiedObjIndex) { m_stRoleInfo.stBaseInfo.iBattlefieldObjIndex = iBattlefiedObjIndex; };
    int GetBattlefieObjID() { return m_stRoleInfo.stBaseInfo.iBattlefieldObjIndex; };

    //��ҵ�ս��ʤ����Ϣ
    void SetCombatResult(bool bIsWin) { m_stRoleInfo.stBaseInfo.bIsSelfWin = bIsWin; };
    bool GetCombatResult() { return m_stRoleInfo.stBaseInfo.bIsSelfWin; };

public:
    //������Ϣ�����ݿ��������
    void UpdateBaseInfoToDB(BASEDBINFO& rstBaseInfo);
    void InitBaseInfoFromDB(const BASEDBINFO& rstBaseInfo);

///////////////////////////////////////////////////////////////////////
public:

    void SetKicker(const World_KickRole_Request& rstKicker) {m_stKicker.CopyFrom(rstKicker);}
    World_KickRole_Request& GetKicker() {return m_stKicker;}

///////////////////////////////////////////////////////////////////////
///
public:
    // ˢ����ҵ��������Ϣ
    void ActiveRefresh();
    bool IsUnactive();

public:
    // �������컹ʣ����ȴʱ��
    int GetWorldChatCD();
    void RefreshWorldChatCD();

    //��������
    void RefreshNearbyChatTime();
    bool CanChatNearbyNow();

public:
    // Tick ����
    void OnTick();

public:
    // ������״̬
    void SetAAStatus(TAAStatus enAAStatus) {m_enAAStatus = enAAStatus;};
    TAAStatus GetAAStatus() {return m_enAAStatus;};

/////////////////////////////////////////////////////////////////////////////////////////
public:
    // ����ʹ��
    void PrintMyself();

public:
    // ����״̬
    void SetOnline();
    void SetOffline();
    bool IsOnline();

    void SetForbidTime(int iForbidTime)
    {
        m_iForbidTime = iForbidTime;
    }

    int GetForbidTime() const
    {
        return m_iForbidTime;
    }

public:

    bool TryToRefreshActivityDayLimit();

public:
    // ������GM�ʺ�, ������10000�����µ�
    //todo jasonxiong ��������GM����ʱ�����¿���
    bool IsRealGM() { return false; }

/////////////////////////////////////////////////////////////////////////////////////////
// ��Ա����
private:
    int m_iSessionID;

    //��ҽ�ɫ��������Ϣ
    TROLEINFO m_stRoleInfo;

    //��ֹ��¼ʱ��
    int m_iForbidTime;

    // �����������ʱ��
    int m_iLastWorldChatTime;

    // �ϴθ�������ʱ��
    int m_iLastNearbyChatTime;

private:
    // ����
    //CFriendManager m_stFriendManager;

    // ������¼
    int m_iLastActiveTime;

    // ��¼�������ߵ���
    World_KickRole_Request m_stKicker;

    // Tick��¼
    int m_iLastTickTime;
    int m_iLastUpdateDBTickTime;
    int m_iLastUpdateCheckTimeoutTime;
    int m_iLastMinTime;
    int m_iLastTenSecondTime;
        
    // ������״̬
    TAAStatus m_enAAStatus;

public:
    bool IsLock(ENUM_LOCK eLock)
    {
        return m_abLock[eLock];
    }

    void UnLock(ENUM_LOCK eLock)
    {
        m_abLock[eLock] = false;
    }

    void Lock(ENUM_LOCK eLock)
    {
        m_abLock[eLock] = true;
    }

private:
    //��Ϊ��
    //  �����޴Σ���ֹ������������첽�����쳣�����ֵ��߸��Ƶ�����
    bool m_abLock[MAX_LOCK_NUM];

public:

    //��ұ���������
    CRepThingsManager m_stRepThingsManager;

    CRepThingsManager& GetRepThingsManager();
    void UpdateRepThingsToDB(ITEMDBINFO& rstItemDBInfo);
    void InitRepThingsFromDB(const ITEMDBINFO& rstItemDBInfo);

    //��ҵ�ս����λ������
    CFightUnitManager m_stFightUnitManager;

    CFightUnitManager& GetFightUnitManager();
    int UpdateFightUnitToDB(FIGHTDBINFO& stFightDBInfo);
    int InitFightUnitFromDB(const FIGHTDBINFO& rstFightDBInfo);

public:
    void SetLogoutReason(int iReason);
    int GetLogoutReason() {return m_iLogoutReason;}

private:
    int m_iLogoutReason;

public:
    bool IsMsgFreqLimit(const int iMsgID) const;
    void PushMsgID(const int iMsgID, const unsigned char ucSource);
private:
    //��Ϣ����
    MsgInfoQueue m_stMsgInfoQueue;

public:
    unsigned char GetLogoutRequestReason() {return m_ucLogoutReason;}
    void SetLogoutRequestReason(unsigned char ucLogoutReason) {m_ucLogoutReason = ucLogoutReason;}

private:
    //��¼����ԭ��
    unsigned char m_ucLogoutReason;

public:
    void OnTwoSecondTimerUp(time_t tNow);

private:
    int m_iLastTwoSecondTime;
};

#endif
