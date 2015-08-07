
#include "UnitEventManager.hpp"
#include "ModuleHelper.hpp"
#include "GameProtocol.hpp"
#include "ZoneObjectHelper.hpp"
#include "GameSession.hpp"
#include "GameRole.hpp"
#include "LogAdapter.hpp"
#include "UnitUtility.hpp"
#include "ZoneMsgHelper.hpp"

void CUnitEventManager::NotifyUnitLogin(TUNITINFO* pUnit)
{

}

void CUnitEventManager::NotifyUnitLogout(TUNITINFO* pUnit)
{

}

void CUnitEventManager::NotifyTick()
{
    // ����ϵͳϵͳ����æ״̬, ��̬����ϵͳ����
    EGameServerStatus enServerStatus = GetServerStatus();

    int iNumber = (enServerStatus == GAME_SERVER_STATUS_BUSY) ? 2 : 200;
    for (int i = 0; i < iNumber; i++)
    {
        int iRoleIdx = CUnitUtility::IterateRoleIdx();
        if (iRoleIdx < 0)
        {
            break;
        }

        CGameRoleObj* pRoleObj = GameTypeK32<CGameRoleObj>::GetByIdx(iRoleIdx);
        if (!pRoleObj)
        {
            continue;
        }

        TUNITINFO* pUnitInfo = &pRoleObj->GetRoleInfo().stUnitInfo;
        // ����ɾ����λ
        if (CUnitUtility::IsUnitStatusSet(pUnitInfo, EGUS_DELETE))
        {
            CUnitUtility::DeleteUnit(pUnitInfo);
            continue;
        }

        // ��ɫTick
        pRoleObj->OnTick();
    }

    return;
}

void CUnitEventManager::NotifyCombatEnd(CGameRoleObj& stRoleObj, bool bIsWin)
{

}

