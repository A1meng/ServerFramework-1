#ifndef __ACCOUNT_OSS_LOG_HPP__
#define __ACCOUNT_OSS_LOG_HPP__

///////////////////////////////////////////////////////////////////////////////////// 
#include "LogAdapter.hpp"
#include "GameConfigDefine.hpp"

using namespace ServerLib;

class CGameRoleObj;

enum TAccountOssLogType
{
    OSS_LOG_TYPE_CREATE_ACCOUNT = 50001,    //�����ʺŵ���־
};

//OSS��Ӫϵͳ��־������

class CAccountOssLog
{
public:
    // ��ʼ��
    static int Initialize();

public:
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    //Account��־��ӡ�ӿ�

    //��¼������ɫ�����־
    static void TraceCreateAccount(unsigned int uin, unsigned int uiSessionFd);
};

#endif
