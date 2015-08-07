#ifndef __APP_LOOP_HPP__
#define __APP_LOOP_HPP__

#include "GameProtocolEngine.hpp"
#include "HandlerList.hpp"
#include "ModuleHelper.hpp"
#include "ZoneObjectAllocator.hpp"
#include "TimeUtility.hpp"
#include "AppTick.hpp"
#include "GameMsgTransceiver.hpp"

using namespace ServerLib;

class CAppLoop
{
public:
    // �źŴ���
    static int ms_iAppCmd;
    static void SetAppCmd(int iAppCmd);

public:
    CAppLoop();

    int Initialize(bool bResume, int iWorldID, int iZoneID, int iInstanceID);
    int Run();

public:
    void OnTick();

    static CTimeValue m_stLotusMsgMaxProsessTime;
    static CTimeValue m_stWorldMsgMaxProcessTime;
    static CTimeValue m_stTickMaxProcessTime;

private:
    // ���ü���
    int LoadConfig();
    int ReloadConfig();

    // ͣ��
    bool Stop();

private:
    // �������������CAppLoop�У��ڹ����ڴ�ָ�

    CGameMsgTransceiver m_stZoneMsgTransceiver;
    CGameProtocolEngine m_stZoneProtocolEngine;
    CUnitEventManager m_stUnitEventManager;
    CConfigManager m_stConfigManager;
    CHandlerList m_stHandlerList;
    CSessionManager m_stSessionManager;
    CZoneObjectAllocator m_stAllocator;
    CSharedMemory m_stSharedMemoryTimer;
    CAppTick m_stAppTick;

private:
    bool  m_bResumeMode;
    CTimeValue m_stStopService;
    timeval m_tvLastCheckTimeout;
    timeval m_tvLastCheckStat;
};

#endif


