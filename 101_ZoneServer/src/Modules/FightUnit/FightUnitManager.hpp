#ifndef __FIGHT_UNIT_MANAGER_HPP__
#define __FIGHT_UNIT_MANAGER_HPP__

//���ս����λ������

#include "GameConfigDefine.hpp"
#include "GameProtocol.hpp"

using namespace GameConfig;

class CFightUnitObj;
class CGameRoleObj;

class CFightUnitManager
{
public:
    CFightUnitManager();
    ~CFightUnitManager();

public:

    //��ʼ�� FightUnitManager
    int Initialize();

    void SetOwner(unsigned int uin);
    unsigned int GetOwner();

    //��ȡս����λ��Ϣ
    CFightUnitObj* GetFightUnitByID(int iFightUnitID);

    //ս������������ս����λ
    int AddFightUnit(int iFightUnitID);

    //��ȡ����ս����λ��Ϣ
    void GetFormFightUnitInfo(std::vector<int>& vFormUnitInfo);

    int InitFightUnitInfoFromDB(const FIGHTDBINFO& rstFightUnitInfo);
    int UpdateFightUnitInfoToDB(FIGHTDBINFO& rstFightUnitInfo);

    //����ڴ��е�FightUnitObj
    void ClearFightUnitObj();

private:

    //��ҽ�ɫ��uin
    unsigned int m_uiUin;

    //������е�ս����λ����Ϣ
    int m_iFightUnitNum;
    int m_aiUnitObjID[MAX_ROLE_FIGHTUNIT_NUM];

    //���������Ϣ
    int m_aiFightForm[MAX_FIGHTUNIT_ON_FORM];

    static GameProtocolMsg ms_stZoneMsg; 
};

#endif
