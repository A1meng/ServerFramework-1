#ifndef __GM_BASE_COMMAND_HPP__
#define __GM_BASE_COMMAND_HPP__

//����GM����ܴ���Ļ���
#include <vector>
#include <string>

#include "GameProtocol.hpp"

enum GMCommandType
{
    GM_COMMAND_TYPE_INVALID         = 0,        //�Ƿ���GM command ����
    GM_COMMAND_ADD_REPITEM          = 1,        //����������Ʒ
    GM_COMMAND_ADD_FIGHTUNIT        = 2,        //������ҵ�ս����λ
    GM_COMMAND_ADD_UNITATTR         = 5,        //����ս����λ������
    GM_COMMAND_ADD_COMBATUNIT       = 13,       //����ս���ϵ�ս����λ
    GM_COMMAND_ADD_SKILL            = 14,       //����ָ����ʽ
};

class CGameRoleObj;

class IGMBaseCommand
{
public:
    virtual ~IGMBaseCommand();

    //ִ����ӦGM����Ĺ���
    virtual int Run(CGameRoleObj* pRoleObj, int iCommandType, std::vector<std::string>& vParams) = 0;

protected:

    //���в������
    virtual int SecurityCheck(int iCommandType, std::vector<std::string>& vParams) = 0;

protected:
    static GameProtocolMsg m_stGameMsg;
};

#endif
