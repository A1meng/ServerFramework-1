
#include "GameProtocol.hpp"
#include "LogAdapter.hpp"
#include "ZoneErrorNumDef.hpp"
#include "ModuleShortcut.hpp"
#include "ZoneMsgHelper.hpp"
#include "UnitUtility.hpp"
#include "ModuleHelper.hpp"
#include "Random.hpp"

#include "FightUnitObj.hpp"

using namespace ServerLib;
using namespace GameConfig;

IMPLEMENT_DYN(CFightUnitObj)

CFightUnitObj::CFightUnitObj()
{
    Initialize();
}

CFightUnitObj::~CFightUnitObj()
{

}

int CFightUnitObj::Initialize()
{
    m_uiUin = 0;

    m_iConfigID = 0;

    m_iFightUnitAIID = 0;

    m_iLevel = 0;

    m_iLevelExp = 0;

    memset(m_aiUnitAttribute, 0, sizeof(m_aiUnitAttribute));

    m_iNormalSkill = 0;

    m_iSlotNum = 0;
    memset(m_astItems, 0, sizeof(m_astItems));

    return T_SERVER_SUCESS;
}

void CFightUnitObj::SetOwner(unsigned int uin)
{
    m_uiUin = uin;
}

unsigned CFightUnitObj::GetOwner()
{
    return m_uiUin;
}

//װж��Ʒ, bIsEquip = true ��ʾװ��Ʒ��= false ��ʾж��Ʒ
int CFightUnitObj::UnitEquipItem(int iSlot, RepItem& stItem, bool bIsEquip)
{
    if(iSlot<0 || iSlot>= m_iSlotNum)
    {
        return T_ZONE_SYSTEM_PARA_ERR;
    }

    if(bIsEquip)
    {
        //��װ��
        memcpy(&m_astItems[iSlot], &stItem, sizeof(stItem));
    }
    else
    {
        //û����Ʒ��ֱ�ӷ���
        if(m_astItems[iSlot].iItemID == 0)
        {
            return T_SERVER_SUCESS;
        }

        //ж��Ʒ����������ж�µ���Ʒ��Ϣ
        memcpy(&stItem, &m_astItems[iSlot], sizeof(m_astItems[iSlot]));
        m_astItems[iSlot].Reset();
    }

    return T_SERVER_SUCESS;
}

//����λ�û�ȡ��Ʒ��Ϣ
RepItem* CFightUnitObj::GetItemInfo(int iSlot)
{
    if(iSlot<0 || iSlot>=m_iSlotNum)
    {
        return NULL;
    }

    return &m_astItems[iSlot];
}

//����ID��ʼ��ս����λ��Ϣ
int CFightUnitObj::InitFightUnitByID(int iFightUnitID)
{
    //��ȡ���õ�ս����λ��Ϣ
    const SFightUnitConfig* pstFightUnitConfig = FightUnitCfgMgr().GetConfig(iFightUnitID);
    if(!pstFightUnitConfig)
    {
        LOGERROR("Failed to get fight unit config, id %d\n", iFightUnitID);
        return T_ZONE_SYSTEM_INVALID_CFG;
    }

    m_iConfigID = iFightUnitID;

    m_iFightUnitAIID = pstFightUnitConfig->aiUnitAIID[0];

    //��ʼ������ֵ
    for(int i=FIGHT_ATTR_HP; i<FIGHT_ATTR_MAX; ++i)
    {
        m_aiUnitAttribute[i] = pstFightUnitConfig->aiAttribute[i];
    }

    m_iLevel = 0;
    m_iLevelExp = 0;

    //��ʼ������
    m_iNormalSkill = pstFightUnitConfig->iNormalSkill;

    //��ʼ��װ������
    m_iSlotNum = pstFightUnitConfig->iInitSlot;

    return T_SERVER_SUCESS;
}

//��ʼ��ս�����������
int CFightUnitObj::InitFightAttr(int* aiAttribute, int iAttrMaxNum)
{
    if(!aiAttribute || iAttrMaxNum!=FIGHT_ATTR_MAX)
    {
        LOGERROR("Failed to init fight attr, param error, uin %u\n", m_uiUin);
        return T_ZONE_SYSTEM_PARA_ERR;
    }

    for(int i=0; i<FIGHT_ATTR_MAX; ++i)
    {
        aiAttribute[i] = m_aiUnitAttribute[i];
    }

    //todo jasonxiong2 װ���Ⱥ���ʵ��
    //����װ��������Ч��
    /*
    for(int i=0; i<EQUIP_SLOT_TYPE_MAX; ++i)
    {
        if(m_astEquips[i].iItemID == 0)
        {
            continue;
        }

        const SEquipmentConfig* pstEquipConfig = EquipmentCfgMgr().GetEquipmentConfig(m_astEquips[i].iItemID);
        if(!pstEquipConfig)
        {
            LOGERROR("Failed to get equipment config, equipment id %d\n", m_astEquips[i].iItemID);
            return T_ZONE_SYSTEM_INVALID_CFG;
        }

        for(int i=0; i<FIGHT_ATTR_MAX; ++i)
        {
            aiAttribute[i] += pstEquipConfig->aiAttribute[i];
        }
    }
    */

    return T_SERVER_SUCESS;
}

//��ʼ��ս���������Ʒ
void CFightUnitObj::InitFightUnitItem(RepItem* pstItems, int iItemMaxNum)
{
    if(!pstItems || iItemMaxNum<0 || iItemMaxNum>=MAX_UNIT_ITEM_SLOT)
    {
        return;
    }

    memcpy(pstItems, m_astItems, sizeof(m_astItems));

    return;
}

//��ȡ�չ�ID
int CFightUnitObj::GetNormalSkill()
{
    return m_iNormalSkill;
}

//ս����λ��AI��Ϣ
int CFightUnitObj::GetFightUnitAIID()
{
    return m_iFightUnitAIID;
}

int CFightUnitObj::GetFightUnitAttr(int iAttr)
{
    if(iAttr < 0 || iAttr >= MAX_FIGHT_UNIT_ATTRIBUTE_NUM)
    {
        return 0;
    }

    return m_aiUnitAttribute[iAttr];
}

int CFightUnitObj::AddFightUnitAttr(int iAttr, int iAddAttrNum)
{
    int iRet = AddUnitAttrWithoutNotify(iAttr, iAddAttrNum);
    if(iRet)
    {
        return iRet;
    }

    //todo jasonxiong4 ���������֪ͨ
    /*
    //���͸��ͻ��˵�֪ͨ
    static GameProtocolMsg stFightAttrNotify;
    CZoneMsgHelper::GenerateMsgHead(stFightAttrNotify, MSGID_ZONE_FIGHTATTR_NOTIFY);

    Zone_FightAttr_Notify* pstNotify = stFightAttrNotify.mutable_m_stmsgbody()->mutable_m_stzone_fightattr_notify();
    pstNotify->set_iunitconfigid(m_iFightUnitID);

    for(int i=0; i<MAX_FIGHT_UNIT_ATTRIBUTE_NUM; ++i)
    {
        pstNotify->add_ifightunitattr(m_aiUnitAttribute[i]);
    }

    CZoneMsgHelper::SendZoneMsgToRole(stFightAttrNotify, CUnitUtility::GetRoleByUin(m_uiUin));
    */

    return T_SERVER_SUCESS;
}

//�������͸��µ���Ϣ
int CFightUnitObj::AddUnitAttrWithoutNotify(int iAttr, int iAddAttrNum)
{
    if(iAttr < 0 || iAttr >= MAX_FIGHT_UNIT_ATTRIBUTE_NUM)
    {
        return T_ZONE_SYSTEM_PARA_ERR;
    }

    if(iAddAttrNum == 0)
    {
        return T_SERVER_SUCESS;
    }

    m_aiUnitAttribute[iAttr] += iAddAttrNum;
    if(m_aiUnitAttribute[iAttr] < 0)
    {
        m_aiUnitAttribute[iAttr] = 0;
    }

    //Ѫ�����ܳ�������
    if(iAttr==FIGHT_ATTR_HP && m_aiUnitAttribute[iAttr]>m_aiUnitAttribute[FIGHT_ATTR_HPMAX])
    {
        m_aiUnitAttribute[iAttr] = m_aiUnitAttribute[FIGHT_ATTR_HPMAX];
    }

    return T_SERVER_SUCESS;
}

//���ݿ��������
void CFightUnitObj::InitFightUnitFromDB(const OneFightUnitInfo& stUnitInfo)
{
    //��ʼ��ID
    m_iConfigID = stUnitInfo.iunitid();

    //��ʼ��AI
    m_iFightUnitAIID = stUnitInfo.iunitaiid();

    //��ʼ���ȼ�
    m_iLevel = stUnitInfo.ilevel();
    m_iLevelExp = stUnitInfo.ilevelexp();

    //��ʼ������
    for(int i=0; i<stUnitInfo.iattributes_size(); ++i)
    {
        m_aiUnitAttribute[i] = stUnitInfo.iattributes(i);
    }

    //��ʼ������
    m_iNormalSkill = stUnitInfo.inormalskillid();

    //��ʼ����Ʒ��Ϣ
    m_iSlotNum = stUnitInfo.stiteminfo().stitems_size();
    for(int i=0; i<m_iSlotNum; ++i)
    {
        m_astItems[i].iItemID = stUnitInfo.stiteminfo().stitems(i).iitemid();
        m_astItems[i].iItemNum = stUnitInfo.stiteminfo().stitems(i).iitemnum();
    }

    return;
}

void CFightUnitObj::UpdateFightUnitToDB(OneFightUnitInfo& stUnitInfo)
{
    //����ID
    stUnitInfo.set_iunitid(m_iConfigID);
    stUnitInfo.set_iunitaiid(m_iFightUnitAIID);

    //���µȼ�
    stUnitInfo.set_ilevel(m_iLevel);
    stUnitInfo.set_ilevelexp(m_iLevelExp);

    //���»�������
    for(int i=FIGHT_ATTR_HP; i<FIGHT_ATTR_MAX; ++i)
    {
        stUnitInfo.add_iattributes(m_aiUnitAttribute[i]);
    }

    //���¼���
    stUnitInfo.set_inormalskillid(m_iNormalSkill);

    //������Ʒ��Ϣ
    UnitItemInfo* pstItemInfo = stUnitInfo.mutable_stiteminfo();
    for(int i=0; i<m_iSlotNum; ++i)
    {
        OneSlotInfo* pstItem = pstItemInfo->add_stitems();
        pstItem->set_iitemid(m_astItems[i].iItemID);
        pstItem->set_iitemnum(m_astItems[i].iItemNum);
    }

    return;
}

