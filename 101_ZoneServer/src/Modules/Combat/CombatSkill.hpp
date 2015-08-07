#ifndef __COMBAT_SKILL_HPP__
#define __COMBAT_SKILL_HPP__

//ս���м��ܴ�����

#include "GameConfigDefine.hpp"

using namespace GameConfig;

class CGameRoleObj;
class CCombatUnitObj;
class CCombatSkill
{
public:
    CCombatSkill();
    ~CCombatSkill();

public:

    //ʹ��ս������,���һ��������ʾ�Ƿ��Ȼ����
    static int CastSkill(int iBattlefieldObjID, int iCastUnitID, const TUNITPOSITION& stTargetPos, int iSkillID, int iSkillUseType, bool bIsScriptCast = false);

private:

    //�Ե���Ŀ��ʹ��ս������
    static int CastSkillToOneTarget(int iCastUnitID, int iTargetUnitID, int iSkillID, bool bIsChiefTarget);

    //�����ж�
    static bool CheckCanHit(CCombatUnitObj& stCastUnitObj, CCombatUnitObj& stTargetUnitObj, const SFightUnitSkillConfig& stSkillConfig);

    //���ж�
    static bool CheckCanBlock(CCombatUnitObj& stCastUnitObj, CCombatUnitObj& stTargetUnitObj, const SFightUnitSkillConfig& stSkillConfig);

    //�����ж�
    static bool CheckCanCrit(CCombatUnitObj& stCastUnitObj, CCombatUnitObj& stTargetUnitObj, const SFightUnitSkillConfig& stSkillConfig);

    //�����˺�
    static int GetSkillHurtNum(CCombatUnitObj& stCastUnitObj, CCombatUnitObj& stTargetUnitObj, const SFightUnitSkillConfig& stSkillConfig, bool bIsChiefTarget);

    //����Ƿ���Ч�ļ����ͷ�Ŀ��
    static bool CheckIsValidTarget(CBattlefieldObj& stBattlefieldObj, int iTargetType, int iCastUnitID, int iTargetUnitID);

    //�����ܻ���λ��ת��
    static void TargetUnderAttackDirection(CCombatUnitObj& stTargetUnitObj, int iCastUnitDirection);

    //����Ŀ�굥λ���˺�
    static int ProcessRealDamage(CBattlefieldObj& stBattlefieldObj, CCombatUnitObj& stCastUnitObj, CCombatUnitObj& stTargetUnitObj, int iDamageNum, ActionTarget& stNotify, const SFightUnitSkillConfig& stSkillConfig);

    //����Ŀ�굥λ�������Ե��˺�
    //ע�⣺��������л��޸�iDamageNum��ֵ
    static void ProcessTargetDamage(CCombatUnitObj& stTargetUnitObj, const SSkillDamageConfig& stDamageConfig, ActionTarget& stNotify, int iAttrType, int& iDamageNum);

    //�����ܻ����Ķ���Э��,����ֵΪЭ����Ч��ϵ��
    static float ProcessCoordDefense(CBattlefieldObj& stBattlefieldObj, CCombatUnitObj& stTargetUnitObj);

    //������������ɵĵ�λλ��
    static int ProcessSkillMove(CBattlefieldObj& stBattlefieldObj, CCombatUnitObj& stCastUnitObj, CCombatUnitObj& stTargetUnitObj, ActionTarget& stNotify, const SFightUnitSkillConfig& stSkillConfig);

    //�����ܵ�����Ч��
    static int ProcessSkillSpecailFunc(CCombatUnitObj& stCastUnitObj, CCombatUnitObj& stTargetUnitObj, ActionTarget& stNotify, const SFightUnitSkillConfig& stSkillConfig);

public:

    //��ҪĿ�������״̬
    static int m_iChiefTargetDodge;

private:

    //ս����ID
    static int m_iBattlefiledObjID;
    
    //�ͷż��ܵ�ս����λID
    static int m_iCastUnitID;
    
    //�����ͷ�Ŀ���ս����λID
    static int m_iTargetUnitID;
    
    //ʹ�õļ���ID
    static int m_iSkillID;  

    //ս�����𷽵�uin
    static unsigned m_uiActiveUin;

    //ս�����ܷ���uin
    static unsigned m_uiPassiveUin;

    //�����ͷŷ���RoleObj,����Ϊ��
    static CGameRoleObj* m_pstCastRoleObj;

    //����˫���ľ���
    static int m_iDistance;

    //�Ƿ�ű���ʹ�õļ���
    static bool m_bIsScriptCast;

    //���ܵ�ʹ������
    static int m_iUseType;

    //����ʹ�ø��ͻ��˵�֪ͨ��Ϣ
    static GameProtocolMsg m_stCombatActionNotify;

    //����BUFF���ͻ��˵�֪ͨ
    static GameProtocolMsg m_stCombatAddBuffNotify;
};

#endif
