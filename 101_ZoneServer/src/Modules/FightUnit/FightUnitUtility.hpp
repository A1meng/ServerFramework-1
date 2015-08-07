#ifndef __FIGHT_UNIT_UTILITY_HPP__
#define __FIGHT_UNIT_UTILITY_HPP__

class CGameRoleObj;
class CFightUnitUtility
{
public:
    
    //��ȡս����λ����
    static int GetFightUnitAttr(CGameRoleObj& rstRoleObj, int iFightUnitID, int iAttrType);

    //����ս����λ����
    static int AddFightUnitAttr(CGameRoleObj& rstRoleObj, int iFightUnitID, int iAttrType, int iAddAttrNum);

    //����ս����λ������
    static int SaveCombatUnitAttr(CGameRoleObj& stRoleObj, int iFightUnitID, int iAttrType, int iSaveNum);
};

#endif
