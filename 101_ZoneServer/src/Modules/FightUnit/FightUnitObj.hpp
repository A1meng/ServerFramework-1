#ifndef __FIGHT_UNIT_OBJ_HPP__
#define __FIGHT_UNIT_OBJ_HPP__

#include "GameProtocol.hpp"
#include "GameConfigDefine.hpp"
#include "ZoneObjectAllocator.hpp"
#include "RepThingsManager.hpp"

//���ս����λ����

class CFightUnitObj : public CObj
{
public:
    CFightUnitObj();
    virtual ~CFightUnitObj();
    virtual int Initialize();

    void SetOwner(unsigned int uin);
    unsigned GetOwner();

    DECLARE_DYN

public:

    //��ȡս����λ��ID
    int GetFightUnitID() { return m_iConfigID; };

    //װж��Ʒ, bIsEquip = true ��ʾװ��Ʒ��= false ��ʾж��Ʒ
    int UnitEquipItem(int iSlot, RepItem& stItem, bool bIsEquip);

    //����λ�û�ȡ��Ʒ��Ϣ
    RepItem* GetItemInfo(int iSlot);

    //����ID��ʼ��ս����λ��Ϣ
    int InitFightUnitByID(int iFightUnitID);

    //��ʼ��ս�����������
    int InitFightAttr(int* aiAttribute, int iAttrMaxNum);

    //��ʼ��ս���������Ʒ
    void InitFightUnitItem(RepItem* pstItems, int iItemMaxNum);

    //��ȡ�չ�ID
    int GetNormalSkill();

    //ս����λ��AI��Ϣ
    int GetFightUnitAIID();

    //ս����λ����
    int GetFightUnitAttr(int iAttr);
    int AddFightUnitAttr(int iAttr, int iAddAttrNum);
    int AddUnitAttrWithoutNotify(int iAttr, int iAddAttrNum);

	//�ȼ�
	inline int GetLevel(){ return m_iLevel;}

	//����
	inline int GetExp(){return m_iLevelExp;}

public:
    
    //���ݿ��������
    void InitFightUnitFromDB(const OneFightUnitInfo& stUnitInfo);
    void UpdateFightUnitToDB(OneFightUnitInfo& stUnitInfo); 

///////////////////////////////////////////////////////////////////////
public:

    //��ҽ�ɫ��uin
    unsigned int m_uiUin;

    //ս����λ����ID
    int m_iConfigID;

    //���ս����λ��AI ID
    int m_iFightUnitAIID;

	//�ȼ�
	int m_iLevel;

	//����
	int m_iLevelExp;

    //ս����λ������
    int m_aiUnitAttribute[MAX_FIGHT_UNIT_ATTRIBUTE_NUM];

    //ս����λ���չ�
    int m_iNormalSkill;

    //ս����λ��Ʒ��Ϣ
    int m_iSlotNum;
    RepItem m_astItems[MAX_UNIT_ITEM_SLOT];
};

#endif
