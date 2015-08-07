#ifndef __ACCOUNTDBLOGMANAGER_HPP
#define __ACCOUNTDBLOGMANAGER_HPP

#include "AccountDBThreadManager.hpp"
#include "LogAdapter.hpp"


#ifdef _QQGAME_NO_LOG_
#define TRACE_THREAD(iThreadIdx, format, ...)
#else
//!������־������־�ȼ���ǿ�ƴ�
#define TRACE_THREAD(iThreadIdx, format, ...) ((CAccountDBApp*)CAppType<CAccountDBApp>::GetApp())->GetLogAdapter(iThreadIdx)->m_stLogFile.WriteLogEx(LOG_LEVEL_ANY, ((CAccountDBApp*)CAppType<CAccountDBApp>::GetApp())->GetLogAdapter(iThreadIdx)->m_iTraceDepth, __FILE__, __LINE__, __FUNCTION__, format, ##__VA_ARGS__)	
#endif																

#ifdef _QQGAME_NO_LOG_													
#define DEBUG_THREAD(iThreadIdx, format, ...)								
#else															
//!������־��DEBUG��־��LOG_LEVEL_DETAIL�ȼ�
#define DEBUG_THREAD(iThreadIdx, format, ...) ((CAccountDBApp*)CAppType<CAccountDBApp>::GetApp())->GetLogAdapter(iThreadIdx)->m_stLogFile.WriteLogEx(LOG_LEVEL_DETAIL, ((CAccountDBApp*)CAppType<CAccountDBApp>::GetApp())->GetLogAdapter(iThreadIdx)->m_iTraceDepth, __FILE__, __LINE__, __FUNCTION__, format, ##__VA_ARGS__)
#endif

class CAccountDBLogManager
{
public:
	int Initialize(const int iLogNum, const TLogConfig& rLogConfig);
	CServerLogAdapter* GetLogAdapter(const int iThreadIdx);
	
private:
	int m_iLogNum;
	CServerLogAdapter* m_pLogAdapter;
};


#endif
