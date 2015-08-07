#ifndef __ACCOUNTDB_ERROR_NUM_DEF_HPP__
#define __ACCOUNTDB_ERROR_NUM_DEF_HPP__

#include "ErrorNumDef.hpp"

enum EN_ACCOUNTDB_MODULE_DEF
{
    EN_ACCOUNTDB_MODULE_COMMON = 0x001,               //ͨ��ģ�������
};

#define TACCOUNTDB_COMMON_BASE MAKE_ERRNO(EN_SERVER_ACCOUNTDB, EN_ACCOUNTDB_MODULE_COMMON, 0)
enum ACCOUNTDB_SYSTEM_ERR_DEF
{
    T_ACCOUNTDB_SYSTEM_PARA_ERR         = TACCOUNTDB_COMMON_BASE + 0x01,        //��������
    T_ACCOUNTDB_ACCOUNT_EXISTS          = TACCOUNTDB_COMMON_BASE + 0x02,        //Ҫ�������ʺ��Ѿ�����
    T_ACCOUNTDB_SQL_EXECUTE_FAILED      = TACCOUNTDB_COMMON_BASE + 0x03,        //SQL���ִ��ʧ��
    T_ACCOUNTDB_PROTOBUFF_ERR           = TACCOUNTDB_COMMON_BASE + 0x04,        //protobuf ��ش���
    T_ACCOUNTDB_INVALID_CONFIG          = TACCOUNTDB_COMMON_BASE + 0x05,        //�Ƿ�������
    T_ACCOUNTDB_INVALID_RECORD          = TACCOUNTDB_COMMON_BASE + 0x06,        //�Ƿ������ݿ��¼   
    T_ACCOUNTDB_AUTH_FAILED             = TACCOUNTDB_COMMON_BASE + 0x07,        //�ʺ���֤ʧ��   
    T_ACCOUNTDB_PASSWD_NOT_MATCH        = TACCOUNTDB_COMMON_BASE + 0x08,        //�ʺ��������
};

#endif
