#ifndef __ROLEDB_THREAD_HPP__
#define __ROLEDB_THREAD_HPP__

#include <pthread.h>

#include "SSProtocolEngine.hpp"
#include "RoleDBHandlerSet.hpp"
#include "CodeQueue.hpp"

using namespace ServerLib;

// RoleDB�߳�
class CRoleDBThread
{
public:
    int Initialize(bool bResume, const int iThreadIdx);
	int InitBase();
	int InitCodeQueue(bool bResume, const int iThreadIdx);
    int Run();//thread 

public:
	int GetIdx() const {return m_iThreadIdx;}

public:
	//receive msg from incode queue
	int ReceiveMsg(int& riCodeLength);
	//process msg
	int ProcessMsg(SHandleResult& stMsgHandleResult);
	//send msg to main thread
	int EncodeAndSendMsg(SHandleResult& stMsgHandleResult);

private:
	int CreateThread();

public:
	//push code into queue
	int PushCode(const unsigned char* pMsg, int iCodeLength);
	//pop code
	int PopCode(unsigned char* pMsg, int iMaxLength, int& riLength);
	
public:
	CSSProtocolEngine* GetProtocolEngine() {return &m_stProtocolEngine;}

private:
    CSSProtocolEngine m_stProtocolEngine;
    CRoleDBHandlerSet m_stRoleDBHandlerSet;

private:
	CCodeQueue* m_pInputQueue; //������Ϣ���У������ڲ��������������̷ַ߳�
	CCodeQueue* m_pOutputQueue; //�����Ϣ����
	//thread index in array
	int m_iThreadIdx;

private:
	pthread_t m_hTrd;
	pthread_attr_t m_stAttr;
	
private:
    GameProtocolMsg m_stGameMsg;
    unsigned char m_szCodeBuf[MAX_MSGBUFFER_SIZE];
};

#endif
