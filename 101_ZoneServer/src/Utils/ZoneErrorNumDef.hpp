#ifndef __ZONE_ERROR_NUM_DEF_HPP__
#define __ZONE_ERROR_NUM_DEF_HPP__

#include "ErrorNumDef.hpp"

enum EN_ZONE_MODULE_DEF
{
    EN_ZONE_MODULE_COMMON = 0x001,               //ͨ��ģ�������
};

#define TZONE_COMMON_BASE MAKE_ERRNO(EN_SERVER_ZONE, EN_ZONE_MODULE_COMMON, 0)
enum ZONE_SYSTEM_ERR_DEF
{
    T_ZONE_SYSTEM_PARA_ERR         = TZONE_COMMON_BASE + 0x01,        //��������
    T_ZONE_PROTOBUFF_ERR           = TZONE_COMMON_BASE + 0x02,        //protobuf ����
    T_ZONE_LOGINSERVER_FAILED      = TZONE_COMMON_BASE + 0x03,        //��¼������������
    T_ZONE_LOGOUT_KICKOFF          = TZONE_COMMON_BASE + 0x04,        //�������ߴ���
    T_ZONE_SESSION_EXISTS_ERR      = TZONE_COMMON_BASE + 0x05,        //��ҵ�Session�Ѿ�����
    T_ZONE_LOGOUT_UNACTIVE         = TZONE_COMMON_BASE + 0x06,        //���Ѿ�����Ծ���������
    T_ZONE_FIN_PVEFIGHT_FAILED     = TZONE_COMMON_BASE + 0x07,        //���PVE�ؿ�ս��ʧ��
    T_ZONE_SYSTEM_INVALID_CFG      = TZONE_COMMON_BASE + 0x08,        //�Ƿ�������

    T_ZONE_GAMEROLE_NOT_EXIST      = TZONE_COMMON_BASE + 0x09,        //��ҽ�ɫ��Ϣ������
    T_ZONE_GAMEHERO_NOT_EXIST      = TZONE_COMMON_BASE + 0x0a,        //������Ϣ������
                                                                      
    T_ZONE_SECURITY_CHECK_FAILED   = TZONE_COMMON_BASE + 0x0b,        //���������ȫУ�����
    T_ZONE_GM_COMMAND_FAILED       = TZONE_COMMON_BASE + 0x0c,        //���������ȫУ�����

    T_ZONE_USER_REP_FULL           = TZONE_COMMON_BASE + 0x0d,        //��ұ����ֿ���
    T_ZONE_INVALID_EQUIP_TYPE      = TZONE_COMMON_BASE + 0x0e,        //�Ƿ���װ������
    T_ZONE_REACH_LEVEL_MAX         = TZONE_COMMON_BASE + 0x0f,        //�ﵽ�ȼ�����

    T_ZONE_PVE_KILLMONSTER_FAILED  = TZONE_COMMON_BASE + 0x10,        //PVE��ɱ����ʧ��
    T_ZONE_INVALID_PVE_CROSS       = TZONE_COMMON_BASE + 0x11,        //�Ƿ���PVEս���ؿ�

    T_ZONE_INVALID_EQUIPMENT       = TZONE_COMMON_BASE + 0x12,        //�Ƿ���װ��

    T_ZONE_REVIVAL_HERO_FAILED     = TZONE_COMMON_BASE + 0x13,        //�ؿ�Ӣ�۸���ʧ��

    T_ZONE_INVALID_PROPS_NUM       = TZONE_COMMON_BASE + 0x14,        //��Ϸ���߷Ƿ�������
};

#endif
