#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/file.h>

#include "AppLoop.hpp"
#include "AppDef.hpp"
#include "SignalUtility.hpp"
#include "NowTime.hpp"
#include "AppUtility.hpp"
#include "ShmObjectCreator.hpp"
#include "FileUtility.hpp"
#include "StringUtility.hpp"
#include "Random.hpp"
#include "ConfigHelper.hpp"

using namespace ServerLib;

// ��������ģʽ
void RunTestMode(int argc, char** argv);

// �ڴ����ģʽ
void CheckShareMemoryMode(int argc, char** argv)
{
	//TODO -Werror=unused-but-set-variable
	//int iWorldID;
	int iZoneID;
	int iInstanceID;

	if (argc < 5)
	{
		printf("Usage: ./StarZoneServer -vv worldid zoneid insid\n");
		return;
	}

	//TODO -Werror=unused-but-set-variable
	//iWorldID = atoi(argv[2]);
	iZoneID = atoi(argv[3]);
	iInstanceID = atoi(argv[4]);

	key_t iShmKey = GenerateServerShmKey(GAME_SERVER_ZONE, iZoneID * 10 + iInstanceID);

	int iTotalMemSize = sizeof(CAppLoop) + sizeof(CSharedMemory);

	//��ȡ�����ڴ�
	int iMemID = shmget(iShmKey, iTotalMemSize, 0444);//ֻ����ʽ
	if (iMemID < 0)
	{
		printf("Error: get shm:key %d size:%d fail. error:%s\n", iShmKey, iTotalMemSize, strerror(errno));
		return;
	}

	//���ӹ����ڴ�
	char* pShm = (char*)shmat(iMemID, NULL, SHM_RDONLY);//ֻ����ʽ����
	if (!pShm)
	{
		printf("Error: attach shm fail. shm id:%d. error:%s\n", iMemID, strerror(errno));
		return;
	}

	CAppLoop* pAppLoop = (CAppLoop*)(pShm + sizeof(CSharedMemory));

	while (true)
	{
		//��������Ϊ���ܱ��룬unused pAppLoop
		if (pAppLoop)
		{
			sleep(60);
		}
	}
}

// ����������ģʽ
void NormalServerMode(int argc, char** argv)
{
	bool bResume;
	int iWorldID;
	int iZoneID;
	int iInstanceID;

	CAppUtility::AppLaunch(argc, argv, CAppLoop::SetAppCmd, bResume, iWorldID, iInstanceID, &iZoneID, NULL, NULL, true);

	//todo jasonxiong ���ʹ�ù����ڴ��е����⣬����ֱ��new��������
	/*
	CSharedMemory stShmMain;
	CAppLoop* pAppLoop = CShmObjectCreator<CAppLoop>::CreateObjectByKey
		(&stShmMain, GenerateServerShmKey(GAME_SERVER_ZONE,iZoneID * 10 + iInstanceID));
	if(!pAppLoop)
	{
		TRACESVR("CreateObject CAppLoop Failed!\n");
		return;
	}
	*/
	
	CAppLoop* pAppLoop = new CAppLoop;
	int iRet = pAppLoop->Initialize(bResume, iWorldID, iZoneID, iInstanceID);
	if(iRet)
	{
		TRACESVR("MainLoop Initialize Error:%d. So Quit!\n", iRet);
		return;
	}

	pAppLoop->Run();
}

int main(int argc, char** argv)
{
	if (strcmp(argv[1], "-vv") == 0)
	{
		CheckShareMemoryMode(argc, argv);
	}
    else if (strcmp(argv[1], "-test") == 0)
    {
        RunTestMode(argc, argv);
    }
	else
	{
		NormalServerMode(argc, argv);
	}

	return 0;
}

// ����ģʽ
void RunTestMode(int argc, char** argv)
{
#ifdef _DEBUG_

    bool bResume = false;
    int iWorldID = 1;
    int iZoneID = 1;
    int iInstanceID = 1;

    // ʱ���Ż�
    NowTimeSingleton::Instance()->RefreshNowTime();
    NowTimeSingleton::Instance()->RefreshNowTimeVal();

    // ���ý������������
    CRandomCalculator::Initialize();

    CSharedMemory stShmMain;
    CAppLoop* pAppLoop = CShmObjectCreator<CAppLoop>::CreateObjectByKey
        (&stShmMain, GenerateServerShmKey(GAME_SERVER_ZONE, iZoneID * 10 + iInstanceID));
    if(!pAppLoop)
    {
        TRACESVR("CreateObject CAppLoop Failed!\n");
        return;
    }

    int iRet = pAppLoop->Initialize(bResume, iWorldID, iZoneID, iInstanceID);
    if(iRet)
    {
        TRACESVR("MainLoop Initialize Error:%d. So Quit!\n", iRet);
        return;
    }
#endif

}

