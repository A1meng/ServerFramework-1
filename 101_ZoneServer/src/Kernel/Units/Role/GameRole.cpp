#include <algorithm>
#include <string.h>

#include "MathHelper.hpp"
#include "GameRole.hpp"
#include "ZoneMsgHelper.hpp"
#include "LogAdapter.hpp"
#include "ModuleHelper.hpp"
#include "TimeUtility.hpp"
#include "Random.hpp"
#include "StringUtility.hpp"
#include "UpdateRoleInfoHandler.hpp"
#include "LogoutHandler.hpp"
#include "UnitUtility.hpp"
#include "FreqCtrl.hpp"

using namespace ServerLib;

IMPLEMENT_DYN(CGameRoleObj)

CGameRoleObj::CGameRoleObj()
{
}

CGameRoleObj::~CGameRoleObj()
{
}

int CGameRoleObj::Initialize()
{
    m_stRoleInfo.stBaseInfo.iForbidTalkingTime = 0;

    m_stKicker.Clear();

    return 0;
}

// ��ʼ����ҵ�������ֵΪĬ����ֵ
int CGameRoleObj::InitRole(const RoleID& stRoleID)
{
    time_t tNow = time(NULL);

    SetRoleID(stRoleID);

    memset(&m_stRoleInfo.stBaseInfo, 0, sizeof(m_stRoleInfo.stBaseInfo));

    // ��¼������ʱ��
    m_stRoleInfo.stBaseInfo.iLoginTime = tNow;
    m_stRoleInfo.stBaseInfo.iLogoutTime = tNow;
    m_stRoleInfo.stBaseInfo.iLastLogin = tNow;

 
    //��ҵ�ս����Ϣ
    m_stRoleInfo.stBaseInfo.iBattlefieldObjIndex = -1;
    m_stRoleInfo.stBaseInfo.bIsSelfWin = false;

    m_iLastActiveTime = tNow;

    m_iLastTickTime = tNow;
    m_iLastUpdateDBTickTime = tNow;
    m_iLastUpdateCheckTimeoutTime = tNow;
    m_iLastMinTime = tNow;
    m_iLastTenSecondTime = tNow;
    m_iLastTwoSecondTime = tNow;

    m_enAAStatus = EAS_HEALTHY;
    SetOffline();

    memset(m_abLock, 0, sizeof(m_abLock));

    //��ʼ���������߹�����
    m_stRepThingsManager.Initialize();
    m_stRepThingsManager.SetOwner(GetUin());

    //��ʼ��ս����λ������
    m_stFightUnitManager.Initialize();
    m_stFightUnitManager.SetOwner(GetUin());

    m_stKicker.Clear();

    return 0;
}

void CGameRoleObj::SetNickName(const char* strNickName)
{
    ASSERT_AND_LOG_RTN_VOID(strNickName);
    
    SAFE_STRCPY(m_stRoleInfo.stBaseInfo.szNickName, strNickName, sizeof(m_stRoleInfo.stBaseInfo.szNickName)-1);
}

void CGameRoleObj::SetSessionID(int iSessionID)
{
    m_iSessionID = iSessionID;
}

int CGameRoleObj::GetSessionID()
{
    return m_iSessionID;
}

CGameSessionObj* CGameRoleObj::GetSession()
{
    return CModuleHelper::GetSessionManager()->FindSessionByID(m_iSessionID);
}

//todo jasonxiong ��ʱע�͵�������أ�����ͳһ����
/*
CQuestManager& CGameRoleObj::GetQuestManager()
{
    return m_stQuestManager;
} 
*/ 

bool CGameRoleObj::IsFirstLoginToday()
{
    time_t tTodayZero = CTimeUtility::GetTodayTime(CTimeUtility::m_stTimeValueTick.GetTimeValue().tv_sec);
    return (tTodayZero > m_stRoleInfo.stBaseInfo.iLastLogout);
}

void CGameRoleObj::InitLoginTime()
{
    m_stRoleInfo.stBaseInfo.iLoginTime = time(NULL);
    m_iLastWorldChatTime = 0;
    m_iLastNearbyChatTime = 0;
}

int CGameRoleObj::GetLoginTime()
{
    return m_stRoleInfo.stBaseInfo.iLoginTime;
}

int CGameRoleObj::GetLogoutTime()
{
    return m_stRoleInfo.stBaseInfo.iLogoutTime;
}

int CGameRoleObj::GetLastLoginTime()
{
    return m_stRoleInfo.stBaseInfo.iLastLogin;
}

void CGameRoleObj::SetLogoutTime(int iLogoutTime)
{
    m_stRoleInfo.stBaseInfo.iLogoutTime = iLogoutTime;
    return;
}

void CGameRoleObj::SetLastLoginTime(int iLastLoginTime)
{
    if (!iLastLoginTime)
    {
        return ;
    }

    m_stRoleInfo.stBaseInfo.iLastLogin = iLastLoginTime;
}

void CGameRoleObj::SetCreateTime(int iCreateTime)
{
    m_stRoleInfo.stBaseInfo.iCreateTime = iCreateTime;
}

int CGameRoleObj::GetCreateTime()
{
    return m_stRoleInfo.stBaseInfo.iCreateTime;
}

void CGameRoleObj::SetOnlineTotalTime(int iOnlineTime)
{
    m_stRoleInfo.stBaseInfo.iOnlineTime = iOnlineTime;
}

int CGameRoleObj::GetOnlineTotalTime()
{
    m_stRoleInfo.stBaseInfo.iOnlineTime += GetOnlineThisTime();

    return m_stRoleInfo.stBaseInfo.iOnlineTime;
}

int CGameRoleObj::GetOnlineThisTime()const
{
    int iOnlineThisTime = CTimeUtility::m_uiTimeTick - m_stRoleInfo.stBaseInfo.iLoginTime;
    if (iOnlineThisTime <= 0)
    {
        iOnlineThisTime = 0;
    }

    return iOnlineThisTime;
}

void CGameRoleObj::PrintMyself()
{
    //todo jasonxiong ��������ߴ�ӡ���GameRole����ϸ��Ϣ

    return;
}

int CGameRoleObj::Resume()
{
    TRACESVR("role obj resume\n");

    //todo jasonxiong ��ʱע�͵����أ�����ͳһ����
    //m_stActivityManager.RegisterAllActivities();

    return 0;
}

const char* CGameRoleObj::GetNickName()
{
    return m_stRoleInfo.stBaseInfo.szNickName;
}

void CGameRoleObj::UpdateBaseInfoToDB(BASEDBINFO& rstBaseInfo)
{
    rstBaseInfo.set_sznickname(GetNickName());
    rstBaseInfo.set_ustatus(GetRoleStatus()&(!EGUS_LOGOUT));    //�����Logout��־
    rstBaseInfo.set_ilastlogin(GetLastLoginTime());
    rstBaseInfo.set_ilastlogout(GetLastLogoutTime());
    rstBaseInfo.set_icreatetime(GetCreateTime());
    rstBaseInfo.set_ionlinetime(GetOnlineTotalTime());
    rstBaseInfo.set_ilogincount(GetLoginCount());
    rstBaseInfo.set_iforbidtalkingtime(GetForbidTalkingTime());
    rstBaseInfo.set_ilogintime(GetLoginTime());
    rstBaseInfo.set_ilogouttime(GetLogoutTime());

    return;
}

void CGameRoleObj::InitBaseInfoFromDB(const BASEDBINFO& rstBaseInfo)
{
    //todo jasonxiong �����治��Ҫ����
    SetNickName(rstBaseInfo.sznickname().c_str());
    SetRoleStatus(rstBaseInfo.ustatus());
    SetLastLoginTime(rstBaseInfo.ilastlogin());
    SetLastLogoutTime(rstBaseInfo.ilastlogout());
    SetCreateTime(rstBaseInfo.icreatetime());
    SetOnlineTotalTime(rstBaseInfo.ionlinetime());
    SetLoginCount(rstBaseInfo.ilogincount());
    SetForbidTalkingTime(rstBaseInfo.iforbidtalkingtime());
    InitLoginTime();
    SetLogoutTime(rstBaseInfo.ilogouttime());

    return;
}

// ˢ�»״̬
void CGameRoleObj::ActiveRefresh()
{
    m_iLastActiveTime = time(NULL);
}

bool CGameRoleObj::IsUnactive()
{
    return time(NULL) > (m_iLastActiveTime + UNACTIVE_ROLE_TIMEOUT);
}

// �������컹ʣ����ȴʱ��
int CGameRoleObj::GetWorldChatCD()
{
    const int WORLD_CHAT_CD = 5;
    int iNow = time(NULL);
    int iPassTime = iNow - m_iLastWorldChatTime;

    if (iPassTime < 0)
    {
        iPassTime = 0;
    }

    if (iPassTime > WORLD_CHAT_CD)
    {
        iPassTime = WORLD_CHAT_CD;
    }

    return (WORLD_CHAT_CD - iPassTime);
}

//����������ȴʱ��
bool CGameRoleObj::CanChatNearbyNow()
{
    const unsigned char NEARBYCHATCD = 3;
    int iPassTime = CTimeUtility::m_uiTimeTick - m_iLastNearbyChatTime;

    if (iPassTime >= NEARBYCHATCD)
    {
        return true;
    }
    else
    {
        return false;
    }
}

void CGameRoleObj::RefreshWorldChatCD()
{
    m_iLastWorldChatTime = time(NULL);
    return;
}

void CGameRoleObj::RefreshNearbyChatTime()
{
    m_iLastNearbyChatTime = CTimeUtility::m_uiTimeTick;
}

void CGameRoleObj::OnTick()
{
    time_t tNow = time(NULL);
    int iTickTime = tNow - m_iLastTickTime;
    if (iTickTime < 1)
    {
        return;
    }

    m_iLastTickTime = tNow;

    // ����Ƿ�ʱ�䲻��Ծ�����
    if (IsUnactive())
    {
        CLogoutHandler::LogoutRole(this, LOGOUT_REASON_KICKOFF);
        
        //ˢ����һ�Ծ��ʱ��
        ActiveRefresh();

        return;
    }

    // �������֪ͨ
    if (!IsOnline())
    {
        CLogoutHandler::LogoutAction(this);
        return;
    }

    //���ж�ʱ�������������
    OnTwoSecondTimerUp(tNow); //2�붨ʱ��

    // ������߳�ʱ�ط�
    if (CUnitUtility::IsUnitStatusSet(&m_stRoleInfo.stUnitInfo, EGUS_LOGOUT))
    {
        if (tNow - m_stRoleInfo.stBaseInfo.iLogoutTime > ROLEDATA_REWRITE_SECONDS)
        {
            if (CFreqencyCtroller<0>::Instance()->CheckFrequency(1))
            {
                CLogoutHandler::LogoutRole(this, 0);
                m_stRoleInfo.stBaseInfo.iLogoutTime = tNow;
            }
        }

        return;
    }

    // ���ͣ������
    if (GAME_SERVER_STATUS_STOP == GetServerStatus())
    {
        if (!CUnitUtility::IsUnitStatusSet(&m_stRoleInfo.stUnitInfo, EGUS_LOGOUT))
        {
            if (CFreqencyCtroller<0>::Instance()->CheckFrequency(1))
            {
                CLogoutHandler::LogoutRole(this, LOGOUT_REASON_KICKOFF);
            }
        }

        return;
    }

    // ����DB
    if ((tNow - m_iLastUpdateDBTickTime) >= TICK_TIMEVAL_UPDATEDB
            && GetServerStatus() != GAME_SERVER_STATUS_STOP)
    {
        CUpdateRoleInfo_Handler::UpdateRoleInfo(this, 0);
        m_iLastUpdateDBTickTime = tNow; 
    }

    //һ���Ӷ�ʱ��
    if ((tNow - m_iLastMinTime) >= ONE_MIN_TIMEOUT)
    {
        m_iLastMinTime = tNow;
    }

    //10�붨ʱ��
    if ((tNow - m_iLastTenSecondTime) >= TEN_SECOND_TIMER)
    {
        m_iLastTenSecondTime = tNow;
    }

    return;
}

void CGameRoleObj::SetOnline()
{
    CUnitUtility::SetUnitStatus(&m_stRoleInfo.stUnitInfo, EGUS_ONLINE);
}

void CGameRoleObj::SetOffline()
{
    CUnitUtility::ClearUnitStatus(&m_stRoleInfo.stUnitInfo, EGUS_ONLINE);
}

bool CGameRoleObj::IsOnline()
{
    return CUnitUtility::IsUnitStatusSet(&m_stRoleInfo.stUnitInfo, EGUS_ONLINE);
}

CRepThingsManager& CGameRoleObj::GetRepThingsManager()
{
    return m_stRepThingsManager;
}

void CGameRoleObj::UpdateRepThingsToDB(ITEMDBINFO& rstItemDBInfo)
{
    m_stRepThingsManager.UpdateRepItemToDB(rstItemDBInfo);

    return;
}

void CGameRoleObj::InitRepThingsFromDB(const ITEMDBINFO& rstItemDBInfo)
{
   m_stRepThingsManager.InitRepItemFromDB(rstItemDBInfo);

   return;
}

CFightUnitManager& CGameRoleObj::GetFightUnitManager()
{
    return m_stFightUnitManager;
}

int CGameRoleObj::UpdateFightUnitToDB(FIGHTDBINFO& stFightDBInfo)
{
    return m_stFightUnitManager.UpdateFightUnitInfoToDB(stFightDBInfo);
}

int CGameRoleObj::InitFightUnitFromDB(const FIGHTDBINFO& rstFightDBInfo)
{
    return m_stFightUnitManager.InitFightUnitInfoFromDB(rstFightDBInfo);
}

void CGameRoleObj::SetLogoutReason(int iReason)
{
    m_iLogoutReason = iReason;
}

bool CGameRoleObj::IsMsgFreqLimit(const int iMsgID) const
{
    int iLastPushIdx = (MAX_MSG_QUEUE_LEN + m_stMsgInfoQueue.m_iMsgQueueIndex - 1) % MAX_MSG_QUEUE_LEN;
    if (CModuleHelper::GetConfigManager()->GetMsgInterval(iMsgID) == 0)
    {
        return false;
    }

    while (iLastPushIdx != m_stMsgInfoQueue.m_iMsgQueueIndex)
    {
        const MsgInfo& rstMsgInfo = m_stMsgInfoQueue.m_astMsgInfo[iLastPushIdx];
        //�յĸ���
        if ((rstMsgInfo.m_stTimeval.tv_sec == 0)||(rstMsgInfo.m_wMsgID == 0))
        {
            return false;
        }

        //�����������
        CTimeValue stTimeDiff = CTimeUtility::m_stTimeValueTick - CTimeValue(rstMsgInfo.m_stTimeval);
        if (stTimeDiff.GetMilliSec() >= MAX_MSG_INTERVAL_TIME)
        {
            return false;
        }

        //�ҵ�����ͬ����ϢID, ����Ҳ��lotus��
        if ((rstMsgInfo.m_wMsgID == iMsgID)&& (rstMsgInfo.m_ucSource == (unsigned char)GAME_SERVER_LOTUSZONE))
        {
            //С�ڼ��ֵ
            if (stTimeDiff.GetMilliSec() < CModuleHelper::GetConfigManager()->GetMsgInterval(iMsgID))
            {
                return true;
            }
        }

        iLastPushIdx = (MAX_MSG_QUEUE_LEN + iLastPushIdx - 1) % MAX_MSG_QUEUE_LEN;
    }

    return false;
}

void CGameRoleObj::PushMsgID(const int iMsgID, const unsigned char ucSource)
{
    MsgInfo& rstMsgInfo = m_stMsgInfoQueue.m_astMsgInfo[m_stMsgInfoQueue.m_iMsgQueueIndex];
    rstMsgInfo.m_wMsgID = iMsgID;
    rstMsgInfo.m_ucSource = ucSource;
    rstMsgInfo.m_stTimeval = CTimeUtility::m_stTimeValueTick.GetTimeValue();

    m_stMsgInfoQueue.m_iMsgQueueIndex = (m_stMsgInfoQueue.m_iMsgQueueIndex + 1) % MAX_MSG_QUEUE_LEN;
}

void CGameRoleObj::OnTwoSecondTimerUp(time_t tNow)
{
    //2�붨ʱ��
    if ((tNow - m_iLastTwoSecondTime) < 2)
    {
        return;
    }

    m_iLastTwoSecondTime = tNow;
}

