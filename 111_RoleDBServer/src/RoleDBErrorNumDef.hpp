#ifndef __ROLEDB_ERROR_NUM_DEF_HPP__
#define __ROLEDB_ERROR_NUM_DEF_HPP__

#include "ErrorNumDef.hpp"

enum EN_ROLEDB_MODULE_DEF
{
    EN_ROLEDB_MODULE_COMMON = 0x001,               //ͨ��ģ�������
};

#define TROLEDB_COMMON_BASE MAKE_ERRNO(EN_SERVER_ROLEDB, EN_ROLEDB_MODULE_COMMON, 0)
enum ROLEDB_SYSTEM_ERR_DEF
{
    T_ROLEDB_SYSTEM_PARA_ERR         = TROLEDB_COMMON_BASE + 0x01,        //��������
    T_ROLEDB_CANNOT_CREATE_ROLE      = TROLEDB_COMMON_BASE + 0x02,        //���ܴ�����ɫ
    T_ROLEDB_SQL_EXECUTE_FAILED      = TROLEDB_COMMON_BASE + 0x03,        //SQL���ִ��ʧ��
    T_ROLEDB_PROTOBUFF_ERR           = TROLEDB_COMMON_BASE + 0x04,        //protobuf ��ش���
    T_ROLEDB_INVALID_RECORD           = TROLEDB_COMMON_BASE + 0x04,        //�Ƿ������ݿ��ѯ��¼
};

#endif
