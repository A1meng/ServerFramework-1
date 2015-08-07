#ifndef __GM_COMBAT_HANDLER_HPP__
#define __GM_COMBAT_HANDLER_HPP__

#include "GMBaseCommand.hpp"

class CGameRoleObj;
class CGMCombatHandler : public IGMBaseCommand
{
public:
    CGMCombatHandler();
    ~CGMCombatHandler() { };

public:
    //ִ����ӦGM����Ĺ���
    virtual int Run(CGameRoleObj *pRoleObj, int iCommandType, std::vector<std::string>& vParams);

protected:
    //���в������
    virtual int SecurityCheck(int iCommandType, std::vector<std::string>& vParams);

private:
    CGameRoleObj* m_pRoleObj;
};

#endif
