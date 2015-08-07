
#ifndef __COMBAT_EVENT_MANAGER_HPP__
#define __COMBAT_EVENT_MANAGER_HPP__

#include "CombatEventListener.hpp"

class CGameRoleObj;

//ս���¼�������
class CCombatEventManager
{
public:

    //ս����ʼ�¼�
    static void NotifyCombatBegin(CGameRoleObj& stRoleObj);

    //ս�������¼�
    static void NotifyCombatEnd(CGameRoleObj& stRoleObj);

    //�غϿ�ʼ�¼�
    static void NotifyRoundBegin(CGameRoleObj& stRoleObj);

    //�غϽ����¼�
    static void NotifyRoundEnd(CGameRoleObj& stRoleObj);

    //ս����λ�ж��غϿ�ʼ�¼�
    static void NotifyActionRoundBegin(CGameRoleObj& stRoleObj);

    //ս����λ�ж��غϽ����¼�
    static void NotifyActionRoundEnd(CGameRoleObj& stRoleObj);

    //ս����λ�ƶ����¼�
    static void NotifyCombatUnitMove(CGameRoleObj& stRoleObj);

    //ս����λ�ж����¼�
    static void NotifyCombatUnitAction(CGameRoleObj& stRoleObj);

private:
    static CCombatEventListener m_stCombatEventListener;
};
    
#endif
