#ifndef __ACCOUNT_ERROR_NUM_DEF_HPP__
#define __ACCOUNT_ERROR_NUM_DEF_HPP__

#include "ErrorNumDef.hpp"

enum EN_ACCOUNT_MODULE_DEF
{
    EN_ACCOUNT_MODULE_COMMON = 0x001,               //ͨ��ģ�������
};

#define TACCOUNT_COMMON_BASE MAKE_ERRNO(EN_SERVER_ACCOUNT, EN_ACCOUNT_MODULE_COMMON, 0)
enum ACCOUNT_SYSTEM_ERR_DEF
{
    T_ACCOUNT_SYSTEM_PARA_ERR        = TACCOUNT_COMMON_BASE + 0x01,        //��������
    T_ACCOUNT_CANNOT_CREATE_ROLE     = TACCOUNT_COMMON_BASE + 0x02,        //Account Server���ܴ�����ɫ
    T_ACCOUNT_ROLE_NUMLIMIT          = TACCOUNT_COMMON_BASE + 0x03,         //�����Ľ�ɫ��Ŀ�Ѿ��ﵽϵͳ����
    T_ACCOUNT_SERVER_BUSY            = TACCOUNT_COMMON_BASE + 0x04,      //Account������æ
};

#endif
