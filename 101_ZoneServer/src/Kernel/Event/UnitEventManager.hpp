
#ifndef __UNIT_EVENT_MANAGER_HPP__
#define __UNIT_EVENT_MANAGER_HPP__

#include "GameProtocol.hpp"
#include "ZoneErrorNumDef.hpp"
#include "GameObjCommDef.hpp"
#include "GameRole.hpp"
#include "SessionDefine.hpp"

//����������¼�֪ͨ��������

class CUnitEventManager
{
public:
    void NotifyUnitLogin(TUNITINFO* pUnit);
    void NotifyUnitLogout(TUNITINFO* pUnit);

    void NotifyCombatEnd(CGameRoleObj& stRoleObj, bool bIsWin);

    void NotifyTick();
};

#endif
