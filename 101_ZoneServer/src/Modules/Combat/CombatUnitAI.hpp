
#ifndef __COMBAT_UNIT_AI_HPP__
#define __COMBAT_UNIT_AI_HPP__

#include <vector>

#include "GameProtocol.hpp"
#include "GameConfigDefine.hpp"
#include "ModuleShortcut.hpp"

//AI���ƶ���λ�õļ�ֵ��Ϣ
struct FightAIPosValue
{
    TUNITPOSITION stPos;        //���ƶ�����λ�õ���Ϣ
    TUNITPOSITION stTargetPos;  //��ѹ������λ����Ϣ
    int iAttackValue;           //�ж���Ĺ�����ֵ
    int iDefenceValue;          //�ж���ķ�����ֵ
    int iMoveValue;             //�ж�����ƶ���ֵ

    FightAIPosValue()
    {
       stPos.iPosX = -1;
       stTargetPos.iPosX = -1;

       iAttackValue = 0;
       iDefenceValue = 0;
       iMoveValue = 0;
    };
};

//���зǿ��Ƶ�λ��AI�߼�����
class CCombatUnitAI
{
public:
    CCombatUnitAI();
    ~CCombatUnitAI();

    static int DoActionUnitAI(int iBattlefiledObjID, int iActionUnitID);

    //��ȡAI�ƶ���·��
    static const UnitPath& GetMoveTargetPath();

    //��ǰִ�е�AI�Ƿ���Թ���
    static bool CanDoAttack();

    //��ǰʹ�õļ���
    static int GetUseSkill();

    //��ǰ������Ŀ�굥λ
    static const TUNITPOSITION& GetTargetUnitPos();

private:

    //��ȡ��ǰ���п��ƶ���ļ���
    static int GetAllValidMovePos(CBattlefieldObj& stBattlefieldObj, CCombatUnitObj& stActionObj);

    //��ȡ��ǰ����Χ�Ŀ��ƶ���
    static void GetNearByValidMovePos(CBattlefieldObj& stBattlefieldObj, TUNITPOSITION& stPos, int iMoveStep);

    //�жϵ�ǰ���Ƿ���Ч�Ŀ��ƶ���
    static bool ProcessCurrentMovePos(CBattlefieldObj& stBattlefieldObj, TUNITPOSITION& stPos);

    //�������п��ƶ�λ�õ�Ĺ�����ֵ
    static int ProcessAttackValue(CBattlefieldObj& stBattlefieldObj, CCombatUnitObj& stActionObj, const SFightUnitAIConfig& stAIConfig);

    //�������п��ƶ�λ�õ�ķ�����ֵ
    static int ProcessDefenceValue(CBattlefieldObj& stBattlefieldObj, CCombatUnitObj& stActionObj, const SFightUnitAIConfig& stAIConfig);

    //�������п��ƶ�λ�õ���ƶ���ֵ
    static int ProcessMoveValue(CBattlefieldObj& stBattlefieldObj, CCombatUnitObj& stActionObj, const SFightUnitAIConfig& stAIConfig);

    //����ĳ�����ƶ�λ�õ�Ĺ�����ֵ
    static int ProcessOnePosAttackValue(CBattlefieldObj& stBattlefieldObj, FightAIPosValue& stPosValue, int& iMaxValue);

    //����ĳ�����ƶ�λ�õ�ķ�����ֵ
    static int ProcessOnePosDefenceValue(CBattlefieldObj& stBattlefiledObj, CCombatUnitObj& stActionObj, const SFightUnitAIConfig& stAIConfig, FightAIPosValue& stPosValue);

    //����ĳ�����ƶ�λ�õ���ƶ���ֵ
    static int ProcessOnePosMoveValue(CBattlefieldObj& stBattlefiledObj, CCombatUnitObj& stActionObj, const SFightUnitAIConfig& stAIConfig, FightAIPosValue& stPosValue);

    //��ȡ��ǰʹ�õļ���
    static void GetAIUseSkill(CBattlefieldObj& stBattlefieldObj, CCombatUnitObj& stActionObj, const SFightUnitAIConfig& stAIConfig);

private:

    //���㼼�ܵĹ�����ֵ
    static int ProcessSkillAttackValue(CCombatUnitObj& stActionObj, CCombatUnitObj& stTargetObj, const SFightUnitAIConfig& stAIConfig, 
                                       const SFightUnitSkillConfig& stSkillConfig, bool bIsToEnemy, const TUNITPOSITION& stPos);

    //��ȡλ�õ����е���λ�õ�ƽ������
    static int GetEnemyDisance(const TUNITPOSITION& stPos, std::vector<int>& vEnemyUnits);

    //��ȡ����֮������ߵľ���
    static int GetWalkDistance(CBattlefieldObj& stBattlefieldObj, const TUNITPOSITION& stCurPos, const TUNITPOSITION& stTargetPos);

public:

    //��ǰִ��AI��ս��ID
    static int m_iBattlefiledObjID;
    
    //����ս������uin
    static int m_uiActiveUin;

    //��ǰ����AI�ĵ�λID
    static int m_iActionUnitID;

    //��ǰս���ĵ�ͼID
    static int m_iMapID;

    //��ǰ�����AI��ID
    static int m_iUnitAIID;

    //AIʹ�õļ���ID
    static int m_iUseSkillID;

    //��Ч��AI�ж����ֵ��Ϣ
    static int m_iValidPosNum;
    static FightAIPosValue m_astValidPos[MAX_FIGHT_AI_POS_NUM];

    //ѡ�е��ƶ�Ŀ�������Ч�������е�λ��
    static int m_iMovePosIndex;
};

#endif
