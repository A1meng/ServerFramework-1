#ifndef __COMBAT_REWARD_HPP__
#define __COMBAT_REWARD_HPP__

//ս�������Ĵ���

#include "GameProtocol.hpp"

class CGameRoleObj;
class CCombatReward
{
public:
    
    //��ȡս������
    static int GetCombatReward(int iLevelID, CGameRoleObj& stRoleObj, CombatReward& stReward);
};

#endif
