#ifndef __WORLD_ERROR_NUM_DEF_HPP__
#define __WORLD_ERROR_NUM_DEF_HPP__

#include "ErrorNumDef.hpp"

enum EN_WORLD_MODULE_DEF
{
    EN_WORLD_MODULE_COMMON = 0x001,               //ͨ��ģ�������
};

#define TWORLD_COMMON_BASE MAKE_ERRNO(EN_SERVER_WORLD, EN_WORLD_MODULE_COMMON, 0)
enum WORLD_SYSTEM_ERR_DEF
{
    T_WORLD_SYSTEM_PARA_ERR         = TWORLD_COMMON_BASE + 0x01,        //��������
    T_WORLD_CANNOT_CREATE_ROLE      = TWORLD_COMMON_BASE + 0x02,        //World Server���ܴ�����ɫ
    T_WORLD_SERVER_BUSY             = TWORLD_COMMON_BASE + 0x03,        //World������æ
    T_WORLD_SESSION_EXIST           = TWORLD_COMMON_BASE + 0x04,        //session�Ѿ�����  
    T_WORLD_FETCHROLE_FAIED         = TWORLD_COMMON_BASE + 0x05,        //��ȡ��ɫ������Ϣʧ�� 
    T_WORLD_SYSYTEM_INVALID_CFG     = TWORLD_COMMON_BASE + 0x06,        //��������
};

#endif
