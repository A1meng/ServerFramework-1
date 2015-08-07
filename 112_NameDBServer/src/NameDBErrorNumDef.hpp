#ifndef __NAMEDB_ERROR_NUM_DEF_HPP__
#define __NAMEDB_ERROR_NUM_DEF_HPP__

#include "ErrorNumDef.hpp"

enum EN_NAMEDB_MODULE_DEF
{
    EN_NAMEDB_MODULE_COMMON = 0x001,               //ͨ��ģ�������
};

#define TNAMEDB_COMMON_BASE MAKE_ERRNO(EN_SERVER_NAMEDB, EN_NAMEDB_MODULE_COMMON, 0)
enum NAMEDB_SYSTEM_ERR_DEF
{
    T_NAMEDB_SYSTEM_PARA_ERR         = TNAMEDB_COMMON_BASE + 0x01,        //��������
    T_NAMEDB_NAME_EXISTS             = TNAMEDB_COMMON_BASE + 0x02,        //Ҫ�������ʺ��Ѿ�����
    T_NAMEDB_SQL_EXECUTE_FAILED      = TNAMEDB_COMMON_BASE + 0x03,        //SQL���ִ��ʧ��
    T_NAMEDB_PROTOBUFF_ERR           = TNAMEDB_COMMON_BASE + 0x04,        //protobuf ��ش���
    T_NAMEDB_INVALID_CONFIG          = TNAMEDB_COMMON_BASE + 0x05,        //�Ƿ�������
    T_NAMEDB_INVALID_RECORD          = TNAMEDB_COMMON_BASE + 0x06,        //�Ƿ������ݿ��¼   
    T_NAMEDB_PASSWD_NOT_MATCH        = TNAMEDB_COMMON_BASE + 0x08,        //�ʺ��������
};

#endif
