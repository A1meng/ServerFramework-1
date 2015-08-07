#ifndef __APP_TICK_HPP__
#define __APP_TICK_HPP__

#include "TimeValue.hpp"
#include "OnlineStatTick.hpp"
#include "MsgStatistic.hpp"

using namespace ServerLib;

// ϵͳ������ʱ��
#define TICK_TIMEVAL_BASE        5

class CAppTick
{
public:
	CAppTick();

	int Initialize(bool bResume);

	int OnTick();

private:
	int CountObjStat();	
    int m_iLastStaticsTick;
    COnlineStatTick m_OnlineStatTick;
};

#endif
