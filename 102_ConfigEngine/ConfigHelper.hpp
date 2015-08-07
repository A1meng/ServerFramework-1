#ifndef __CONFIG_HELPER_HPP__
#define __CONFIG_HELPER_HPP__

#include <typeinfo>
#include <stdint.h>

#include "ErrorNumDef.hpp"
#include "LogAdapter.hpp"
#include "StringUtility.hpp"
#include "SortArray.hpp"

using namespace ServerLib;

extern int GAME_GET_WORLD_ID(uint64_t busid);
extern int GAME_GET_ZONE_ID(uint64_t busid);
extern int GAME_GET_FUNC_ID(uint64_t busid);
extern int GAME_GET_INST_ID(uint64_t busid);

// Server������ID, ��ͬ��TCM��FuncitionID
typedef enum enGameServerID
{
    GAME_SERVER_UNDEFINE        = -1,
    GAME_SERVER_WORLD           = 0,
    GAME_SERVER_ZONE            = 1,
    GAME_SERVER_ROLEDB          = 2,
    GAME_SERVER_ACCOUNT         = 3,
    GAME_SERVER_MAIL            = 4,
    GAME_SERVER_CHARGE          = 5,
    GAME_SERVER_LOGDB           = 6,
    GAME_SERVER_MAILDB          = 7,
    GAME_SERVER_FRIENDDB        = 8,
    GAME_SERVER_GUILDDB         = 9,
    GAME_SERVER_CLUSTER         = 10,

    GAME_SERVER_LOTUSACCOUNT    = 11,
    GAME_SERVER_LOTUSZONE       = 12,
    GAME_SERVER_LOTUSREGAUTH    = 13,

    GAME_SERVER_REGAUTH         = 14,

    GAME_SERVER_ACCOUNTDB       = 15,

    GAME_SERVER_NAMEDB          = 16,

    GAME_SERVER_MAX             = 17,
} EGameServerID;

typedef enum enServerStatus
{
    // ����������
    // 1) Zone��World����������Ϣ
    // 2) World��Cluster����������Ϣ
    GAME_SERVER_STATUS_INIT    = 1,

    // �ָ�������
    GAME_SERVER_STATUS_RESUME  = 2,

    // ����������
    GAME_SERVER_STATUS_IDLE    = 3,

    // æ������
    GAME_SERVER_STATUS_BUSY    = 4,

    // ׼��ֹͣ
    // 1) ֪ͨ�ͻ���׼��ͣ������
    // 2) ��ʼ����STOP״̬����ʱ
    GAME_SERVER_STATUS_PRESTOP = 5,

    // ֹͣ��
    // 1) ֪ͨLotusֹͣ���е��������
    // 2) �������е�Lotus����
    // 3) �������е�TBus����
    // 4) �������������������, ��֪ͨLotus�Ͽ�����
    // 5) ֹͣLotus��Zone������
    GAME_SERVER_STATUS_STOP    = 6,

} EGameServerStatus;

key_t GenerateServerShmKey(EGameServerID enServerID, int iKeyIndex);

extern EGameServerStatus g_enServerStatus;
inline void SetServerStatus(EGameServerStatus enStatus)
{
    g_enServerStatus = enStatus;
}

inline EGameServerStatus GetServerStatus()
{
    return g_enServerStatus;
}

// ���ɵ�ServerID: world:16.zone:16.function:16.instance:16
// ����TCM�淶, Ĭ��instance��1��ʼ
// ZoneIDΪ0, ��ʾ��������world�ķ�����
inline uint64_t GetServerBusID(short iWorldID, EGameServerID enServerID, short iInstance = 1, short iZoneID = 0)
{
    uint64_t ullBusID = ((uint64_t)iWorldID) << 48;

    ullBusID += ((uint64_t)iZoneID)<<32;
    ullBusID += ((uint64_t)enServerID)<<16;
    ullBusID += iInstance;

    return ullBusID;
};

//todo jasonxiong ���������ÿ�����������涼Ҫ���¶����,��Ҫʹ������
const char* GetZmqBusAddress(uint64_t ullClientBusID, uint64_t ullServerBusID);

// ��ȡ��Դ·��
extern int GetCommonResourcePath(char* pszPathBuffer, const int iPathLen, const char* pszResName);

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��̬�ڴ����ʹ������

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��̬�ڴ����ʹ������

#ifdef _DEBUG_

//////////////////////////////////////////
// Zone
/////////////////////////////////////////

// ��λID
const int MAX_UNIT_NUMBER_IN_ZONE = 10000;

// ��ɫ����
const int MAX_ROLE_OBJECT_NUMBER_IN_ZONE = 100;

//��Ϸ��ս����λ����
const int MAX_FIGHT_UNIT_NUMBER_IN_ZONE = 100 * MAX_UNIT_NUMBER_IN_ZONE;

//��Ϸս��ս������
const int MAX_BATTLEFIELD_OBJECT_NUMBER_IN_ZONE = MAX_ROLE_OBJECT_NUMBER_IN_ZONE;

//��Ϸս��ʱ��̬ս����λ����
const int MAX_COMBAT_UNIT_OBJECT_NUMBER_IN_ZONE = MAX_BATTLEFIELD_OBJECT_NUMBER_IN_ZONE * 10;

//��Ϸս��ʱBuff����
const int MAX_COMBAT_BUFF_OBJECT_NUMBER_IN_ZONE = MAX_BATTLEFIELD_OBJECT_NUMBER_IN_ZONE * 15;

const int MAX_COMBAT_TRAP_OBJECT_NUMBER_IN_ZONE = MAX_BATTLEFIELD_OBJECT_NUMBER_IN_ZONE * 8;

//////////////////////////////////////////
// World
/////////////////////////////////////////
const int MAX_REGISTER_ROLE_NUMBER_PER_WORLD = 10000;

/////////////////////////////////////////
//��HashMap�ڵ��С
const int MAX_ROLE_NAME_NUMBER = 10000;
const int MAX_GUILD_NAME_NUMBER = 10000;
//Ԥ����ڵ��С
const int MAX_PRE_CACHE_SIZE = 10000;
//���ݻ���ڵ��С
const int MAX_ROLE_BACKUP_CACHE_SIZE = 10000;
const int MAX_GUILD_BACKUP_CACHE_SIZE = 100;
//ɾ������ڵ��С
const int MAX_ROLE_DELETE_CACHE_SIZE = 10000;
const int MAX_GUILD_DELETE_CACHE_SIZE = 100;

const int MAX_ROLE_OBJECT_NUMBER_IN_WORLD = 6000;

//////////////////////////////////////////
// Cluster
/////////////////////////////////////////
const int MAX_ROLE_OBJECT_NUMBER_IN_CLUSTER = 1000;

//////////////////////////////////////////
// Register & Auth
/////////////////////////////////////////
const int MAX_REGISTER_ACCOUNT_REQUEST_NUMBER = 100000;
const int MAX_ACCOUNT_OBJ_CACHE_NUMBER = 1000;

#else
//////////////////////////////////////////
// Zone
/////////////////////////////////////////

// ��ɫ����
const int MAX_ROLE_OBJECT_NUMBER_IN_ZONE = 4000;

// ��λID
const int MAX_UNIT_NUMBER_IN_ZONE = (MAX_ROLE_OBJECT_NUMBER_IN_ZONE) * 2;

//��Ϸ��ս����λ����
const int MAX_FIGHT_UNIT_NUMBER_IN_ZONE = 100 * MAX_UNIT_NUMBER_IN_ZONE;

//��Ϸս��ս������
const int MAX_BATTLEFIELD_OBJECT_NUMBER_IN_ZONE = MAX_ROLE_OBJECT_NUMBER_IN_ZONE;

//��Ϸս��ʱ��̬ս����λ����
const int MAX_COMBAT_UNIT_OBJECT_NUMBER_IN_ZONE = MAX_BATTLEFIELD_OBJECT_NUMBER_IN_ZONE * 10;

//��Ϸս��ʱBuff����
const int MAX_COMBAT_BUFF_OBJECT_NUMBER_IN_ZONE = MAX_BATTLEFIELD_OBJECT_NUMBER_IN_ZONE * 15;

const int MAX_COMBAT_TRAP_OBJECT_NUMBER_IN_ZONE = MAX_BATTLEFIELD_OBJECT_NUMBER_IN_ZONE * 8;

//////////////////////////////////////////
// World
/////////////////////////////////////////
const int MAX_ZONE_NUMBER = 3;
const int MAX_REGISTER_ROLE_NUMBER_PER_WORLD = 1000000;
const int MAX_TEAM_OBJECT_NUMBER = 4000;
const int MAX_TEAM_TRANSACTION_OBJECT_NUMBER = 4000;
const int MAX_ROLE_OBJECT_NUMBER_IN_WORLD = 6000;
const int MAX_GUILD_NUMBER_PER_WORLD = 3000;
const int MAX_GUILD_INVITE_REQUEST_NUMBER = 1000;
const int MAX_GUILD_APPLY_OBJECT_NUMBER = 6000;
const int MAX_ROLE_GUILD_MAPPING_NUMBER = 250000;

//////////////////////////////////////////
// Name
///////////////////////////////////////
//��HashMap�ڵ��С
const int MAX_ROLE_NAME_NUMBER = 100000;
const int MAX_GUILD_NAME_NUMBER = 5000;

//Ԥ����ڵ��С
const int MAX_PRE_CACHE_SIZE = 10000;

//���ݻ���ڵ��С
const int MAX_ROLE_BACKUP_CACHE_SIZE = 10000;
const int MAX_GUILD_BACKUP_CACHE_SIZE = 5000;

//ɾ������ڵ��С
const int MAX_ROLE_DELETE_CACHE_SIZE = 10000;
const int MAX_GUILD_DELETE_CACHE_SIZE = 5000;

//////////////////////////////////////////
// Cluster
/////////////////////////////////////////
const int MAX_ROLE_OBJECT_NUMBER_IN_CLUSTER = 1000000;

//////////////////////////////////////////
// Register & Auth
/////////////////////////////////////////
const int MAX_REGISTER_ACCOUNT_REQUEST_NUMBER = 10000;

const int MAX_ACCOUNT_OBJ_CACHE_NUMBER = 1000000;

#endif

//////////////////////////////////////////////////////////////////////////////////////////////////////////////

// �ڲ��ʺ�
const unsigned int MAX_TEST_UIN = 10000;

// Zoneά�� ����ɫ��������д���ݿ�ʱ�������룩
const int ROLEDATA_REWRITE_SECONDS = 10;

#ifdef _DEBUG_
const int SERVER_PRESTOP_TIME = 0;
#else
const int SERVER_PRESTOP_TIME = 20;
#endif

typedef struct MiniTdrResHead
{
    int32_t iMagic;             //Magic Number of Resouce File
    int32_t iVersion;           //Version of Resouce File
    int32_t iUnit;              //size of each resouce calced by byte
    int32_t iCount;             //total count number of resouces
} TRESHEAD;

template <typename T, int N>
int LoadTemplateCfg(const char * pCfgFilePath, T (&astTArrar)[N], int *piNum, bool (*IfPred)(const void *))
{
    ASSERT_AND_LOG_RTN_INT(pCfgFilePath);
    ASSERT_AND_LOG_RTN_INT(piNum);

    char szResPath[256] = {0};

    *piNum = 0;
    memset(astTArrar, 0, sizeof(astTArrar));

    // ����������Ϣ
    GetCommonResourcePath(szResPath, sizeof(szResPath), pCfgFilePath);

    //�������ļ�
    FILE* pFile = fopen(szResPath, "rb");
    if(!pFile)
    {
        TRACESVR("Fail to open config file %s\n", szResPath);
        return -1;
    }

    fseek(pFile, 0L, SEEK_END);     //��λ���ļ�ĩβ
    int iFileLen = ftell(pFile);    //��ȡ�ļ���С
    fseek(pFile, 0L, SEEK_SET);     //��λ���ļ�ͷ

    char* pBuffer = (char*)malloc(iFileLen+1);
    fread(pBuffer, iFileLen, 1, pFile);
    pBuffer[iFileLen] = '\0';

    //�Ƚ���ͷ
    TRESHEAD* pstHead = (TRESHEAD*)pBuffer;
    if(pstHead->iUnit > (int)sizeof(T))
    {
        free(pBuffer);
        fclose(pFile);
        TRACESVR("Invalid Unit Size: version not matched, %d %ld\n", pstHead->iUnit, sizeof(T));
        return -2;
    }

    //�ж����õ�Ԫ�ĸ����Ƿ��Ѿ������������õĸ���
    if (pstHead->iCount > N)
    {
        TRACESVR("Too many Configs: %d:%d\n", pstHead->iCount, N);

        free(pBuffer);
        fclose(pFile);
        return -5;
    }
    
    char* pszResUnit = pBuffer + 136;   //ͷ�ĳ��ȹ̶�Ϊ136���ֽ�

    memcpy((void*)astTArrar, (void*)pszResUnit, pstHead->iCount*sizeof(T));

    *piNum = pstHead->iCount;

    free(pBuffer);

    if (IfPred)
    {
        MyDeleteArray_If((const void *)astTArrar, &pstHead->iCount, sizeof(T), IfPred);
    }

    fclose(pFile);

    TRACESVR("Load %s array: num = %d\n", typeid(T).name(), *piNum);

    return 0;
}

#endif

