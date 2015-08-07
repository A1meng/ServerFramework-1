
#include "GameProtocol.hpp"
#include "LogAdapter.hpp"
#include "ZoneErrorNumDef.hpp"
#include "BattlefieldObj.hpp"
#include "CombatUnitObj.hpp"
#include "CombatUtility.hpp"
#include "Random.hpp"
#include "AStarPathUtility.hpp"
#include "ModuleHelper.hpp"

#include "CombatUnitAI.hpp"

using namespace ServerLib;

int CCombatUnitAI::m_iBattlefiledObjID = -1;
int CCombatUnitAI::m_uiActiveUin = 0;
int CCombatUnitAI::m_iActionUnitID = -1;
int CCombatUnitAI::m_iMapID = 0;
int CCombatUnitAI::m_iUnitAIID = 0;
int CCombatUnitAI::m_iUseSkillID = 0;

//ս��AI��Ч�ƶ�������
int CCombatUnitAI::m_iValidPosNum = 0;
FightAIPosValue CCombatUnitAI::m_astValidPos[MAX_FIGHT_AI_POS_NUM];
int CCombatUnitAI::m_iMovePosIndex = 0;

CCombatUnitAI::CCombatUnitAI()
{

}

CCombatUnitAI::~CCombatUnitAI()
{

}

int CCombatUnitAI::DoActionUnitAI(int iBattlefiledObjID, int iActionUnitID)
{
    m_iBattlefiledObjID = iBattlefiledObjID;
    m_iActionUnitID = iActionUnitID;

    m_uiActiveUin = 0;
    m_iMapID = 0;
    m_iUnitAIID = 0;
    m_iUseSkillID = 0;
    
    m_iValidPosNum = 0;
    memset(m_astValidPos, 0, sizeof(m_astValidPos));
    m_iMovePosIndex = 0;

    //��ȡս����Ϣ
    CBattlefieldObj* pstBattlefieldObj = CCombatUtility::GetBattlefiledObj(m_iBattlefiledObjID);
    if(!pstBattlefieldObj)
    {
        LOGERROR("Failed to get battlefield obj, invalid battlefield id %d\n", m_iBattlefiledObjID);
        return T_ZONE_SYSTEM_PARA_ERR;
    }

    //��ȡ�����ж��ߵ�uin
    m_uiActiveUin = pstBattlefieldObj->GetActiveUin();

    //��ȡս����ͼ�ļ�ID
    m_iMapID = pstBattlefieldObj->GetMapID();

    //��ȡ��ǰ�ж���ս����λ��Ϣ
    CCombatUnitObj* pstActionUnitObj = CCombatUtility::GetCombatUnitObj(m_iActionUnitID);
    if(!pstActionUnitObj)
    {
        LOGERROR("Failed to get combat unit obj, unit id %d, uin %u\n", m_iActionUnitID, m_uiActiveUin);
        return T_ZONE_SYSTEM_PARA_ERR;
    }

    if(pstActionUnitObj->GetMotorDistance() >= 0)
    {
        //ս����λ��ǰ��������ģʽ
        return T_SERVER_SUCESS;
    }

    //�����ж�AI��ID
    m_iUnitAIID = pstActionUnitObj->GetFightAI();

    //��ȡ�ж�AI������
    const SFightUnitAIConfig* pstAIConfig = FightUnitAICfgMgr().GetConfig(m_iUnitAIID);
    if(!pstAIConfig)
    {
        LOGERROR("Failed to get fight unit AI config, invalid AI id %d\n", m_iUnitAIID);
        return T_ZONE_SYSTEM_INVALID_CFG;
    }

    //��ȡ��λ�����п��ƶ�λ��
    int iRet = GetAllValidMovePos(*pstBattlefieldObj, *pstActionUnitObj);
    if(iRet)
    {
        LOGERROR("Failed to get all valid move pos, ret %d, uin %u\n", iRet, m_uiActiveUin);
        return iRet;
    }

    //�������п��ƶ�λ�õĹ�����ֵ
    iRet = ProcessAttackValue(*pstBattlefieldObj, *pstActionUnitObj, *pstAIConfig);
    if(iRet)
    {
        LOGERROR("Failed to process attack value, ret %d, uin %u\n", iRet, m_uiActiveUin);
        return iRet;
    }

    //�������п��ƶ�λ�õķ�����ֵ
    iRet = ProcessDefenceValue(*pstBattlefieldObj, *pstActionUnitObj, *pstAIConfig);
    if(iRet)
    {
        LOGERROR("Failed to process defence value, ret %d, uin %u\n", iRet, m_uiActiveUin);
        return iRet;
    }

    //�������п��ƶ�λ�õ��ƶ���ֵ
    iRet = ProcessMoveValue(*pstBattlefieldObj, *pstActionUnitObj, *pstAIConfig);
    if(iRet)
    {
        LOGERROR("Failed to process move value, ret %d, uin %u\n", iRet, m_uiActiveUin);
        return iRet;
    }

    //��ȡĿ���ƶ���λ
    if(m_iValidPosNum == 0)
    {
        LOGERROR("Failed to get AI unit move pos, invalid pos num zero, uin %u\n", m_uiActiveUin);
        return T_ZONE_SYSTEM_PARA_ERR;
    }

    int iMaxValue = 100*(m_astValidPos[0].iAttackValue + m_astValidPos[0].iDefenceValue) + m_astValidPos[0].iMoveValue;
    int iCurrentValue = 0;
    for(int i=0; i<m_iValidPosNum; ++i)
    {
        iCurrentValue = 100*(m_astValidPos[i].iAttackValue + m_astValidPos[i].iDefenceValue) + m_astValidPos[i].iMoveValue;
        if(iMaxValue <= iCurrentValue)
        {
            iMaxValue = iCurrentValue;
            m_iMovePosIndex = i;
        }
    }

    return T_SERVER_SUCESS;
}

//��ȡAI�ƶ���·��
const UnitPath& CCombatUnitAI::GetMoveTargetPath()
{
    static UnitPath stMovePath;
    stMovePath.Clear();

    CBattlefieldObj* pstBattlefieldObj = CCombatUtility::GetBattlefiledObj(m_iBattlefiledObjID);
    if(!pstBattlefieldObj)
    {
        LOGERROR("Failed to get battlefield obj, id %d, uin %u\n", m_iBattlefiledObjID, m_uiActiveUin);
        return stMovePath;
    }

    CCombatUnitObj* pstActionUnitObj = CCombatUtility::GetCombatUnitObj(m_iActionUnitID);
    if(!pstActionUnitObj)
    {
        LOGERROR("Failed to get combat unit obj, unit id %d, uin %u\n", m_iActionUnitID, m_uiActiveUin);
        return stMovePath;
    }

    int iMotorDistance = pstActionUnitObj->GetMotorDistance();
    if(iMotorDistance >= 0)
    {
        if(!pstBattlefieldObj->IsPosCanWalk(pstActionUnitObj->GetMotorTargetPos(),m_iActionUnitID))
        {
            //����ģʽĿ��㲻������,ȡ������ģʽ
            pstActionUnitObj->SetMotorMode(TUNITPOSITION(-1,0), -1);
            return stMovePath;
        }

        //ʹ�û���ģʽ
        m_iMovePosIndex = 0;
        m_astValidPos[m_iMovePosIndex].iAttackValue = 1;    //���ò�Ϊ0�����ж����ٴ������ģʽ
        m_astValidPos[m_iMovePosIndex].stPos.iPosX = pstActionUnitObj->GetMotorTargetPos().iPosX;
        m_astValidPos[m_iMovePosIndex].stPos.iPosY = pstActionUnitObj->GetMotorTargetPos().iPosY;
    }

    const TUNITPOSITION& stSrcPos = pstActionUnitObj->GetUnitPosition();
    const TUNITPOSITION& stDestPos = m_astValidPos[m_iMovePosIndex].stPos;

    if(stSrcPos.iPosX==stDestPos.iPosX && stSrcPos.iPosY==stDestPos.iPosY)
    {
        return stMovePath;
    }

    //��A*Ѱ·���ҳ�·��
    std::vector<TUNITPOSITION> vPath;
    if(!CAStarPathUtility::FindAStarPath(*pstBattlefieldObj, pstBattlefieldObj->GetBattlePathManager(), stSrcPos, 
                                         stDestPos, pstActionUnitObj->GetUnitSize(), vPath))
    {
        if(iMotorDistance >= 0)
        {
            //����ģʽ�����߲���Ŀ���,ȡ������ģʽ
            pstActionUnitObj->SetMotorMode(TUNITPOSITION(-1,0), -1);
        }

        //��������
        return stMovePath;
    }

    unsigned uMaxMoveDistance = pstBattlefieldObj->GetMaxMoveDistance(*pstActionUnitObj);
    for(unsigned i=0; i<vPath.size()&&i<uMaxMoveDistance; ++i)
    {
        UnitPosition* pstPos = stMovePath.add_stposes();
        pstPos->set_iposx(vPath[i].iPosX);
        pstPos->set_iposy(vPath[i].iPosY);
    }

    return stMovePath;
}

//��ǰִ�е�AI�Ƿ���Թ���
bool CCombatUnitAI::CanDoAttack()
{
    return (m_astValidPos[m_iMovePosIndex].iAttackValue > 0);
}

//��ǰʹ�õļ���
int CCombatUnitAI::GetUseSkill()
{
    return m_iUseSkillID;
}

//��ǰ������Ŀ��λ��
const TUNITPOSITION& CCombatUnitAI::GetTargetUnitPos()
{
    return m_astValidPos[m_iMovePosIndex].stTargetPos;
}

//��ȡ��ǰ���п��ƶ���ļ���
int CCombatUnitAI::GetAllValidMovePos(CBattlefieldObj& stBattlefieldObj, CCombatUnitObj& stActionObj)
{
    GetNearByValidMovePos(stBattlefieldObj, stActionObj.GetUnitPosition(), stBattlefieldObj.GetMaxMoveDistance(stActionObj));

    return T_SERVER_SUCESS;
}

//��ȡ��ǰ����Χ�Ŀ��ƶ���
void CCombatUnitAI::GetNearByValidMovePos(CBattlefieldObj& stBattlefieldObj, TUNITPOSITION& stPos, int iMoveStep)
{
    //�ȴ���ǰ���ƶ�����Ϣ
    bool bIsValidPos = ProcessCurrentMovePos(stBattlefieldObj, stPos);
    if(!bIsValidPos)
    {
        //��λ�ò�����Ч�ĵ㣬���������Χ��
        return;
    }

    if(iMoveStep <= 0)
    {
        //�Ѿ������������ˣ�ֱ�ӷ���
        return;
    }

    //������Χ��4�����ƶ�����Ϣ
    TUNITPOSITION astPos[4];

    //���
    astPos[0].iPosX = stPos.iPosX - 1;
    astPos[0].iPosY = stPos.iPosY;
    
    //�ұ�
    astPos[1].iPosX = stPos.iPosX + 1;
    astPos[1].iPosY = stPos.iPosY;

    //�ϱ�
    astPos[2].iPosX = stPos.iPosX;
    astPos[2].iPosY = stPos.iPosY + 1;

    //�±�
    astPos[3].iPosX = stPos.iPosX;
    astPos[3].iPosY = stPos.iPosY - 1;

    for(int i=0; i<4; ++i)
    {
        GetNearByValidMovePos(stBattlefieldObj, astPos[i], iMoveStep-1);
    }
     
    return; 
}

//�жϵ�ǰ���Ƿ���Ч�Ŀ��ƶ���
bool CCombatUnitAI::ProcessCurrentMovePos(CBattlefieldObj& stBattlefieldObj, TUNITPOSITION& stPos)
{
    CCombatUnitObj* pstCombatUnitObj = CCombatUtility::GetCombatUnitObj(m_iActionUnitID);
    if(!pstCombatUnitObj)
    {
        return false;
    }

    //��ȡ������Ҫ���ĵ�
    std::vector<TUNITPOSITION> vCheckPos;
    for(int x=0; x<=pstCombatUnitObj->GetUnitSize(); ++x)
    {
        for(int y=0; y<=pstCombatUnitObj->GetUnitSize(); ++y)
        {
            vCheckPos.push_back(TUNITPOSITION(stPos.iPosX+x, stPos.iPosY-y));
        }
    }

    //������赲
    for(unsigned i=0; i<vCheckPos.size(); ++i)
    {
        if(!stBattlefieldObj.IsPosCanWalk(vCheckPos[i], pstCombatUnitObj->GetCombatUnitID()))
        {
            return false;
        }
    }

    //��λ���Ƿ��Ѽ�����ƶ��б�
    for(int i=0; i<m_iValidPosNum; ++i)
    {
        if(m_astValidPos[i].stPos.iPosX==stPos.iPosX
           && m_astValidPos[i].stPos.iPosY==stPos.iPosY)
        {
            //�Ѽ��뵽���ƶ��б�
            return true;
        }
    }

    //��λ�ü�����ƶ��б�
    FightAIPosValue stPosValue;
    stPosValue.stPos.iPosX = stPos.iPosX;
    stPosValue.stPos.iPosY = stPos.iPosY;

    m_astValidPos[m_iValidPosNum++] = stPosValue;

    return true;
}

//�������п��ƶ�λ�õ�Ĺ�����ֵ
int CCombatUnitAI::ProcessAttackValue(CBattlefieldObj& stBattlefieldObj, CCombatUnitObj& stActionObj, const SFightUnitAIConfig& stAIConfig)
{
    //��AI�������ʹ�õļ���
    GetAIUseSkill(stBattlefieldObj, stActionObj, stAIConfig);

    int iRet = T_SERVER_SUCESS;
    int iMaxAttackValue = 0;

    //�������е�Ĺ�����ֵ
    for(int i=0; i<m_iValidPosNum; ++i)
    {
        iRet = ProcessOnePosAttackValue(stBattlefieldObj, m_astValidPos[i], iMaxAttackValue);
        if(iRet)
        {
            LOGERROR("Failed to process one pos attack value, ret %d, uin %u\n", iRet, m_uiActiveUin);
            return iRet;
        }
    }
    
    //������е���󹥻���ֵΪ0���ҷ��չ����򻻳��չ����¼���һ��
    if(iMaxAttackValue==0 && m_iUseSkillID!=0 && stActionObj.GetNormalSkillID()!=m_iUseSkillID)
    {
        m_iUseSkillID = stActionObj.GetNormalSkillID();

        //ʹ���չ����¼������е���ж���ֵ
        for(int i=0; i<m_iValidPosNum; ++i)
        {
            iRet = ProcessOnePosAttackValue(stBattlefieldObj, m_astValidPos[i], iMaxAttackValue);
            if(iRet)
            {
                LOGERROR("Failed to process one pos attack value, ret %d, uin %u\n", iRet, m_uiActiveUin);
                return iRet;
            }
        }
    }

    //������е���󹥻���ֵΪ0,�������е�
    if(iMaxAttackValue == 0)
    {
        return T_SERVER_SUCESS;
    }

    //�޳�������ֵΪ0�ĵ�
    for(int i=0; i<m_iValidPosNum;)
    {
        if(m_astValidPos[i].iAttackValue == 0)
        {
            //�����һ����������λ��
            m_astValidPos[i] = m_astValidPos[m_iValidPosNum-1];
            --m_iValidPosNum;

            continue;
        }

        ++i;
    }

    return T_SERVER_SUCESS;
}

//�������п��ƶ�λ�õ�ķ�����ֵ
int CCombatUnitAI::ProcessDefenceValue(CBattlefieldObj& stBattlefieldObj, CCombatUnitObj& stActionObj, const SFightUnitAIConfig& stAIConfig)
{
    int iRet = T_SERVER_SUCESS;

    //��������ÿ��λ��
    for(int i=0; i<m_iValidPosNum; ++i)
    {
        iRet = ProcessOnePosDefenceValue(stBattlefieldObj, stActionObj, stAIConfig, m_astValidPos[i]);
        if(iRet)
        {
            LOGERROR("Failed to process one pos defence value, ret %d, uin %u\n", iRet, m_uiActiveUin);
            return iRet;
        }
    }

    return T_SERVER_SUCESS;
}

//�������п��ƶ�λ�õ���ƶ���ֵ
int CCombatUnitAI::ProcessMoveValue(CBattlefieldObj& stBattlefieldObj, CCombatUnitObj& stActionObj, const SFightUnitAIConfig& stAIConfig)
{
    int iRet = T_SERVER_SUCESS;

    //�������е�λ��
    for(int i=0; i<m_iValidPosNum; ++i)
    {
        iRet = ProcessOnePosMoveValue(stBattlefieldObj, stActionObj, stAIConfig, m_astValidPos[i]);
        if(iRet)
        {
            LOGERROR("Failed to process one pos move value, ret %d, uin %u\n", iRet, m_uiActiveUin);
            return iRet;
        }
    }

    return T_SERVER_SUCESS;
}

//����ĳ�����ƶ�λ�õ�Ĺ�����ֵ
int CCombatUnitAI::ProcessOnePosAttackValue(CBattlefieldObj& stBattlefieldObj, FightAIPosValue& stPosValue, int& iMaxValue)
{
    if(m_iUseSkillID == 0)
    {
        stPosValue.iAttackValue = 0;
        return T_SERVER_SUCESS;
    }

    //��ȡ���ܵ�����
    const SFightUnitSkillConfig* pstSkillConfig = FightSkillCfgMgr().GetConfig(m_iUseSkillID);
    if(!pstSkillConfig)
    {
        LOGERROR("Failed to get fight skill config, invalid skill id %d\n", m_iUseSkillID);
        return T_ZONE_SYSTEM_INVALID_CFG;
    }

    //��ȡ���ܵ����зֲ�����
    const SSkillRangeConfig* pstTargetRangeConfig = SkillRangeCfgMgr().GetConfig(pstSkillConfig->iTargetRangeID);
    if(!pstTargetRangeConfig)
    {
        LOGERROR("Failed to get skill range config, invalid range id %d\n", pstSkillConfig->iTargetRangeID);
        return T_ZONE_SYSTEM_INVALID_CFG;
    }

    //�������пɹ�����λ��
    TUNITPOSITION stTmpPos;
    int iAttackValue = 0;
    for(int i=0; i<pstTargetRangeConfig->iRangeNum; ++i)
    {
        if(pstTargetRangeConfig->aiRangeInfo[i] == 0)
        {
            continue;
        }

        //��ȡArea��
        const SSkillAreaConfig* pstAreaConfig = SkillAreaCfgMgr().GetConfig(i+1+pstTargetRangeConfig->iRangeAreaParam);
        if(!pstAreaConfig)
        {
            LOGERROR("Failed to get skill area config, invalid area id %d\n", i+1+pstTargetRangeConfig->iRangeAreaParam);
            return T_ZONE_SYSTEM_INVALID_CFG;
        }

        //����Area�е�����λ��
        for(int j=0; j<pstAreaConfig->iAreaPosNum; ++j)
        {
            stTmpPos.iPosX = stPosValue.stPos.iPosX + pstAreaConfig->astTargetPosInfo[j].iPosX;
            stTmpPos.iPosY = stPosValue.stPos.iPosY + pstAreaConfig->astTargetPosInfo[j].iPosY;

            //todo jasonxiong4 �ű�����
            //iAttackValue = CModuleHelper::GetStoryFramework()->DoCombatAIScript(m_uiActiveUin, stBattlefieldObj.GetCrossID(), m_iUseSkillID, m_iActionUnitID, stPosValue.stPos,
            //                                                                    stTmpPos, pstTargetRangeConfig->aiRangeInfo[i], pstDamageRangeConfig->aiRangeInfo[i]);
            if(stPosValue.iAttackValue < iAttackValue)
            {
                stPosValue.iAttackValue = iAttackValue;
                stPosValue.stTargetPos.iPosX = stTmpPos.iPosX;
                stPosValue.stTargetPos.iPosY = stTmpPos.iPosY;
            }
        }
    }

    if(stPosValue.iAttackValue > iMaxValue)
    {
        iMaxValue = stPosValue.iAttackValue;
    }

    return T_SERVER_SUCESS;
}

//����ĳ�����ƶ�λ�õ�ķ�����ֵ
int CCombatUnitAI::ProcessOnePosDefenceValue(CBattlefieldObj& stBattlefiledObj, CCombatUnitObj& stActionObj, const SFightUnitAIConfig& stAIConfig, FightAIPosValue& stPosValue)
{
    //��ȡ���еĵ���
    std::vector<int> vEnemyUnits;
    stBattlefiledObj.GetEnemyUnits(stActionObj.GetCombatUnitID(), vEnemyUnits);

    static int iValidEnemyNum = 0;
    static int aiDefenceValue[MAX_CAMP_FIGHT_UNIT_NUM];

    iValidEnemyNum = 0;

    for(unsigned i=0; i<vEnemyUnits.size(); ++i)
    {
        //��ȡ��������
        CCombatUnitObj* pstEnemyObj = CCombatUtility::GetCombatUnitObj(vEnemyUnits[i]);
        if(!pstEnemyObj)
        {
            continue;
        }

        //��ȡ�չ�
        const SFightUnitSkillConfig* pstSkillConfig = FightSkillCfgMgr().GetConfig(pstEnemyObj->GetNormalSkillID());
        if(!pstSkillConfig)
        {
            LOGERROR("Failed to get normal skill config, invalid skill id %d\n", pstEnemyObj->GetNormalSkillID());
            return T_ZONE_SYSTEM_INVALID_CFG;
        }

        //�жϵ����Ƿ��ܴ���
        std::vector<TUNITPOSITION> vCheckPos;
        for(int x=0; x<=stActionObj.GetUnitSize(); ++x)
        {
            for(int y=0; y<=stActionObj.GetUnitSize(); ++y)
            {
                vCheckPos.push_back(TUNITPOSITION(stPosValue.stPos.iPosX+x, stPosValue.stPos.iPosY-y));
            }
        }
    
        int iDistance = 0;
        for(unsigned j=0; j<vCheckPos.size(); ++j)
        {
            iDistance = CCombatUtility::GetAttackDistance(pstEnemyObj->GetUnitPosition(), vCheckPos[j], pstSkillConfig->iTargetRangeID);
            if(iDistance != 0)
            {
                break;
            }
        }

        if(iDistance == 0)
        {
            //��λ�ô򲻵�
            continue;
        }

        //���Դ򵽣������˺�����
        //����Ч��
        int iBaseEffect = pstSkillConfig->iDamageBaseNum;
        if(!pstEnemyObj->HasBuffOfSameType(pstSkillConfig->iTargetBuff))
        {
            iBaseEffect += pstSkillConfig->iSkillBuffValue;
        }

        //������������

        //��ʽ����
        int iSkillHit = CCombatUtility::GetSkillHit(*pstEnemyObj, *pstSkillConfig, iDistance);
        //��ʽ����
        int iSkillDodge = CCombatUtility::GetSkillDodge(*pstEnemyObj, stActionObj, *pstSkillConfig);
        if(iSkillHit+iSkillDodge < 0)
        {
            LOGERROR("Failed to process skill defence value, invalid skillhit and skilldodge, uin %u!\n", m_uiActiveUin);

            return T_ZONE_SYSTEM_PARA_ERR;
        }

        //���㹥������
        //����ǿ��
        int iAttackStrength = CCombatUtility::GetAttackStrength(*pstEnemyObj, *pstSkillConfig);
        //����ǿ��
        int iDefenceStrength = CCombatUtility::GetDefenceStrength(*pstEnemyObj, stActionObj, *pstSkillConfig);
        if(iAttackStrength+iDefenceStrength <= 0)
        {
            LOGERROR("Failed to process skill defence value, invalid attack strength and defence strength, uin %u\n", m_uiActiveUin);
            
            return T_ZONE_SYSTEM_PARA_ERR;
        }

        //�������Ч��
        int iAttackValue = 0;
        if(iSkillHit+iSkillDodge > 0)
        {
            iAttackValue = iBaseEffect*iSkillHit*iAttackStrength/((iSkillHit+iSkillDodge)*(iAttackStrength+iDefenceStrength));
        }

        //����AI���˺�����
        iAttackValue = iAttackValue*stAIConfig.aiAIAttribute[COMBAT_UNIT_AI_BEIJI]/100;
        if(iAttackValue > 0)
        {
            aiDefenceValue[iValidEnemyNum++] = iAttackValue;
        }
    }

    if(iValidEnemyNum == 0)
    {
        stPosValue.iDefenceValue = 0;

        return T_SERVER_SUCESS;
    }

    //����ƽ��ֵ
    int iTotalValue = 0;
    for(int i=0; i<iValidEnemyNum; ++i)
    {
        iTotalValue += aiDefenceValue[i];
    }

    stPosValue.iDefenceValue = iTotalValue / iValidEnemyNum;

    return T_SERVER_SUCESS;
}

//����ĳ�����ƶ�λ�õ���ƶ���ֵ
int CCombatUnitAI::ProcessOnePosMoveValue(CBattlefieldObj& stBattlefiledObj, CCombatUnitObj& stActionObj, const SFightUnitAIConfig& stAIConfig, FightAIPosValue& stPosValue)
{
    //��ȡ���еĵ���
    std::vector<int> vEnemyUnits;
    stBattlefiledObj.GetEnemyUnits(stActionObj.GetCombatUnitID(), vEnemyUnits);

    //��ȡ��ǰ������˵�ƽ������
    int iCurrentDistance = GetEnemyDisance(stActionObj.GetUnitPosition(), vEnemyUnits);

    //��ȡ�ƶ�Ŀ�������˵�ƽ������
    int iMoveDistance = GetEnemyDisance(stPosValue.stPos, vEnemyUnits);

    //�����ƶ���ֵ
    stPosValue.iMoveValue = (iCurrentDistance - iMoveDistance)*stAIConfig.aiAIAttribute[COMBAT_UNIT_AI_ZHUIJI];

    //�����λ���л��ز��ҿɼ����۳����صļ�ֵ����
    CCombatTrapObj* pstTrapObj = stBattlefiledObj.GetTrapByPos(stPosValue.stPos);
    if(pstTrapObj && stBattlefiledObj.IsTrapVisible(stActionObj,*pstTrapObj))
    {
        //��λ���пɼ�����
        stPosValue.iMoveValue -= pstTrapObj->GetConfig()->iTrapValue;
    }

    return T_SERVER_SUCESS;
}

//��ȡ��ǰʹ�õļ���
void CCombatUnitAI::GetAIUseSkill(CBattlefieldObj& stBattlefieldObj, CCombatUnitObj& stActionObj, const SFightUnitAIConfig& stAIConfig)
{
    std::vector<int> vValidSkills;
    stActionObj.GetValidFightSkills(vValidSkills);

    int iRandomRange = 0;
    for(unsigned i=0; i<vValidSkills.size(); ++i)
    {
        iRandomRange += stAIConfig.aiAIAttribute[COMBAT_UNIT_AI_SKILL1+i];
    }

    if(iRandomRange == 0)
    {
        return;
    }

    int iRandNum = CRandomCalculator::GetRandomNumberInRange(iRandomRange);
    for(unsigned i=0; i<vValidSkills.size(); ++i)
    {
        if(iRandNum<=stAIConfig.aiAIAttribute[COMBAT_UNIT_AI_SKILL1+i])
        {
            m_iUseSkillID = vValidSkills[i];
            break;
        }

        iRandNum -= stAIConfig.aiAIAttribute[COMBAT_UNIT_AI_SKILL1+i];
    }

    //�жϼ����Ƿ��ܹ�ʹ��
    const SFightUnitSkillConfig* pstSkillConfig = FightSkillCfgMgr().GetConfig(m_iUseSkillID);
    if(!pstSkillConfig)
    {
        LOGERROR("Failed to get fight skill, skill id %d\n", m_iUseSkillID);
        return;
    }

    //�ж��Ƿ�CD��
    if(stActionObj.GetSkillCDRound(m_iUseSkillID) != 0)
    {
        //����ʹ�ã������չ�
        m_iUseSkillID = vValidSkills[0];
        return;
    }

    return;
}

//���㼼�ܵĹ�����ֵ
int CCombatUnitAI::ProcessSkillAttackValue(CCombatUnitObj& stActionObj, CCombatUnitObj& stTargetObj, const SFightUnitAIConfig& stAIConfig, 
                                           const SFightUnitSkillConfig& stSkillConfig, bool bIsToEnemy, const TUNITPOSITION& stPos)
{
    int iAttackValue = 0;

    if(bIsToEnemy)
    {
        //����Ե��˺�
        
        //����Ч��
        int iBaseEffect = stSkillConfig.iDamageBaseNum;
        if(!stTargetObj.HasBuffOfSameType(stSkillConfig.iTargetBuff))
        {
            iBaseEffect += stSkillConfig.iSkillBuffValue;
        }

        //�������
        std::vector<TUNITPOSITION> vCheckPos;
        for(int x=0; x<=stTargetObj.GetUnitSize(); ++x)
        {
            for(int y=0; y<=stTargetObj.GetUnitSize(); ++y)
            {
                vCheckPos.push_back(TUNITPOSITION(stTargetObj.GetUnitPosition().iPosX+x, stTargetObj.GetUnitPosition().iPosY-y));
            }
        }
    
        int iDistance = 0;
        for(unsigned i=0; i<vCheckPos.size(); ++i)
        {
            iDistance = CCombatUtility::GetAttackDistance(stPos, vCheckPos[i], stSkillConfig.iTargetRangeID);
            if(iDistance != 0)
            {
                break;
            }
        }

        //���Ͼ�����˺�Ч����Ӱ��
        const SSkillRangeConfig* pstRangeConfig = SkillRangeCfgMgr().GetConfig(stSkillConfig.iDamageRangeID);
        if(!pstRangeConfig)
        {
            LOGERROR("Failed to get skill range config, skill id %d, range id %d, uin %u\n", stSkillConfig.iConfigID, stSkillConfig.iDamageRangeID, m_uiActiveUin);
            return T_ZONE_SYSTEM_INVALID_CFG;
        }

        int iRangeIndex = iDistance;
        if(iRangeIndex >= MAX_SKILL_RANGE_INFO_NUM)
        {
            //����MAX_SKILL_RANGE_INFO_NUM���⺬�壬������MAX_SKILL_RANGE_INFO_NUM-1
            iRangeIndex = MAX_SKILL_RANGE_INFO_NUM-1;
        }
    
        //����ת�����±���Ҫ��1
        iRangeIndex = iRangeIndex - 1;

        iBaseEffect = iBaseEffect*pstRangeConfig->aiRangeInfo[iRangeIndex]/10000;

        //������������

        //��ʽ����
        int iSkillHit = CCombatUtility::GetSkillHit(stActionObj,stSkillConfig, iDistance);
        //��ʽ����
        int iSkillDodge = CCombatUtility::GetSkillDodge(stActionObj, stTargetObj, stSkillConfig);
        if(iSkillHit+iSkillDodge <= 0)
        {
            LOGERROR("Failed to process skill attack value, invalid skillhit and skilldodge, uin %u!\n", m_uiActiveUin);

            return T_ZONE_SYSTEM_PARA_ERR;
        }

        //���㹥������
        //����ǿ��
        int iAttackStrength = CCombatUtility::GetAttackStrength(stActionObj, stSkillConfig);
        //����ǿ��
        int iDefenceStrength = CCombatUtility::GetDefenceStrength(stActionObj, stTargetObj, stSkillConfig);
        if(iAttackStrength+iDefenceStrength <= 0)
        {
            LOGERROR("Failed to process skill attack value, invalid attack strength and defence strength, uin %u\n", m_uiActiveUin);
            
            return T_ZONE_SYSTEM_PARA_ERR;
        }

        //�������Ч��
        iAttackValue = iBaseEffect*iSkillHit*iAttackStrength/((iSkillHit+iSkillDodge)*(iAttackStrength+iDefenceStrength));
        iAttackValue = (iAttackValue<=0) ? 1 : (iAttackValue+1);

        //Ŀ���Ƿ���Ա���ɱ
        if(stTargetObj.GetFightAttr(FIGHT_ATTR_HP) <= iAttackValue)
        {
            //Ŀ���ܹ�����ɱ
            iAttackValue = iAttackValue*stAIConfig.aiAIAttribute[COMBAT_UNIT_AI_JISHA]/100;
        }

        //�Ƿ��Լ�����󹥻�Ŀ��
        if(stActionObj.GetLastAttackUnitID() == stTargetObj.GetCombatUnitID())
        {
            //���Լ�����󹥻�Ŀ��
            iAttackValue = iAttackValue * stAIConfig.aiAIAttribute[COMBAT_UNIT_AI_JIAOSHOU]/100;
        }
        
        CBattlefieldObj* pstBattlefieldObj = CCombatUtility::GetBattlefiledObj(m_iBattlefiledObjID);
        if(!pstBattlefieldObj)
        {
            LOGERROR("Failed to get battlefield obj, invalid id %d, uin %u\n", m_iBattlefiledObjID, m_uiActiveUin);
            return T_ZONE_SYSTEM_PARA_ERR;
        }

        //�Ƿ�����󹥻�Ŀ��
        if(pstBattlefieldObj->IsTeammateLastAttackUnit(stActionObj.GetCombatUnitID(), stTargetObj.GetCombatUnitID()))
        {
            //�ǻ�����󹥻�Ŀ��
            iAttackValue = iAttackValue*stAIConfig.aiAIAttribute[COMBAT_UNIT_AI_HEJI]/100;
        }
    }
    else
    {
        //�����ѷ�����
        int iTargetMaxHP = stTargetObj.GetFightAttr(FIGHT_ATTR_HPMAX);
        int iTargetHP = stTargetObj.GetFightAttr(FIGHT_ATTR_HP);

        iAttackValue = (-stSkillConfig.iDamageBaseNum)*(iTargetMaxHP-iTargetHP)/iTargetMaxHP;

        if(!stTargetObj.HasBuffOfSameType(stSkillConfig.iTargetBuff))
        {
            iAttackValue += stSkillConfig.iSkillBuffValue;
        }
    }

    return iAttackValue;
}

//��ȡλ�õ����е���λ�õ�ƽ������
int CCombatUnitAI::GetEnemyDisance(const TUNITPOSITION& stPos, std::vector<int>& vEnemyUnits)
{
    if(vEnemyUnits.size() == 0)
    {
        return 0;
    }

    CBattlefieldObj* pstBattlefieldObj = CCombatUtility::GetBattlefiledObj(m_iBattlefiledObjID);
    if(!pstBattlefieldObj)
    {
        return 0;
    }

    int iTotalDistance = 0;
    for(unsigned i=0; i<vEnemyUnits.size(); ++i)
    {
        CCombatUnitObj* pstEnemyObj = CCombatUtility::GetCombatUnitObj(vEnemyUnits[i]);
        if(!pstEnemyObj)
        {
            LOGERROR("Failed to get enemy obj, invalid id %d, uin %u\n", vEnemyUnits[i], m_uiActiveUin);
            return 0;
        }

        TUNITPOSITION& stEnemyPos = pstEnemyObj->GetUnitPosition();

        //iTotalDistance += ABS(stPos.iPosX,stEnemyPos.iPosX)+ABS(stPos.iPosY,stEnemyPos.iPosY); 
        iTotalDistance += GetWalkDistance(*pstBattlefieldObj, stPos, stEnemyPos);
    }

    return iTotalDistance/(int)vEnemyUnits.size();
}

//��ȡ����֮������ߵľ���
int CCombatUnitAI::GetWalkDistance(CBattlefieldObj& stBattlefieldObj, const TUNITPOSITION& stCurPos, const TUNITPOSITION& stTargetPos)
{
    //��ȡԭ���ս����λ��С
    int iSize = 0;

    CCombatUnitObj* pstUnitObj = stBattlefieldObj.GetCombatUnitByPos(stCurPos);
    if(pstUnitObj)
    {
        iSize = pstUnitObj->GetUnitSize();
    }

    //��A*Ѱ·���ҳ�·��
    std::vector<TUNITPOSITION> vPath;
    if(!CAStarPathUtility::FindAStarPath(stBattlefieldObj, stBattlefieldObj.GetBattlePathManager(), stCurPos, stTargetPos, iSize, vPath))
    {
        //��������
        return 0;
    }

    //��ȥ�����ĵ�ǰ��
    return vPath.size();
}

