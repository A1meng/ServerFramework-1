
#include "GameProtocol.hpp"
#include "LogAdapter.hpp"
#include "ZoneErrorNumDef.hpp"
#include "ModuleShortcut.hpp"
#include "FightUnitManager.hpp"
#include "GameRole.hpp"
#include "ZoneMsgHelper.hpp"
#include "UnitUtility.hpp"
#include "Random.hpp"

#include "CombatUnitObj.hpp"

using namespace ServerLib;
using namespace GameConfig;

IMPLEMENT_DYN(CCombatUnitObj)

CCombatUnitObj::CCombatUnitObj()
{
    Initialize();
}

CCombatUnitObj::~CCombatUnitObj()
{
    //ClearUnitBuff();
}

int CCombatUnitObj::Initialize()
{
    m_iCombatUnitType = COMBAT_UNIT_TYPE_INVALID;
    m_iCombatUnitID = -1;
    m_iConfigID = 0;

    m_iSize = 0;
    m_stPos.iPosX = 0;
    m_stPos.iPosY = 0;
    m_iDirection = 0;

    memset(m_aiAttributes, 0, sizeof(m_aiAttributes));

    //����
    m_iNormalSkill = 0;
    memset(m_astUnitItems, 0, sizeof(m_astUnitItems));
    memset(m_aiCDRounds, 0, sizeof(m_aiCDRounds));

    //ս����λAI
    m_iUnitAIID = 0;

    //Buffer������
    m_stBuffManager.Initialize();

    //��󹥻��ĵ�λID
    m_iLastAttackUnitID = -1;

    //ս����λ��״̬
    m_ucUnitStatus = 0;

    //��λ����ģʽ���
    m_stMotorTargetPos.iPosX = -1;
    m_stMotorTargetPos.iPosY = 0;

    m_iMotorDistance = -1;

    //ս����λ�������
    m_uiUin = 0;

    return T_SERVER_SUCESS;
}

//��ʼ������ս������
int CCombatUnitObj::InitMonsterUnitObj(int iCombatUnitObjIndex, const OneCrossMonster& stOneConfig, const SFightPosInfo& stPosInfo, int iUnitType)
{
    m_iCombatUnitType = iUnitType;
    m_iConfigID = stOneConfig.iMonsterID;
    m_iCombatUnitID = iCombatUnitObjIndex;

    //����û�л���ģʽ
    m_iMotorDistance = -1;

    m_iLastAttackUnitID = -1;

    //��ȡ��������
    const SMonsterConfig* pstMonsterConfig = MonsterCfgMgr().GetConfig(m_iConfigID);
    if(!pstMonsterConfig)
    {
        LOGERROR("Failed to get monster config, invalid id %d\n", m_iConfigID);
        return T_ZONE_SYSTEM_PARA_ERR;
    }

    m_iSize = pstMonsterConfig->iSize;

    //���ù���Ļ�������
    for(int i=0; i<FIGHT_ATTR_MAX; ++i)
    {
        m_aiAttributes[i] = pstMonsterConfig->aiAttribute[i];
    }

    //���ù��＼����Ϣ
    m_iNormalSkill = pstMonsterConfig->iNormalSkill;

    //���ù�����Ʒ��Ϣ
    for(int i=0; i<MAX_UNIT_ITEM_SLOT; ++i)
    {
        m_astUnitItems[i].iItemID = stOneConfig.aiItemInfo[i];
    }

    memset(m_aiCDRounds, 0, sizeof(m_aiCDRounds));

    //���ù���ʹ�õ�AI
    m_iUnitAIID = pstMonsterConfig->iMonsterAIID;

    //���ù������ʼλ����Ϣ
    m_stPos.iPosX = stPosInfo.iPosX;
    m_stPos.iPosY = stPosInfo.iPosY;
    m_iDirection = stPosInfo.iDirection;

    //BUFF������
    m_stBuffManager.SetOwnerUnitID(m_iCombatUnitID);

    return T_SERVER_SUCESS;
}

//��ʼ����ɫս������
int CCombatUnitObj::InitRoleUnitObj(int iCombatUnitObjIndex, CGameRoleObj& stRoleObj, int iFightUnitID, const SFightPosInfo& stPosInfo)
{
    //����ս����������
    if(iFightUnitID == 1)
    {
        //Ĭ��IndexΪ0��λ��������
        m_iCombatUnitType = COMBAT_UNIT_TYPE_ROLE;
    }
    else
    {
        //�������ǻ��
        m_iCombatUnitType = COMBAT_UNIT_TYPE_PARTNER;
    }

    m_uiUin = stRoleObj.GetUin();

    //��ȡս����λ������
    CFightUnitManager& rstFightUnitManager = stRoleObj.GetFightUnitManager();

    //��ȡս����λ��Ϣ
    CFightUnitObj* pstFightUnitObj = rstFightUnitManager.GetFightUnitByID(iFightUnitID);
    if(!pstFightUnitObj)
    {
        LOGERROR("Failed to get fight unit obj, index %d, uin %u\n", iFightUnitID, stRoleObj.GetUin());
        return T_ZONE_SYSTEM_PARA_ERR;
    }

    //����ս����λΨһID
    m_iCombatUnitID = iCombatUnitObjIndex;

    //�������õ�ID
    m_iConfigID = pstFightUnitObj->GetFightUnitID();

    //����ս����λ��AI
    m_iUnitAIID = pstFightUnitObj->GetFightUnitAIID();

    //����ս��������ս���ϵ�λ��
    m_stPos.iPosX = stPosInfo.iPosX;
    m_stPos.iPosY = stPosInfo.iPosY;
    m_iDirection = stPosInfo.iDirection;

    //����û�л���ģʽ
    m_iMotorDistance = -1;

    m_iLastAttackUnitID = -1;

    //����ս�����������
    int iRet = pstFightUnitObj->InitFightAttr(m_aiAttributes, FIGHT_ATTR_MAX);
    if(iRet)
    {
        LOGERROR("Failed to init combat unit attr, unit index %d, ret %d, uin %u\n", iFightUnitID, iRet, stRoleObj.GetUin());
        return iRet;
    }

    //����ս����λ�չ�����Ʒ
    m_iNormalSkill = pstFightUnitObj->GetNormalSkill();

    pstFightUnitObj->InitFightUnitItem(m_astUnitItems, sizeof(m_astUnitItems));

    memset(m_aiCDRounds, 0, sizeof(m_aiCDRounds));

    //����BUFF������
    m_stBuffManager.SetOwnerUnitID(m_iCombatUnitID);

    return T_SERVER_SUCESS;
}

//λ�����
void CCombatUnitObj::SetUnitPosition(const TUNITPOSITION& rstUnitPos)
{
    m_stPos.iPosX = rstUnitPos.iPosX;
    m_stPos.iPosY = rstUnitPos.iPosY;

    return;
}

TUNITPOSITION& CCombatUnitObj::GetUnitPosition()
{
    return m_stPos;
}

//����ս����λ��Buff��Ϣ
void CCombatUnitObj::ClearUnitBuff()
{
    m_stBuffManager.ClearBuffObj();
}

//�������ս��������Ϣ
void CCombatUnitObj::PackCombatUnitInfo(int iCamp, CombatUnit& stUnitInfo)
{
    //ս����λ����
    stUnitInfo.set_etype((CombatUnitType)m_iCombatUnitType);

    //ս����λ��ΨһID
    stUnitInfo.set_iunitid(m_iCombatUnitID);

    //ս����λ��������Ӫ
    stUnitInfo.set_icamp(iCamp);

    //ս����λ������ID
    stUnitInfo.set_iconfigid(m_iConfigID);

    //ս����λս���ϵ�λ��
    UnitPosition* pstPos = stUnitInfo.mutable_stpos();
    pstPos->set_iposx(m_stPos.iPosX);
    pstPos->set_iposy(m_stPos.iPosY);
    
    //ս����λս���ϵķ���
    stUnitInfo.set_idirection(m_iDirection);

    //����ս����λ������
    for(int i=0; i<FIGHT_ATTR_MAX; ++i)
    {
        stUnitInfo.add_iattributes(m_aiAttributes[i]);
    }

    //����ս����λ��AI
    stUnitInfo.set_iunitaiid(m_iUnitAIID);

    //���õ�λ�չ�����Ʒ
    stUnitInfo.set_inormalskill(m_iNormalSkill);

    for(int i=0; i<MAX_UNIT_ITEM_SLOT; ++i)
    {
        OneSlotInfo* pstOneItem = stUnitInfo.add_stitems();
        pstOneItem->set_iitemid(m_astUnitItems[i].iItemID);
        pstOneItem->set_iitemnum(m_astUnitItems[i].iItemNum);
    }

    return;
}

//ս����λ�Ƿ�����
bool CCombatUnitObj::IsCombatUnitDead()
{
    return (m_aiAttributes[FIGHT_ATTR_HP] <= 0);
}

//��ȡս����λ��ս������
int CCombatUnitObj::GetFightAttr(int iAttrType)
{
    if(iAttrType<0 || iAttrType>=FIGHT_ATTR_MAX)
    {
        return 0;
    }

    return m_aiAttributes[iAttrType];
}

//����ս����λ��ս������
int CCombatUnitObj::AddFightAttr(int iAttrType, int iAddNum, int* iRealAddNum)
{
    if(iAddNum == 0)
    {
        return T_SERVER_SUCESS;
    }

    if(iAttrType<0 || iAttrType>=FIGHT_ATTR_MAX)
    {
        return T_ZONE_SYSTEM_PARA_ERR;
    }

    if(iRealAddNum)
    {
        *iRealAddNum = -m_aiAttributes[iAttrType];
    }

    m_aiAttributes[iAttrType] += iAddNum;
    if(m_aiAttributes[iAttrType] < 0)
    {
        m_aiAttributes[iAttrType] = 0;
    }

    //�����Ѫ�������ܳ�����ǰ���Ѫ��
    if(iAttrType==FIGHT_ATTR_HP && m_aiAttributes[iAttrType]>m_aiAttributes[FIGHT_ATTR_HPMAX])
    {
        m_aiAttributes[iAttrType] = m_aiAttributes[FIGHT_ATTR_HPMAX];

    }

    //���ս����λѪ��Ϊ0
    if(iAttrType==FIGHT_ATTR_HP && m_aiAttributes[iAttrType]==0)
    {
        if(GetCombatUnitStatus(COMBAT_UNIT_STATUS_UNBENDING))
        {
            //�в���״̬��������
            SetCombatUnitStatus(COMBAT_UNIT_STATUS_UNBENDING, false);

            m_aiAttributes[iAttrType] = 1;
        }
        else
        {
            //��ս����ɾ��
            m_stPos.iPosX = -1;
        }
    }

    if(iRealAddNum)
    {
        *iRealAddNum += m_aiAttributes[iAttrType];
    }

    return T_SERVER_SUCESS;
}

//��ȡս����λ������
int CCombatUnitObj::GetCombatUnitType()
{
    return m_iCombatUnitType;
}

//��ȡս����λ��ID
int CCombatUnitObj::GetCombatUnitID()
{
    return m_iCombatUnitID;
}

//��ȡս����λ��������buff������ID
void CCombatUnitObj::GetUnitBuffID(std::vector<int>& vBuffIDs)
{
    return m_stBuffManager.GetUnitBuffID(vBuffIDs);
}

//�Ƿ�����ͬID��BUFF
bool CCombatUnitObj::HasBuffOfSameID(int iBuffID)
{
    return m_stBuffManager.HasBuffOfSameID(iBuffID);
}

void CCombatUnitObj::SetUnitDirection(int iDirection)
{
    m_iDirection = iDirection;
}

//��ȡս����λ�ķ���
int CCombatUnitObj::GetUnitDirection()
{
    return m_iDirection;
}

//����ս����λ��AI
int CCombatUnitObj::SetFightAI(int iFightAIID)
{
    //�ж��Ƿ��ǺϷ���AI ID
    const SFightUnitConfig* pstUnitConfig = FightUnitCfgMgr().GetConfig(m_iConfigID);
    if(!pstUnitConfig)
    {
        LOGERROR("Failed to set fight ai , invalid unit config id %d\n", m_iConfigID);
        return T_ZONE_SYSTEM_INVALID_CFG;
    }

    for(int i=0; i<pstUnitConfig->iAINum; ++i)
    {
        if(iFightAIID == pstUnitConfig->aiUnitAIID[i])
        {
            m_iUnitAIID = iFightAIID;

            return T_SERVER_SUCESS;
        }
    }

    return T_ZONE_SYSTEM_PARA_ERR;
}

int CCombatUnitObj::ChangeFightAI(int iFightAIID)
{
    //�жϸ�AI�Ƿ�Ϸ�
    const SFightUnitAIConfig* pstAIConfig = FightUnitAICfgMgr().GetConfig(iFightAIID);
    if(!pstAIConfig)
    {
        LOGERROR("Failed to get fight unit AI config, invalid AI id %d\n", iFightAIID);
        return T_ZONE_SYSTEM_INVALID_CFG;
    }

    m_iUnitAIID = iFightAIID;

    return T_SERVER_SUCESS;
}

int CCombatUnitObj::GetFightAI()
{
    return m_iUnitAIID;
}

void CCombatUnitObj::GetValidFightSkills(std::vector<int>& vValidSkills)
{
    vValidSkills.clear();

    //�ȷ����չ�
    vValidSkills.push_back(m_iNormalSkill);

    //todo jasonxiong5 ��ߺ��������Ʒ���ӵļ�����Ϣ

    return;
}

int CCombatUnitObj::GetNormalSkillID()
{
    return m_iNormalSkill;
}

void CCombatUnitObj::GetSkillByType(int iSkillType, std::vector<int>& vSkillIDs)
{
    //todo jasonxiong5 �������ȷ������ô��
    /*
    for(int i=0; i<m_iSkillNum; ++i)
    {
        const SFightUnitSkillConfig* pstSkillConfig = FightSkillCfgMgr().GetConfig(m_aiSkills[i]);
        if(!pstSkillConfig)
        {
            //LOGERROR("Failed to get fight skill, skill id %d\n", m_astSkills[i].iSkillID);
            break;
        }

        if(pstSkillConfig->iSkillType == iSkillType)
        {
            vSkillIDs.push_back(m_aiSkills[i]);
        }
    }
    */

    return;
}

//��󹥻��ĵ�λID
int CCombatUnitObj::GetLastAttackUnitID()
{
    return m_iLastAttackUnitID;
}

void CCombatUnitObj::SetLastAttackUnitID(int iUnitID)
{
    m_iLastAttackUnitID = iUnitID;
}

//BUFF���
int CCombatUnitObj::AddUnitBuff(unsigned int uin, int iCrossID, int iBuffID, int iCastUnitID, Zone_CombatAddBuff_Notify& stNotify)
{
    if(IsCombatUnitDead())
    {
        //��λ�Ѿ����������ܼ�BUFF
        return T_SERVER_SUCESS;
    }

    return m_stBuffManager.AddUnitBuff(uin, iCrossID, iBuffID, iCastUnitID, stNotify);
}

int CCombatUnitObj::DoBuffEffectByType(unsigned int uin, int iCrossID, int iTriggerType, int iTriggerUnitID, Zone_DoBuffEffect_Notify& stNotify, int* pDamageNum)
{
    return m_stBuffManager.DoUnitBuffEffect(uin, iCrossID, iTriggerType, iTriggerUnitID, stNotify, pDamageNum);
}

//����BUFF�Ļغ���
int CCombatUnitObj::DecreaseBuffRound(Zone_RemoveBuff_Notify& stNotify)
{
    return m_stBuffManager.DecreaseAllBuffRound(stNotify);
}

bool CCombatUnitObj::HasBuffOfSameType(int iBuffID)
{
    return m_stBuffManager.HasBuffOfSameType(iBuffID);
}

//ɾ��ս����λ���ϵ�BUFF
void CCombatUnitObj::DelUnitBuff(int iBuffID, RemoveBuffEffect& stEffect)
{
    m_stBuffManager.DelUnitBuffByID(iBuffID, stEffect);

    return;
}

//��ȡ���õĵ�λID
int CCombatUnitObj::GetConfigID()
{
    return m_iConfigID;
}

//��ȡ���õ�λ��Size
int CCombatUnitObj::GetUnitSize()
{
    return m_iSize;
}

//��ȡ�������ܵ�ID
int CCombatUnitObj::GetCounterAtkSkill()
{
    switch(m_iCombatUnitType)
    {
        case COMBAT_UNIT_TYPE_MONSTER:
            {
                //�����
                const SMonsterConfig* pstMonsterConfig = MonsterCfgMgr().GetConfig(m_iConfigID);
                if(!pstMonsterConfig)
                {
                    LOGERROR("Failed to get monster config, monster id %d\n", m_iConfigID);
                    return 0;
                }

                return pstMonsterConfig->iCounterAtkSkillID;
            }
            break;

        default:
            {
                //��ɫ��λ��
                const SFightUnitConfig* pstUnitConfig = FightUnitCfgMgr().GetConfig(m_iConfigID);
                if(!pstUnitConfig)
                {
                    LOGERROR("Failed to get fight unit config, unit id %d\n", m_iConfigID);
                    return 0;
                }

                return pstUnitConfig->iCounterAtkSkillID;
            }
            break;
    }

    return 0;
}

//�ж�ս����λ�Ƿ�ӵ�иü���
bool CCombatUnitObj::HasFightSkill(int iSkillID)
{
    //todo jasonxiong5 ����ȷ������Ʒ��ص���ô��
    return (m_iNormalSkill==iSkillID);
}

//ս����λ���ϵ�״̬
void CCombatUnitObj::SetCombatUnitStatus(int iType, bool bSet)
{
    if(bSet)
    {
        //����״̬
        m_ucUnitStatus = (m_ucUnitStatus | (0x01<<iType));
    }
    else
    {
        //���״̬
        m_ucUnitStatus = (m_ucUnitStatus & ~(0x01<<iType));
    }

    return;
}

bool CCombatUnitObj::GetCombatUnitStatus(int iType)
{
    return ((m_ucUnitStatus>>iType) & 0x01);
}

//��ȡս����λ������uin
unsigned int CCombatUnitObj::GetUin()
{
    return m_uiUin;
}

//ս����λ����ģʽ
void CCombatUnitObj::SetMotorMode(const TUNITPOSITION& stTargetPos, int iDistance, int iNewAIID)
{
    m_stMotorTargetPos = stTargetPos;
    
    m_iMotorDistance = iDistance;

    if(iNewAIID != 0)
    {
        m_iUnitAIID = iNewAIID;
    }

    return;
}

const TUNITPOSITION& CCombatUnitObj::GetMotorTargetPos()
{
    return m_stMotorTargetPos;
}

int CCombatUnitObj::GetMotorDistance()
{
    return m_iMotorDistance;
}

int CCombatUnitObj::GetMotorLeftLen()
{
    return ABS(m_stMotorTargetPos.iPosX,m_stPos.iPosX) + ABS(m_stMotorTargetPos.iPosY,m_stPos.iPosY);
}

//��λ���ܵ�CD״̬
int CCombatUnitObj::GetSkillCDRound(int iSkillID)
{
    //todo jasonxiong5 �ȱ��������濴����ô��

    return 0;
}

void CCombatUnitObj::SetSkillCDRound(int iSkillID, int iRound)
{
    //todo jasonxiong5 �ȱ��������濴����ô��

    return;
}

void CCombatUnitObj::DecreaseSkillCDRound(Zone_SkillCDRound_Notify& stNotify)
{
    stNotify.set_iunitid(m_iCombatUnitID);

    //todo jasonxiong5 �ȱ��������濴����ô��
    /*
    for(int i=0; i<m_iSkillNum; ++i)
    {
        if(m_aiSkills[i]!=0 && m_aiCDRounds[i]>0)
        {
            --m_aiCDRounds[i];

            SkillCDInfo* pstInfo = stNotify.add_stcdinfos();
            pstInfo->set_iskillid(m_aiSkills[i]);
            pstInfo->set_icdround(m_aiCDRounds[i]);
        }
    }
    */

    return;
}

