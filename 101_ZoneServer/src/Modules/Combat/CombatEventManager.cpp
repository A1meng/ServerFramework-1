
#include "GameRole.hpp"
#include "CombatEventListener.hpp"

#include "CombatEventManager.hpp"

CCombatEventListener CCombatEventManager::m_stCombatEventListener;

//ս����ʼ�¼�
void CCombatEventManager::NotifyCombatBegin(CGameRoleObj& stRoleObj)
{
    m_stCombatEventListener.OnCombatBeginEvent(stRoleObj);
}

//ս�������¼�
void CCombatEventManager::NotifyCombatEnd(CGameRoleObj& stRoleObj)
{
    m_stCombatEventListener.OnCombatEndEvent(stRoleObj);
}

//�غϿ�ʼ�¼�
void CCombatEventManager::NotifyRoundBegin(CGameRoleObj& stRoleObj)
{
    m_stCombatEventListener.OnRoundBeginEvent(stRoleObj);
}

//�غϽ����¼�
void CCombatEventManager::NotifyRoundEnd(CGameRoleObj& stRoleObj)
{
    m_stCombatEventListener.OnRoundEndEvent(stRoleObj);
}

//ս����λ�ж��غϿ�ʼ�¼�
void CCombatEventManager::NotifyActionRoundBegin(CGameRoleObj& stRoleObj)
{
    m_stCombatEventListener.OnActionRoundBeginEvent(stRoleObj);
}

//ս����λ�ж��غϽ����¼�
void CCombatEventManager::NotifyActionRoundEnd(CGameRoleObj& stRoleObj)
{
    m_stCombatEventListener.OnActionRoundEndEvent(stRoleObj);
}

//ս����λ�ƶ����¼�
void CCombatEventManager::NotifyCombatUnitMove(CGameRoleObj& stRoleObj)
{
    m_stCombatEventListener.OnCombatUnitMoveEvent(stRoleObj);
}

//ս����λ�ж����¼�
void CCombatEventManager::NotifyCombatUnitAction(CGameRoleObj& stRoleObj)
{
    m_stCombatEventListener.OnCombatUnitActionEvent(stRoleObj);
}

