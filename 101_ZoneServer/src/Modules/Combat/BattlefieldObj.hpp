
#ifndef __BATTLEFIELD_OBJ_HPP__
#define __BATTLEFIELD_OBJ_HPP__

#include <vector>

#include "GameProtocol.hpp"
#include "GameConfigDefine.hpp"
#include "ZoneObjectAllocator.hpp"
#include "ScenePathManager.hpp"
#include "CombatTrapObj.hpp"

class CCombatUnitObj;
class CGameRoleObj;

using namespace GameConfig;

//����ս����λ��ǰ��״̬����
enum enRoleCombatUnitStatType
{
    ROLE_COMBAT_UNIT_STAT_INVALID = 0,      //�Ƿ���״̬
    ROLE_COMBAT_UNIT_STAT_IDLE = 1,         //����״̬
    ROLE_COMBAT_UNIT_STAT_MOVE = 2,         //�ȴ��ƶ�״̬
    ROLE_COMBAT_UNIT_STAT_ACTION = 3,       //�ȴ��ж�״̬
    ROLE_COMBAT_UNIT_STAT_SETFORM = 4,      //�ȴ���������
};

//PVPս���ؿ���ID
#define PVP_COMBAT_CROSS_ID 100

//ս��ս������
class CBattlefieldObj : public CObj
{
public:
    CBattlefieldObj();
    virtual ~CBattlefieldObj();
    virtual int Initialize();

    DECLARE_DYN

public:

    //��������uin
    void SetActiveUin(unsigned int uiUin);
    unsigned int GetActiveUin();
    
    //��������uin
    void SetPassiveUin(unsigned int uiUin);
    unsigned int GetPassiveUin();

    //ս��Obj��ΨһID
    void SetBattlefieldObjID(int iObjID);
    int GetBattlefieldObjID();

    //��ʼ��PVEս��
    int InitPveBattlefield(CGameRoleObj& rstRoleObj, int iCrossID);

    //��ʼ��PVPս��
    int InitPVPBattlefield(CGameRoleObj& stActiveRoleObj, CGameRoleObj& stPassiveRoleObj);

    //�������ս������Ϣ
    int PackBattlefiledInfo(Zone_Battlefield_Notify& stNotify);

    //����ս��
    void ClearBattlefield();

    //��ʼ���غϳ��ֵ�λ��Ϣ
    void InitRoundActionUnits();

    //�жϵ�ǰ�Ƿ���δ���ֵ�λ
    bool HasActionCombatUnit();

    //���õ�ǰ���ֵ�ս����λ
    void SetActionCombatUnit();

    //�����ǰ���ֵ�ս����λ
    void ClearActionCombatUnit();

    //��ȡ��ǰ���ֵ�ս����λ������
    int GetActionUnitType();

    //��ȡ��ǰ���ֵ�λ��ID
    int GetActionUnitID();

    //����ս����λ��ǰ��״̬
    int GetActionUnitStatus();
    void SetActionUnitStatus(int iStatus);

    //�ж��Ƿ��ܹ�����ս��
    bool CanEndCombat();

    //���Ϳͻ������ǲ�����ָ��
    void NotifyRoleUnitDirective(int iDirective);

    //��ʼս����λ���ƶ�
    int DoCombatUnitMove(const UnitPath& stPath);

    //��ʼս����λ���ƶ�,AI�ƶ�ֻ��Ŀ���
    int DoCombatUnitMove(int iMoveDistance, const TUNITPOSITION* pstMovePath);

    //ս����λʹ�ü���
    int DoCombatCastSkill(int iSkillID, const TUNITPOSITION& stTargetPos, int iSkillUseType);

    //ս����λʹ�õ���
    int DoCombatUseItem(int iItemID, const TUNITPOSITION& stTargetPos);

    //��ȡս����λ����Ӫ
    int GetCombatUnitCamp(int iCombatUnitiD);

    //ս����λ�Ƿ�ͬһ��Ӫ
    bool IsUnitInSameCamp(CCombatUnitObj& stUnitObj, CCombatUnitObj& stOtherUnitObj);

    //���û���ս��AI
    int SetPartnerAI(int iPartnerAIID);

    //��ȡ��ͼID
    int GetMapID();

    //��������λ�û�ȡս����λ��Ϣ
    CCombatUnitObj* GetCombatUnitByPos(const TUNITPOSITION& stPos);

    //����ǻ�����󹥻�����
    bool IsTeammateLastAttackUnit(int iCastUnitID, int iTargetUnitID);

    //��ȡ������Ϣ
    void GetEnemyUnits(int iCastUnitID, std::vector<int>& vEnemyUnits);

    //����ս����ص�BUFF
    int DoBuffEffectByType(int iTriggerType, int iTargetUnitID=-1, int iTriggerUnitID=-1, int* pDamageNum = NULL);
    int DecreaseBuffRound();

    //��ȡս��ʤ�����
    bool GetIsActiveWin();

    //����ս�����,�Ƿ񼺷���ʤ
    int SetCampActiveWin(int iIsActiveWin);

    //��ȡ�ؿ���ID
    int GetCrossID();

    //���õ�ǰ�ж���λ�ķ���
    int SetActionUnitDirection(int iDirection);

    //��ȡ��Ӫս����λ��UnitID
    int GetCombatUnitByCamp(int iCampType, int iUnitIndex);

    //ս����λ�Ļغ���
    int GetCombatRoundNum();
    void SetCombatRoundNum(int iRoundNum);

    //����ս���ϵ�ս����λ
    int AddCombatUnit(int iCampType, int iConfigID, int iPosX, int iPosY, int iDirection, int iUnitType, int& iUnitID);

    //�л�ս����λ��AI
    int ChangeCombatUnitAI(int iCombatUnitID, int iNewAI);

    //ǿ��ɱ��ս����λ
    int KillCombatUnit(int iCombatUnitID, bool bSendNotify=true);

    //��ȡս����ʼʱ��
    int GetCombatStartTime();

    //�����ж��غϿ�ʼǰ�ļ���
    int DoRoundActionSkill();

    //��ȡ����ƶ����룬����Χ���߼�
    int GetMaxMoveDistance(CCombatUnitObj& stUnitObj);

    //��ȡս����λ��Χ����Ӫ��λ����
    int GetNearByUnitNum(CCombatUnitObj& stUnitObj, bool bIsEnemy);

    //��ȡս����λб��λ�õ���Ӫ��λ����
    int GetAngleNearByUnitNum(CCombatUnitObj& stUnitObj, bool bIsEnemy);

    //ս����ĳ��λ���Ƿ��������, iUnitID��ʾ���λ�����Ǹõ�λ��������
    bool IsPosCanWalk(const TUNITPOSITION& stPos, int iUnitID = -1);

    //��ȡս����ͼ�赲����
    const CScenePathManager& GetBattlePathManager();

    //ս�������ӻ���,���ػ��ص�ObjID��С��0��ʾ����ʧ��
    int AddCombatTrap(int iConfigID, int iCamp, const TUNITPOSITION& stPos, int iDirection);

    //����ս���ϵĻ���
    int TriggerCombatTrap(int iTriggerObjID, int iTrapObjID, int iTriggerType);

    //���ս���ϵĻ���
    void DeleteCombatTrap(int iTrapObjID);

    //�����������͵Ļ���
    int TriggerPitfallTrap(int iTriggerType);

    //�ֶ���������
    int ManualTriggerTrap(const TUNITPOSITION& stPos);

    //����ս������
    int AttackCombatTrap(const TUNITPOSITION& stPos);

    //�жϻ����Ƿ�ɼ�
    bool IsTrapVisible(CCombatUnitObj& stUnitObj, CCombatTrapObj& stTrapObj);
    bool IsTrapVisibleToActionUnit(CCombatTrapObj& stTrapObj);

    //��ȡս��������ϸ��Ϣ
	void GetCombatTrapInfo(std::vector<TCOMBATTRAPINFO>& vTrapInfo);

    //���ݻ��ص�λ�û�ȡ����
    CCombatTrapObj* GetTrapByPos(const TUNITPOSITION& stPos);
    int GetTrapIndexByPos(const TUNITPOSITION& stPos);

    //ս��״̬�����ű�ʹ��
    bool GetCombatStatus(int iStatusType);
    void SetCombatStatus(int iStatusType, bool bIsSet);

    //ֱ�Ӹı�ս����λ������
    void AddCombatUnitAttr(CCombatUnitObj& stUnitObj, int iAttrType, int iAddValue);

    //ɾ��ս����λ���ϵ�BUFF
    void DelUnitBuff(int iUnitID, int iBuffID);

    //������Ϣ��ս�����������
    void SendNotifyToBattlefield(GameProtocolMsg& stNotify);

    //��ǰ�ж�����ҽ�ɫ�Ƿ���Ч
    bool IsValidActionRole(unsigned int uin);

    //�Ƿ���Ҫ��������
    bool IsNeedSetForm();

    //����ս������
    int SetCombatForm(unsigned int uin, const CombatForm& stForm);

    //����ս�����͵�֪ͨ
    void SendCombatFormNotify();

    //�ж�ս����λ�Ƿ�����
    bool IsCombatUnitDead(int iUnitID);

    //���͵�ǰս���ĳ�����Ϣ
    void SendActionUnitNotify();

    //����ս��ս����λ��ϸ��Ϣ
    void SaveCombatUnitInfo();

    //����ս���������ĵ�λ
    void ClearDeadCombatUnit();

    //�����ж���λ����CD�غ�
    void DecreaseActionSkillCDRound();

private:

    //��ʼ��ս����Ϣ
    int InitBattlefieldInfo(int iCrossID);

    //��ʼ��������Ϣ
    int InitMonsterInfo(int iCrossID);
    
    //��ʼ����ֻ������Ϣ
    int InitOneMonsterInfo(int iUnitIndex, const OneCrossMonster& stOneConfig, const SFightPosInfo& stPosInfo);

    //��ʼ��������Ӫ��Ϣ
    int InitFightCampInfo(CGameRoleObj& rstRoleObj, bool bIsActive = true);    

    //��ʼ������ս����λ��Ϣ
    int InitOneFightUnitInfo(int iUnitIndex, CGameRoleObj& rstRoleObj, int iFightUnitID, const SFightPosInfo& stPosInfo, bool bIsActive);

    //������ص���ս��������Ϣ
    int PackOneCombatUnitInfo(int iCamp, int iUnitObjIndex, CombatUnit& stUnitInfo);

    //���һ��ս����λ���غϳ����б�
    void AddOneUnitToRoundActionList(int iUnitObjIndex);

    //�����ܵ�����
    int DoComboAttack(CCombatUnitObj& stTargetUnitObj, int iSkillID);

    //�����ܵļл�
    int DoPincerAttack(CCombatUnitObj& stTargetUnitObj);

    //�����ܵķ���
    int DoCounterAttack(CCombatUnitObj& stCounterAtkUnitObj, CCombatUnitObj& stTargetObj);

    //�����鼼�ܵĺϻ�
    int DoJointAttack(CCombatUnitObj& stTargetUnitObj, int iSkillID);

    //��ȡ�赲���͵Ļ���
    CCombatTrapObj* GetBlockTrapByPos(const TUNITPOSITION& stPos);

    //��ȡ��ͼ����Чվλλ��
    void GetBattlefieldValidPos(const SBattleCrossConfig& stCrossConfig);

private:
    
    //����CCombatUnitObj,��������ObjIndex
    CCombatUnitObj* CreateCombatUnitObj(int& iCombatUnitObjIndex);

    //ɾ��һ��ս����λ
    void ClearOneCombatUnitObj(int iCombatUnitID);

///////////////////////////////////////////////////////////////////////
private:

    //ս�������ΨһID
    int m_iBattlefieldObjID;

    //����ս����
    unsigned int m_uiActiveUin;
    int m_aiActiveCombatObjIndex[MAX_CAMP_FIGHT_UNIT_NUM];
    SFightPosInfo m_astActivePos[MAX_CAMP_FIGHT_UNIT_NUM];
    bool m_bActiveNeedSetForm;

    //����ս����
    unsigned int m_uiPassiveUin;
    int m_aiPassiveCombatObjIndex[MAX_CAMP_FIGHT_UNIT_NUM];
    SFightPosInfo m_astPassivePos[MAX_CAMP_FIGHT_UNIT_NUM];
    bool m_bPassiveNeedSetForm;

    //�������ű�ǿ��ɱ���ĵ�λ����ID
    int m_iActiveScriptKilledNum;
    int m_aiActiveScriptKilledConfigID[MAX_CAMP_FIGHT_UNIT_NUM];

    //��ǰ�ж���λ��ص���Ϣ
    int m_iActionUnit;
    int m_iCanCombatUnitNum;
    int m_aiCanCombatUnit[MAX_CAMP_FIGHT_UNIT_NUM*2];
    int m_iActionDoneUnitNum;
    int m_aiActionDoneUnit[MAX_CAMP_FIGHT_UNIT_NUM*2];

    //ս���ϵĻ�����Ϣ
    int m_iTrapNum;
    int m_aiCombatTrapIndex[MAX_COMBAT_TRAP_NUM];

    //��ǰ�ж���λ��״̬
    int m_iRoleCombatUnitStatus;

	//����ս���Ĺؿ�ID
	int m_iCrossID;

    //����ս���ĵ�ͼID
    int m_iMapID;

    //����ս����ͼ���赲������
    const CScenePathManager* m_pstBattlePathManager;

    //����ս����ʤ�����
    int m_iCombatResult;

    //���õ�ǰս���Ļغ���
    int m_iCombatRoundNum;

    //ս����ʼ��ʱ��
    int m_iStartTime;

    //����ս����״̬�����ű�ʹ��
    unsigned char m_ucCombatStatus;

private:

    //BUFF������͸��ͻ��˵���Ϣ
    static GameProtocolMsg m_stRemoveBuff_Notify;               //�Ƴ�BUFF
};

#endif
