
#include "GameProtocol.hpp"
#include "ZoneErrorNumDef.hpp"
#include "GameRole.hpp"
#include "FightUnitManager.hpp"
#include "FightUnitObj.hpp"
#include "ModuleShortcut.hpp"
#include "ZoneMsgHelper.hpp"

#include "FightUnitUtility.hpp"

//��ȡս����λ����
int CFightUnitUtility::GetFightUnitAttr(CGameRoleObj& rstRoleObj, int iFightUnitID, int iAttrType)
{
    //��ȡս����λ������
    CFightUnitManager& stUnitManager = rstRoleObj.GetFightUnitManager();

    //��ȡս����λ
    CFightUnitObj* pstUnitObj = stUnitManager.GetFightUnitByID(iFightUnitID);
    if(!pstUnitObj)
    {
        return 0;
    }

    return pstUnitObj->GetFightUnitAttr(iAttrType);
}

//����ս����λ����
int CFightUnitUtility::AddFightUnitAttr(CGameRoleObj& rstRoleObj, int iFightUnitID, int iAttrType, int iAddAttrNum)
{
    //��ȡս����λ������
    CFightUnitManager& stUnitManager = rstRoleObj.GetFightUnitManager();

    //��ȡս����λ
    CFightUnitObj* pstUnitObj = stUnitManager.GetFightUnitByID(iFightUnitID);
    if(!pstUnitObj)
    {
        return T_ZONE_SYSTEM_PARA_ERR;
    }

    return pstUnitObj->AddFightUnitAttr(iAttrType, iAddAttrNum);
}

//����ս����λ������
int CFightUnitUtility::SaveCombatUnitAttr(CGameRoleObj& stRoleObj, int iFightUnitID, int iAttrType, int iSaveNum)
{
    CFightUnitManager& stFightUnitMgr = stRoleObj.GetFightUnitManager();
    CFightUnitObj* pstUnitObj = stFightUnitMgr.GetFightUnitByID(iFightUnitID);
    if(!pstUnitObj)
    {
        return T_ZONE_SYSTEM_PARA_ERR;
    }

    //����Ϊ1��
    iSaveNum = iSaveNum<=0 ? 1 : iSaveNum;

    pstUnitObj->AddFightUnitAttr(iAttrType, iSaveNum-pstUnitObj->GetFightUnitAttr(iAttrType));

    return T_SERVER_SUCESS;
}

