

#ifndef __COMBAT_UNIT_OBJ_HPP__
#define __COMBAT_UNIT_OBJ_HPP__

#include <vector>

#include "GameProtocol.hpp"
#include "GameConfigDefine.hpp"
#include "ZoneObjectAllocator.hpp"
#include "UnitBufferManager.hpp"

#include "FightUnitObj.hpp"

class CGameRoleObj;

//ս����λ����
class CCombatUnitObj : public CObj
{
public:
    CCombatUnitObj();
    virtual ~CCombatUnitObj();
    virtual int Initialize();

    DECLARE_DYN

public:

    //��ʼ������ս������
    int InitMonsterUnitObj(int iCombatUnitObjIndex, const OneCrossMonster& stOneConfig, const SFightPosInfo& stPosInfo, int iUnitType);

    //��ʼ����ɫս������
    int InitRoleUnitObj(int iCombatUnitObjIndex, CGameRoleObj& stRoleObj, int iFightUnitID, const SFightPosInfo& stPosInfo);

    //λ�����
    void SetUnitPosition(const TUNITPOSITION& rstUnitPos);
    TUNITPOSITION& GetUnitPosition();

    //����ս����λ��Buff��Ϣ
    void ClearUnitBuff();
    
    //�������ս��������Ϣ
    void PackCombatUnitInfo(int iCamp, CombatUnit& stUnitInfo);

    //ս����λ�Ƿ�����
    bool IsCombatUnitDead();

    //��ȡս����λ��ս������
    int GetFightAttr(int iAttrType);
    int AddFightAttr(int iAttrType, int iAddNum, int* iRealAddNum = NULL);

    //��ȡս����λ������
    int GetCombatUnitType();

    //��ȡս����λ��ID
    int GetCombatUnitID();

    //��ȡս����λ��������buff������ID
    void GetUnitBuffID(std::vector<int>& vBuffIDs);

    //�Ƿ�����ͬID��BUFF
    bool HasBuffOfSameID(int iBuffID);

    //ս����λ�ķ���
    void SetUnitDirection(int iDirection);
    int GetUnitDirection();

    //����ս����λ��AI
    int SetFightAI(int iFightAIID);
    int ChangeFightAI(int iFightAIID);
    int GetFightAI();

    //�������
    void GetValidFightSkills(std::vector<int>& vValidSkills);
    int GetNormalSkillID();
    void GetSkillByType(int iSkillType, std::vector<int>& vSkillIDs);

    //��󹥻��ĵ�λID
    int GetLastAttackUnitID();
    void SetLastAttackUnitID(int iUnitID);

    //BUFF���
    int AddUnitBuff(unsigned int uin, int iCrossID, int iBuffID, int iCastUnitID, Zone_CombatAddBuff_Notify& stNotify);
    int DoBuffEffectByType(unsigned int uin, int iCrossID, int iTriggerType, int iTriggerUnitID, Zone_DoBuffEffect_Notify& stNotify, int* pDamageNum = NULL);
    int DecreaseBuffRound(Zone_RemoveBuff_Notify& stNotify);
    bool HasBuffOfSameType(int iBuffID);
    void DelUnitBuff(int iBuffID, RemoveBuffEffect& stEffect);

    //��ȡ���õĵ�λID
    int GetConfigID();

    //��ȡ���õ�λ��Size
    int GetUnitSize();

    //��ȡ�������ܵ�ID
    int GetCounterAtkSkill();

    //�ж�ս����λ�Ƿ�ӵ�иü���
    bool HasFightSkill(int iSkillID);

    //ս����λ���ϵ�״̬
    void SetCombatUnitStatus(int iType, bool bSet);
    bool GetCombatUnitStatus(int iType);

    //��ȡս����λ������uin
    unsigned int GetUin();

    //ս����λ����ģʽ��iDistance=-1��ʾȡ������ģʽ
    void SetMotorMode(const TUNITPOSITION& stTargetPos, int iDistance, int iNewAIID = 0);

    //��ȡս����λ����ģʽ
    const TUNITPOSITION& GetMotorTargetPos();
    int GetMotorDistance();
    int GetMotorLeftLen();

    //��λ���ܵ�CD״̬
    int GetSkillCDRound(int iSkillID);
    void SetSkillCDRound(int iSkillID, int iRound);
    void DecreaseSkillCDRound(Zone_SkillCDRound_Notify& stNotify);

///////////////////////////////////////////////////////////////////////
private:

    //ս����λ������
    int m_iCombatUnitType;

    //ս����ս����λ��ΨһID
    int m_iCombatUnitID;

    //ս����λ���õ�ID
    int m_iConfigID;

    //ս����λ�������С
    int m_iSize;

    //ս����λ��ս���ϵ�λ��
    TUNITPOSITION m_stPos;

    //ս����λ��ս���ϵĳ���
    int m_iDirection;
        
    //ս����λ������
    int m_aiAttributes[FIGHT_ATTR_MAX];

    //ս����λ�ļ���
    int m_iNormalSkill;
    RepItem m_astUnitItems[MAX_UNIT_ITEM_SLOT];

    int m_aiCDRounds[MAX_FIGHT_CD_NUM];

    //ս����λ��AI
    int m_iUnitAIID;

    //ս����λ���ϵ�Buff��Ϣ
    CUnitBufferManager m_stBuffManager;

    //��󹥻��ĵ�λID
    int m_iLastAttackUnitID;

    //ս����λ��ǰ��״̬,ÿ��bit��ʾ��Ӧ״̬�Ƿ�����
    unsigned char m_ucUnitStatus;

    //��λ����ģʽ�ƶ�Ŀ��λ��
    TUNITPOSITION m_stMotorTargetPos;

    //��λ����ģʽ�ƶ���������
    int m_iMotorDistance;

    //ս����λ�������uin,��Ϊ0
    unsigned int m_uiUin;
};

#endif
