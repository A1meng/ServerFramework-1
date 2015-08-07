#ifndef __GAME_OBJ_COMM_DEF_HPP__
#define __GAME_OBJ_COMM_DEF_HPP__

//���ļ�����Ҫ������Ϸ��ʹ�õĵ�λ�ͽ�ɫ�Ļ������ݽṹ

#include "GameProtocol.hpp"
#include "GameConfigDefine.hpp"

using namespace GameConfig;

//�����ϵ�λ�Ķ�����Ϣ
struct TUNITINFO
{
    int iUnitID;                    //������λ��ID
    unsigned char ucUnitType;       //������λ������
    unsigned int uiUnitStatus;      //������λ��ǰ��״̬

    TUNITINFO()
    {
        memset(this, 0, sizeof(*this));
    };
};

//��ҽ�ɫ������Ϣ�ṹ����
struct TROLEBASEINFO
{
    char szNickName[MAX_NICK_NAME_LENGTH];  //��ҵ�����
    int iLastLogin;             //����ϴε�¼��ʱ��
    int iLastLogout;            //����ϴεǳ���Ϸ��ʱ��
    int iCreateTime;            //����ʺŴ�����ʱ��
    int iOnlineTime;            //��ҵ�������ʱ��
    int iLoginCount;            //����ܵĵ�¼����
    int iForbidTalkingTime;     //������챻��ֹ���Ե�ʱ��
    int iLoginTime;             //��ұ��ε�¼��ʱ��
    int iLogoutTime;            //��ұ��εǳ���Ϸ��ʱ��
    int iBattlefieldObjIndex;   //��ҵ�ս��ս����Ϣ
    bool bIsSelfWin;            //��ҽ���ս��ʤ�����

    TROLEBASEINFO()
    {
        memset(this, 0, sizeof(*this));
    };
};

//��ɫ��������Ϣ
struct TROLEINFO
{
    RoleID stRoleID;            //��ɫID
    TROLEBASEINFO stBaseInfo;   //��ɫ�Ļ�����Ϣ
    TUNITINFO stUnitInfo;       //��ɫ��Unit��λ��Ϣ
};

#endif
