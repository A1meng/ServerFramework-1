#ifndef __NAMEDBLOGMANAGER_HPP
#define __NAMEDBLOGMANAGER_HPP

#include "NameDBThreadManager.hpp"
#include "LogAdapter.hpp"


#ifdef _QQGAME_NO_LOG_
#define TRACE_THREAD(iThreadIdx, format, ...)
#else
//!������־������־�ȼ���ǿ�ƴ�
#define TRACE_THREAD(iThreadIdx, format, ...) ((CNameDBApp*)CAppType<CNameDBApp>::GetApp())->GetLogAdapter(iThreadIdx)->m_stLogFile.WriteLogEx(LOG_LEVEL_ANY, ((CNameDBApp*)CAppType<CNameDBApp>::GetApp())->GetLogAdapter(iThreadIdx)->m_iTraceDepth, __FILE__, __LINE__, __FUNCTION__, format, ##__VA_ARGS__)	
#endif																

#ifdef _QQGAME_NO_LOG_													
#define DEBUG_THREAD(iThreadIdx, format, ...)								
#else															
//!������־��DEBUG��־��LOG_LEVEL_DETAIL�ȼ�
#define DEBUG_THREAD(iThreadIdx, format, ...) ((CNameDBApp*)CAppType<CNameDBApp>::GetApp())->GetLogAdapter(iThreadIdx)->m_stLogFile.WriteLogEx(LOG_LEVEL_DETAIL, ((CNameDBApp*)CAppType<CNameDBApp>::GetApp())->GetLogAdapter(iThreadIdx)->m_iTraceDepth, __FILE__, __LINE__, __FUNCTION__, format, ##__VA_ARGS__)
#endif

class CNameDBLogManager
{
public:
	int Initialize(const int iLogNum, const TLogConfig& rLogConfig);
	CServerLogAdapter* GetLogAdapter(const int iThreadIdx);
	
private:
	int m_iLogNum;
	CServerLogAdapter* m_pLogAdapter;
};


#endif
