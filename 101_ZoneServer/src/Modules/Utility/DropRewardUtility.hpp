#ifndef __DROP_REWARD_UTILITY_HPP__
#define __DROP_REWARD_UTILITY_HPP__

//������乤����

#include "GameProtocol.hpp"

class CGameRoleObj;
class CDropRewardUtility
{
public:

    //��ȡ�������
    static int GetDropReward(int iDropRewardID, CGameRoleObj& stRoleObj, CombatReward& stReward, int iItemChannel, int iRandParam = 100);

    //��ȡ�������
    static int GetDropReward(int iDropRewardID, CGameRoleObj& stRoleObj, int iItemChannel, int iRandParam = 100);
};

#endif
