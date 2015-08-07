

#include "GameProtocol.hpp"
#include "LogAdapter.hpp"
#include "ZoneErrorNumDef.hpp"
#include "ModuleShortcut.hpp"
#include "GameRole.hpp"
#include "ZoneObjectHelper.hpp"
#include "Random.hpp"
#include "CombatUnitObj.hpp"
#include "ZoneMsgHelper.hpp"
#include "UnitUtility.hpp"
#include "CombatUtility.hpp"
#include "CombatSkill.hpp"
#include "ZoneOssLog.hpp"
#include "DropRewardUtility.hpp"
#include "FightUnitUtility.hpp"

#include "BattlefieldObj.hpp"

using namespace ServerLib;
using namespace GameConfig;

GameProtocolMsg CBattlefieldObj::m_stRemoveBuff_Notify;

IMPLEMENT_DYN(CBattlefieldObj)

CBattlefieldObj::CBattlefieldObj()
{
    Initialize();
}

CBattlefieldObj::~CBattlefieldObj()
{
    return;
}

int CBattlefieldObj::Initialize()
{
    m_uiActiveUin = 0;
    m_uiPassiveUin = 0;

    //��Ӫ��Ϣ
    for(int i=0; i<MAX_CAMP_FIGHT_UNIT_NUM; ++i)
    {
        m_aiActiveCombatObjIndex[i] = -1;
        m_aiPassiveCombatObjIndex[i] = -1;
        m_astActivePos[i].iPosX = -1;
        m_astPassivePos[i].iPosX = -1;
    }

    //��ǰ�غ���δ���ֵ�ս����λ��Ϣ
    for(int i=0; i<MAX_CAMP_FIGHT_UNIT_NUM*2; ++i)
    {
        m_aiCanCombatUnit[i] = -1;
        m_aiActionDoneUnit[i] = -1;
    }

    //�ű�ɱ����������λ��Ϣ
    m_iActiveScriptKilledNum = 0;
    memset(m_aiActiveScriptKilledConfigID, 0, sizeof(m_aiActiveScriptKilledConfigID));

    m_bActiveNeedSetForm = false;
    m_bPassiveNeedSetForm = false;

    m_iCanCombatUnitNum = 0;
    m_iActionDoneUnitNum = 0;
    m_iActionUnit = -1;
    m_iRoleCombatUnitStatus = ROLE_COMBAT_UNIT_STAT_INVALID;

    //ս������
    m_iTrapNum = 0;
    for(int i=0; i<MAX_COMBAT_TRAP_NUM; ++i)
    {
        m_aiCombatTrapIndex[i] = -1;
    }

    //ս��Obj��ID
    m_iBattlefieldObjID = -1;

	//�ؿ�ID
	m_iCrossID = 0;

    //��ͼID
    m_iMapID = 0;

    //��ͼ��Ӧ���赲����
    m_pstBattlePathManager = NULL;

    m_iCombatResult = COMBAT_RESULT_INVALID;

    m_iCrossID = 0;

    m_iCombatRoundNum = 0;

    //ս��ս����״̬
    m_ucCombatStatus = 0;

    return T_SERVER_SUCESS;
}

//��������uin
void CBattlefieldObj::SetActiveUin(unsigned int uiUin)
{
    m_uiActiveUin = uiUin;
}

unsigned int CBattlefieldObj::GetActiveUin()
{
    return m_uiActiveUin;
}
    
//��������uin
void CBattlefieldObj::SetPassiveUin(unsigned int uiUin)
{
    m_uiPassiveUin = uiUin;
}

unsigned int CBattlefieldObj::GetPassiveUin()
{
    return m_uiPassiveUin;
}

//ս��Obj��ΨһID
void CBattlefieldObj::SetBattlefieldObjID(int iObjID)
{
    m_iBattlefieldObjID = iObjID;
}

int CBattlefieldObj::GetBattlefieldObjID()
{
    return m_iBattlefieldObjID;
}

//��ʼ��PVEս��
int CBattlefieldObj::InitPveBattlefield(CGameRoleObj& rstRoleObj, int iCrossID)
{
    //��ȡUIN
    m_uiActiveUin = rstRoleObj.GetUin();

    m_iCombatResult = COMBAT_RESULT_INVALID;

    //��ʼ��PVEս����Ϣ
    int iRet = InitBattlefieldInfo(iCrossID);
    if(iRet)
    {
        LOGERROR("Failed to init battlefield info, cross id %d\n", iCrossID);
        return iRet;
    }

    //��ʼ��PVE������Ϣ
    iRet = InitMonsterInfo(iCrossID);
    if(iRet)
    {
        LOGERROR("Failed to init pve monster info, cross id %d\n", iCrossID);
        return iRet;
    }

    //��ʼ����������ս����λ��Ϣ
    iRet = InitFightCampInfo(rstRoleObj);
    if(iRet)
    {
        LOGERROR("Failed to init active camp fight info, ret %d, uin %u\n", iRet, m_uiActiveUin);
        return iRet;
    }

    //����ս����ʼʱ��
    m_iStartTime = CTimeUtility::GetNowTime();

    m_ucCombatStatus = 0;

    return T_SERVER_SUCESS;
}

//��ʼ��PVPս��
int CBattlefieldObj::InitPVPBattlefield(CGameRoleObj& stActiveRoleObj, CGameRoleObj& stPassiveRoleObj)
{
    //��ȡUIN
    m_uiActiveUin = stActiveRoleObj.GetUin();
    m_uiPassiveUin = stPassiveRoleObj.GetUin();

    m_iCombatResult = COMBAT_RESULT_INVALID;

    //��ʼ��ս����Ϣ
    int iRet = InitBattlefieldInfo(PVP_COMBAT_CROSS_ID);
    if(iRet)
    {
        LOGERROR("Failed to init battlefield info, cross id %d\n", 100);
        return iRet;
    }

    //��ʼ����������ս����λ��Ϣ
    iRet = InitFightCampInfo(stActiveRoleObj);
    if(iRet)
    {
        LOGERROR("Failed to init active camp fight info, ret %d, uin %u\n", iRet, m_uiActiveUin);
        return iRet;
    }

    //��ʼ����������ս����λ��Ϣ
    iRet = InitFightCampInfo(stPassiveRoleObj, false);
    if(iRet)
    {
        LOGERROR("Failed to init passivce camp fight info, ret %d, uin %u\n", iRet, m_uiPassiveUin);
        return iRet;
    }

    //����ս����ʼʱ��
    m_iStartTime = CTimeUtility::GetNowTime();

    m_ucCombatStatus = 0;

    return T_SERVER_SUCESS;
}

//�������ս������Ϣ
int CBattlefieldObj::PackBattlefiledInfo(Zone_Battlefield_Notify& stNotify)
{
    //����ս��ID
    stNotify.set_icrossid(m_iCrossID);

    //����������ս����Ӫ��Ϣ
    CombatCamp* pstSelfCamp = stNotify.mutable_stcampactive();
    CombatCamp* pstOtherCamp = stNotify.mutable_stcamppassive();

    pstSelfCamp->set_uiuin(m_uiActiveUin);
    pstOtherCamp->set_uiuin(m_uiPassiveUin);

    //�������ң�������ҵ����ֺ�����
    CGameRoleObj* pstActiveRoleObj = CUnitUtility::GetRoleByUin(m_uiActiveUin);
    if(pstActiveRoleObj)
    {
        pstSelfCamp->set_strnickname(pstActiveRoleObj->GetNickName());
    }

    CGameRoleObj* pstPassiveRoleObj = CUnitUtility::GetRoleByUin(m_uiPassiveUin);
    if(pstPassiveRoleObj)
    {
        pstOtherCamp->set_strnickname(pstPassiveRoleObj->GetNickName());
    }

    int iRet = T_SERVER_SUCESS;
    for(int i=0; i<MAX_CAMP_FIGHT_UNIT_NUM; ++i)
    {
        if(m_aiActiveCombatObjIndex[i] >= 0)
        {
            iRet = PackOneCombatUnitInfo(FIGHT_CAMP_ACTIVE, m_aiActiveCombatObjIndex[i], *pstSelfCamp->add_stunits());
            if(iRet)
            {
                LOGERROR("Failed to pack one combat unit info, obj index %d, uin %d\n", m_aiActiveCombatObjIndex[i], m_uiActiveUin);
                return iRet;
            }
        }

        if(m_aiPassiveCombatObjIndex[i] >= 0)
        {
            iRet = PackOneCombatUnitInfo(FIGHT_CAMP_PASSIVE, m_aiPassiveCombatObjIndex[i], *pstOtherCamp->add_stunits());
            if(iRet)
            {
                LOGERROR("Failed to pack one combat unit info, obj index %d, uin %u\n", m_aiPassiveCombatObjIndex[i], m_uiPassiveUin);
                return iRet;
            }
        }
    }

    //������������Чλ����Ϣ
    for(int i=0; i<MAX_CAMP_FIGHT_UNIT_NUM; ++i)
    {
        if(m_astActivePos[i].iPosX < 0)
        {
            break;
        }

        UnitPosition* pstActivePos = stNotify.add_stactiveposes();
        pstActivePos->set_iposx(m_astActivePos[i].iPosX);
        pstActivePos->set_iposy(m_astActivePos[i].iPosY);
    }

    //���ر�������Чλ����Ϣ
    for(int i=0; i<MAX_CAMP_FIGHT_UNIT_NUM; ++i)
    {
        if(m_astPassivePos[i].iPosX < 0)
        {
            break;
        }

        UnitPosition* pstPassivePos = stNotify.add_stpassiveposes();
        pstPassivePos->set_iposx(m_astPassivePos[i].iPosX);
        pstPassivePos->set_iposy(m_astPassivePos[i].iPosY);
    }

    return T_SERVER_SUCESS;
}

//����ս��
void CBattlefieldObj::ClearBattlefield()
{
    //����ս����λ
    for(int i=0; i<MAX_CAMP_FIGHT_UNIT_NUM; ++i)
    {
        if(m_aiActiveCombatObjIndex[i] >= 0)
        {
            ClearOneCombatUnitObj(m_aiActiveCombatObjIndex[i]);
        }

        m_aiActiveCombatObjIndex[i] = -1;

        if(m_aiPassiveCombatObjIndex[i] >= 0)
        {
            ClearOneCombatUnitObj(m_aiPassiveCombatObjIndex[i]);
        }

        m_aiPassiveCombatObjIndex[i] = -1;
    }

    //������ص�λ
    for(int i=0; i<m_iTrapNum; ++i)
    {
        GameType<CCombatTrapObj>::Del(m_aiCombatTrapIndex[i]);
    }

    return;
}

//��ȡ��ͼID
int CBattlefieldObj::GetMapID()
{
    return m_iMapID;
}

//��������λ�û�ȡս����λ��Ϣ
CCombatUnitObj* CBattlefieldObj::GetCombatUnitByPos(const TUNITPOSITION& stPos)
{
    for(int i=0; i<MAX_CAMP_FIGHT_UNIT_NUM; ++i)
    {
        if(m_aiActiveCombatObjIndex[i] != -1)
        {
            CCombatUnitObj* pstUnitObj = CCombatUtility::GetCombatUnitObj(m_aiActiveCombatObjIndex[i]);
            if(pstUnitObj)
            {
                TUNITPOSITION& stUnitPos = pstUnitObj->GetUnitPosition();
                for(int x=0; x<=pstUnitObj->GetUnitSize(); ++x)
                {
                    for(int y=0; y<=pstUnitObj->GetUnitSize(); ++y)
                    {
                        if(stUnitPos.iPosX+x==stPos.iPosX && stUnitPos.iPosY-y==stPos.iPosY)
                        {
                            return pstUnitObj;
                        }
                    }
                }
            }
        }

        if(m_aiPassiveCombatObjIndex[i] != -1)
        {
            CCombatUnitObj* pstUnitObj = CCombatUtility::GetCombatUnitObj(m_aiPassiveCombatObjIndex[i]);
            if(pstUnitObj)
            {
                TUNITPOSITION& stUnitPos = pstUnitObj->GetUnitPosition();
                for(int x=0; x<=pstUnitObj->GetUnitSize(); ++x)
                {
                    for(int y=0; y<=pstUnitObj->GetUnitSize(); ++y)
                    {
                        if(stUnitPos.iPosX+x==stPos.iPosX && stUnitPos.iPosY-y==stPos.iPosY)
                        {
                            return pstUnitObj;
                        }
                    }
                }         
            }
        }
    }

    return NULL;
}

//����ǻ�����󹥻�����
bool CBattlefieldObj::IsTeammateLastAttackUnit(int iCastUnitID, int iTargetUnitID)
{
    int iCastCamp = GetCombatUnitCamp(iCastUnitID);

    for(int i=0; i<MAX_CAMP_FIGHT_UNIT_NUM; ++i)
    {
        if(iCastCamp == FIGHT_CAMP_ACTIVE)
        {
            if(m_aiActiveCombatObjIndex[i] < 0 || m_aiActiveCombatObjIndex[i] == iCastUnitID)
            {
                continue;
            }

            CCombatUnitObj* pstUnitObj = CCombatUtility::GetCombatUnitObj(m_aiActiveCombatObjIndex[i]);
            if(!pstUnitObj)
            {
                continue;
            }

            if(pstUnitObj->GetLastAttackUnitID() == iTargetUnitID)
            {
                return true;
            }

        }
        else
        {
            if(m_aiPassiveCombatObjIndex[i] < 0 || m_aiPassiveCombatObjIndex[i] == iCastUnitID)
            {
                continue;
            }

            CCombatUnitObj* pstUnitObj = CCombatUtility::GetCombatUnitObj(m_aiPassiveCombatObjIndex[i]);
            if(!pstUnitObj)
            {
                continue;
            }

            if(pstUnitObj->GetLastAttackUnitID() == iTargetUnitID)
            {
                return true;
            }
        }
    }

    return false;
}

//��ȡ������Ϣ
void CBattlefieldObj::GetEnemyUnits(int iCastUnitID, std::vector<int>& vEnemyUnits)
{
    int iCampType = GetCombatUnitCamp(iCastUnitID);
    
    for(int i=0; i<MAX_CAMP_FIGHT_UNIT_NUM; ++i)
    {
        if(iCampType==FIGHT_CAMP_ACTIVE && m_aiPassiveCombatObjIndex[i]>=0)
        {
            CCombatUnitObj* pstUnitObj = CCombatUtility::GetCombatUnitObj(m_aiPassiveCombatObjIndex[i]);
            if(!pstUnitObj || pstUnitObj->IsCombatUnitDead())
            {
                continue;
            }

            vEnemyUnits.push_back(m_aiPassiveCombatObjIndex[i]);
        }
        else if(iCampType==FIGHT_CAMP_PASSIVE && m_aiActiveCombatObjIndex[i]>=0)
        {
            CCombatUnitObj* pstUnitObj = CCombatUtility::GetCombatUnitObj(m_aiActiveCombatObjIndex[i]);
            if(!pstUnitObj || pstUnitObj->IsCombatUnitDead())
            {
                continue;
            }

            vEnemyUnits.push_back(m_aiActiveCombatObjIndex[i]);
        }
    }

    return;
}

//����ս����ص�BUFF
int CBattlefieldObj::DoBuffEffectByType(int iTriggerType, int iTargetUnitID, int iTriggerUnitID, int* pDamageNum)
{
    int iRet = T_SERVER_SUCESS;

    static GameProtocolMsg stDoBuffEffect_Notify;   //��ЧBUFF

    CZoneMsgHelper::GenerateMsgHead(stDoBuffEffect_Notify, MSGID_ZONE_DOBUFFEFFECT_NOTIFY);
    Zone_DoBuffEffect_Notify* pstNotify = stDoBuffEffect_Notify.mutable_m_stmsgbody()->mutable_m_stzone_dobuffeffect_notify();

    switch(iTriggerType)
    {
        case BUFF_TRIGGER_ROUNDBEGIN:
        case BUFF_TRIGGER_ROUNDEND:
            {
                //�غϿ�ʼǰ�ͻغϽ�����ս�������е�λ���ᴥ��

                //�ȴ����Լ���
                for(int i=0; i<MAX_CAMP_FIGHT_UNIT_NUM; ++i)
                {
                    if(m_aiActiveCombatObjIndex[i] < 0)
                    {
                        continue;
                    }
            
                    CCombatUnitObj* pstCombatObj = CCombatUtility::GetCombatUnitObj(m_aiActiveCombatObjIndex[i]);
                    if(!pstCombatObj)
                    {
                        continue;
                    }
            
                    //����BUFF
                    iRet = pstCombatObj->DoBuffEffectByType(m_uiActiveUin, m_iCrossID, iTriggerType, m_aiActiveCombatObjIndex[i], *pstNotify);
                    if(iRet)
                    {
                        return iRet;
                    }
                }
            
                //�ٴ���Է���
                for(int i=0; i<MAX_CAMP_FIGHT_UNIT_NUM; ++i)
                {
                    if(m_aiPassiveCombatObjIndex[i] < 0)
                    {
                        continue;
                    }
            
                    CCombatUnitObj* pstCombatObj = CCombatUtility::GetCombatUnitObj(m_aiPassiveCombatObjIndex[i]);
                    if(!pstCombatObj)
                    {
                        continue;
                    }
            
                    //����BUFF
                    iRet = pstCombatObj->DoBuffEffectByType(m_uiActiveUin, m_iCrossID, iTriggerType, m_aiPassiveCombatObjIndex[i], *pstNotify);
                    if(iRet)
                    {
                        return iRet;
                    }
                }
            }
            break;

        case BUFF_TRIGGER_ACTIONROUNDBEGIN:
        case BUFF_TRIGGER_ACTIONROUNDEND:
        case BUFF_TRIGGER_UNDERDAMAGE:
        case BUFF_TRIGGER_DODGE:
        case BUFF_TRIGGER_UNDERATTACK:
        case BUFF_TRIGGER_MOVE:
            {
                //��ȡĿ��ս����λObj
                CCombatUnitObj* pstTargetObj = CCombatUtility::GetCombatUnitObj(iTargetUnitID);
                if(!pstTargetObj)
                {
                    LOGERROR("Failed to get combat unit obj, invalid target unit id %d, uin %u\n", iTargetUnitID, m_uiActiveUin);

                    return T_ZONE_SYSTEM_PARA_ERR;
                }

                iRet = pstTargetObj->DoBuffEffectByType(m_uiActiveUin, m_iCrossID, iTriggerType, iTriggerUnitID, *pstNotify);
                if(iRet)
                {
                    LOGERROR("Failed to do buff effect, trigger type %d, unit id %d, uin %u\n", iTriggerType, iTargetUnitID, m_uiActiveUin);
                    return iRet;
                }
            }
            break;

        case BUFF_TRIGGER_PROCESSDAMAGE:
            {
                //��ȡĿ��ս����λ��Obj
                CCombatUnitObj* pstTargetObj = CCombatUtility::GetCombatUnitObj(iTargetUnitID);
                if(!pstTargetObj)
                {
                    LOGERROR("Failed to get combat unit obj, invalid target unit id %d, uin %u\n", iTargetUnitID, m_uiActiveUin);

                    return T_ZONE_SYSTEM_PARA_ERR;
                }

                iRet = pstTargetObj->DoBuffEffectByType(m_uiActiveUin, m_iCrossID, iTriggerType, iTriggerUnitID, *pstNotify, pDamageNum);
                if(iRet)
                {
                    LOGERROR("Failed to do buff effect, trigger type %d, unit id %d, uin %u\n", iTriggerType, iTargetUnitID, m_uiActiveUin);
                    return iRet;
                }
            }
            break;

        default:
            {
                LOGERROR("Failed to do battlefiled buff effect, invalid trigger type %d\n", iTriggerType);
                return T_ZONE_SYSTEM_PARA_ERR;
            }
    }

    if(pstNotify->steffects_size() != 0)
    {
        SendNotifyToBattlefield(stDoBuffEffect_Notify);
    }

    return T_SERVER_SUCESS;
}

int CBattlefieldObj::DecreaseBuffRound()
{
    //�����ͼ���ս����λBUFF�غ�������Ϣ
    static GameProtocolMsg stMsg;
    CZoneMsgHelper::GenerateMsgHead(stMsg, MSGID_ZONE_DECREASEUNITBUFF_NOTIFY);

    stMsg.mutable_m_stmsgbody()->mutable_m_stzone_decreaseunitbuff_notify()->set_icombatunitid(GetActionUnitID());
    SendNotifyToBattlefield(stMsg);

    CZoneMsgHelper::GenerateMsgHead(m_stRemoveBuff_Notify, MSGID_ZONE_REMOVEBUFF_NOTIFY);
    Zone_RemoveBuff_Notify* pstNotify = m_stRemoveBuff_Notify.mutable_m_stmsgbody()->mutable_m_stzone_removebuff_notify();

    //��ȡ��ǰս����λ
    CCombatUnitObj* pstActionObj = CCombatUtility::GetCombatUnitObj(GetActionUnitID());
    if(!pstActionObj)
    {
        LOGERROR("Failed to get combat unit obj, invalid action unit id %d\n", GetActionUnitID());
        return T_ZONE_SYSTEM_PARA_ERR;
    }

    int iRet = pstActionObj->DecreaseBuffRound(*pstNotify);
    if(iRet)
    {
        LOGERROR("Failed to decrease buff round, ret %d, uin %u\n", iRet, m_uiActiveUin);
        return iRet;
    }

    if(pstNotify->steffects_size() != 0)
    {
        SendNotifyToBattlefield(m_stRemoveBuff_Notify);
    }

    return T_SERVER_SUCESS;
}

//��ȡս��ʤ�����
bool CBattlefieldObj::GetIsActiveWin()
{
    return (m_iCombatResult == COMBAT_RESULT_ACTIVE_WIN);
}

//����ս�����,�Ƿ񼺷���ʤ
int CBattlefieldObj::SetCampActiveWin(int iIsActiveWin)
{
    if(iIsActiveWin)
    {
        //����ս����ʤ��
        m_iCombatResult = COMBAT_RESULT_ACTIVE_WIN;
    }
    else
    {
        //����ս����ʤ��
        m_iCombatResult = COMBAT_RESULT_PASSIVE_WIN;
    }

    return T_SERVER_SUCESS;
}

//��ȡ�ؿ���ID
int CBattlefieldObj::GetCrossID()
{
    return m_iCrossID;
}

//���õ�ǰ�ж���λ�ķ���
int CBattlefieldObj::SetActionUnitDirection(int iDirection)
{
    if(iDirection <= COMBAT_DIRECTION_INVALID || iDirection > COMBAT_DIRECTION_SOUTH)
    {
        //�Ƿ��ķ���ֱ�ӷ���
        return T_SERVER_SUCESS;
    }

    CCombatUnitObj* pstActionObj = CCombatUtility::GetCombatUnitObj(m_iActionUnit);
    if(!pstActionObj)
    {
        LOGERROR("Failed to get action unit obj, unit id %d, uin %u\n", m_iActionUnit, m_uiActiveUin);
        return T_ZONE_SYSTEM_PARA_ERR;
    }

    pstActionObj->SetUnitDirection(iDirection);

    return T_SERVER_SUCESS;
}

//��ȡ��Ӫս����λ����Ϣ
int CBattlefieldObj::GetCombatUnitByCamp(int iCampType, int iUnitIndex)
{
    if(iUnitIndex < 0 || iUnitIndex >= MAX_CAMP_FIGHT_UNIT_NUM)
    {
        return -1;
    }

    CCombatUnitObj* pstUnitObj = NULL;
    if(iCampType == FIGHT_CAMP_ACTIVE)
    {
        //ս����������
        if(m_aiActiveCombatObjIndex[iUnitIndex] < 0)
        {
            return -1;
        }

        pstUnitObj = CCombatUtility::GetCombatUnitObj(m_aiActiveCombatObjIndex[iUnitIndex]);
       
    }
    else if(iCampType == FIGHT_CAMP_PASSIVE)
    {
        //ս���������ܷ�
        if(m_aiPassiveCombatObjIndex[iUnitIndex] < 0)
        {
            return -1;
        }

        pstUnitObj = CCombatUtility::GetCombatUnitObj(m_aiPassiveCombatObjIndex[iUnitIndex]);
    }

    if(!pstUnitObj || pstUnitObj->IsCombatUnitDead())
    {
        return -1;
    }


    return pstUnitObj->GetCombatUnitID();
}

//ս����λ�Ļغ���
int CBattlefieldObj::GetCombatRoundNum()
{
    return m_iCombatRoundNum;
}

void CBattlefieldObj::SetCombatRoundNum(int iRoundNum)
{
    m_iCombatRoundNum = iRoundNum;
}

//����ս���ϵ�ս����λ
int CBattlefieldObj::AddCombatUnit(int iCampType, int iConfigID, int iPosX, int iPosY, int iDirection, int iUnitType, int& iUnitID)
{
    /*
    //�������ӵ�ս����λunitID
    iUnitID = -1;

    //��ȡ��������
    const SMonsterConfig* pstMonsterConfig = MonsterCfgMgr().GetConfig(iConfigID);
    if(!pstMonsterConfig)
    {
        LOGERROR("Failed to get monster config, invalid id %d\n", iConfigID);
        return T_ZONE_SYSTEM_PARA_ERR;
    }

    int* piCombatUnitObjIndex = NULL;
    if(iCampType == FIGHT_CAMP_ACTIVE)
    {
        piCombatUnitObjIndex = m_aiActiveCombatObjIndex;
    }
    else if(iCampType == FIGHT_CAMP_PASSIVE)
    {
        piCombatUnitObjIndex = m_aiPassiveCombatObjIndex;
    }

    if(!piCombatUnitObjIndex)
    {
        LOGERROR("Failed to get combat unit index , invalid camp type %d, uin %u\n", iCampType, m_uiActiveUin);
        return T_ZONE_SYSTEM_PARA_ERR;
    }

    //���ҵ�ǰ���е�index
    int iIndex = -1;
    for(int i=0; i<MAX_CAMP_FIGHT_UNIT_NUM; ++i)
    {
        if(piCombatUnitObjIndex[i] < 0)
        {
            iIndex = i;
            break;
        }
    }

    if(iIndex < 0)
    {
        return T_ZONE_SYSTEM_PARA_ERR;
    }

    //����CCombatUnitObj
    CCombatUnitObj* pstCombatUnitObj = CreateCombatUnitObj(piCombatUnitObjIndex[iIndex]);
    if(!pstCombatUnitObj)
    {
        LOGERROR("Failed to create combat unit obj!\n");
        return T_ZONE_SYSTEM_PARA_ERR;
    }

    TUNITPOSITION stPos(iPosX, iPosY);
    TUNITPOSITION stTmpPos;
    bool bFoundPos = false;
    for(int i=0; i<4; ++i)
    {
        //��
        stTmpPos.iPosX = stPos.iPosX + i;
        stTmpPos.iPosY = stPos.iPosY;
        if(IsPosCanWalk(stTmpPos))
        {
            //��λ��û���˲��ҿ�����
            bFoundPos = true;
            break;
        }
        
        //��
        stTmpPos.iPosX = stPos.iPosX - i;
        stTmpPos.iPosY = stPos.iPosY;
        if(IsPosCanWalk(stTmpPos))
        {
            //��λ��û���˲��ҿ�����
            bFoundPos = true;
            break;
        }

        //��
        stTmpPos.iPosX = stPos.iPosX;
        stTmpPos.iPosY = stPos.iPosY + i;
        if(IsPosCanWalk(stTmpPos))
        {
            //��λ��û���˲��ҿ�����
            bFoundPos = true;
            break;
        }

        //��
        stTmpPos.iPosX = stPos.iPosX;
        stTmpPos.iPosY = stPos.iPosY - i;
        if(IsPosCanWalk(stTmpPos))
        {
            //��λ��û���˲��ҿ�����
            bFoundPos = true;
            break;
        }
    }

    if(!bFoundPos)
    {
        //û�ҵ��Ϸ��ĵ�
        LOGERROR("Failed to add combat unit, can not find valid pos, uin %u\n", m_uiActiveUin);
        return T_ZONE_SYSTEM_PARA_ERR;
    }

    SFightPosInfo stTmpPosInfo;
    stTmpPosInfo.iPosX = stTmpPos.iPosX;
    stTmpPosInfo.iPosY = stTmpPos.iPosY;
    int iRet = pstCombatUnitObj->InitMonsterUnitObj(piCombatUnitObjIndex[iIndex], *pstMonsterConfig, stTmpPosInfo, iUnitType);
    if(iRet)
    {
        LOGERROR("Failed to init monster unit obj, monster id %d, ret %d, uin %u\n", iConfigID, iRet, m_uiActiveUin);
        return iRet;
    }

    iUnitID = piCombatUnitObjIndex[iIndex];

    pstCombatUnitObj->SetUnitDirection(iDirection);

    //��ӵ��ж���������
    m_aiActionDoneUnit[m_iActionDoneUnitNum++] = iUnitID;

    LOGDEBUG("Success to init one monster info, monster id %d, uin %u\n", iConfigID, m_uiActiveUin);

    //���͸��ͻ��˵���Ϣ
    static GameProtocolMsg stAddUnitNotify;
    CZoneMsgHelper::GenerateMsgHead(stAddUnitNotify, MSGID_ZONE_ADDCOMBATUNIT_NOTIFY);
    
    Zone_AddCombatUnit_Notify* pstNotify = stAddUnitNotify.mutable_m_stmsgbody()->mutable_m_stzone_addcombatunit_notify();
    pstNotify->set_icamptype(iCampType);
    
    //��װ��λ����ϸ��Ϣ
    PackOneCombatUnitInfo(iCampType, piCombatUnitObjIndex[iIndex], *pstNotify->mutable_stunit());
    
    //������Ϣ���ͻ���
    SendNotifyToBattlefield(stAddUnitNotify);

    //�ͷ�ս����λ�ĵǳ�����
    std::vector<int> vSkillIDs;
    pstCombatUnitObj->GetSkillByType(SKILL_USE_ON_STAGE, vSkillIDs);

    for(unsigned i=0; i<vSkillIDs.size(); ++i)
    {
        iRet = CCombatSkill::CastSkill(m_iBattlefieldObjID, piCombatUnitObjIndex[iIndex], pstCombatUnitObj->GetUnitPosition(), vSkillIDs[i], SKILL_USE_ON_STAGE);
        if(iRet)
        {
            LOGERROR("Failed to cast combat unit skill, unit %d, skill %d, uin %u\n", piCombatUnitObjIndex[iIndex], vSkillIDs[i], m_uiActiveUin);
            return iRet;
        }
    } 
    */ 

    return T_SERVER_SUCESS;
}

//�л�ս����λ��AI
int CBattlefieldObj::ChangeCombatUnitAI(int iCombatUnitID, int iNewAI)
{
    //��ȡս����λ
    CCombatUnitObj* pstUnitObj = CCombatUtility::GetCombatUnitObj(iCombatUnitID);
    if(!pstUnitObj)
    {
        LOGERROR("Failed to get combat unit obj, invalid id %d, uin %u\n", iCombatUnitID, m_uiActiveUin);
        return T_ZONE_GAMEHERO_NOT_EXIST;
    } 

    int iRet = pstUnitObj->ChangeFightAI(iNewAI);
    if(iRet)
    {
        LOGERROR("Failed to change combat unit AI, unit id %d, new AI %d, uin %u\n", iCombatUnitID, iNewAI, m_uiActiveUin);
        return iRet;
    }

    return T_SERVER_SUCESS;
}

//ǿ��ɱ��ս����λ
int CBattlefieldObj::KillCombatUnit(int iCombatUnitID, bool bSendNotify)
{
    //��ȡս����λ
    CCombatUnitObj* pstUnitObj = CCombatUtility::GetCombatUnitObj(iCombatUnitID);
    if(!pstUnitObj)
    {
        LOGERROR("Failed to get combat unit obj, invalid id %d, uin %u\n", iCombatUnitID, m_uiActiveUin);
        return T_ZONE_GAMEHERO_NOT_EXIST;
    } 

    int iRet = pstUnitObj->AddFightAttr(FIGHT_ATTR_HP, -pstUnitObj->GetFightAttr(FIGHT_ATTR_HP));
    if(iRet)
    {
        LOGERROR("Failed to kill combat unit, id %d, uin %u\n", iCombatUnitID, m_uiActiveUin);
        return iRet;
    }

    //ɱ����������λ������ɱ���ĵ�λ����ID
    if(GetCombatUnitCamp(iCombatUnitID) == FIGHT_CAMP_ACTIVE)
    {
        m_aiActiveScriptKilledConfigID[m_iActiveScriptKilledNum++] = pstUnitObj->GetConfigID();
    }

    if(bSendNotify)
    {
        //����ǿ��ɱ������Ϣ
        static GameProtocolMsg stKillUnitNotify;
        CZoneMsgHelper::GenerateMsgHead(stKillUnitNotify, MSGID_ZONE_KILLCOMBATUNIT_NOTIFY);
    
        Zone_KillCombatUnit_Notify* pstNotify = stKillUnitNotify.mutable_m_stmsgbody()->mutable_m_stzone_killcombatunit_notify();
        pstNotify->set_iunitid(iCombatUnitID);
    
        SendNotifyToBattlefield(stKillUnitNotify);
    }

    //���������ĵ�λ
    ClearDeadCombatUnit();

    return T_SERVER_SUCESS;
}

//��ȡս����ʼʱ��
int CBattlefieldObj::GetCombatStartTime()
{
    return m_iStartTime;
}

//�����ж��غϿ�ʼǰ�ļ���
int CBattlefieldObj::DoRoundActionSkill()
{
    //��ȡ��ǰ�ж���λ
    CCombatUnitObj* pstActionUnitObj = CCombatUtility::GetCombatUnitObj(m_iActionUnit);
    if(!pstActionUnitObj)
    {
        LOGERROR("Failed to get action unit obj, unit id %d, uin %u\n", m_iActionUnit, m_uiActiveUin);
        return T_ZONE_SYSTEM_PARA_ERR;
    }

    //��ȡս����λ���м���
    std::vector<int> vSkillIDs;
    pstActionUnitObj->GetSkillByType(SKILL_USE_ROUNDACTION, vSkillIDs);

    //�ͷż���
    int iRet = T_SERVER_SUCESS;
    for(unsigned i=0; i<vSkillIDs.size(); ++i)
    {
        iRet = CCombatSkill::CastSkill(m_iBattlefieldObjID, m_iActionUnit, pstActionUnitObj->GetUnitPosition(), vSkillIDs[i], SKILL_USE_ROUNDACTION);
        if(iRet)
        {
            LOGERROR("Failed to casst combat skill, unit %d, skill %d, uin %u\n", m_iActionUnit, vSkillIDs[i], m_uiActiveUin);
            return iRet;
        }
    }

    return T_SERVER_SUCESS;
}

//��ȡ����ƶ����룬����Χ���߼�
int CBattlefieldObj::GetMaxMoveDistance(CCombatUnitObj& stUnitObj)
{
    if(stUnitObj.GetCombatUnitStatus(COMBAT_UNIT_STATUS_NOTMOVE))
    {
        //�����ƶ�
        return 0;
    }

    int iNearByEnemyNum = 0;
    if(!stUnitObj.GetCombatUnitStatus(COMBAT_UNIT_STATUS_IGNOREJOINT) && stUnitObj.GetUnitSize()==0)
    {
        //û�����Ӻ�Χ��״̬���Ҳ��Ǵ������λ�����������Ŀ
        iNearByEnemyNum = GetNearByUnitNum(stUnitObj, true);
    }

    int iMoveDistance = (stUnitObj.GetFightAttr(FIGHT_ATTR_SPEED)/10)-iNearByEnemyNum;
    if(iMoveDistance <= 0)
    {
        iMoveDistance = 1;
    }

    return iMoveDistance;
}

//��ȡս����λ��Χ����Ӫ��λ����
int CBattlefieldObj::GetNearByUnitNum(CCombatUnitObj& stUnitObj, bool bIsEnemy)
{
    int iTotalUnitNum = 0;
    TUNITPOSITION& stPos = stUnitObj.GetUnitPosition();

    //������
    CCombatUnitObj* pstNearUnitObj = GetCombatUnitByPos(TUNITPOSITION(stPos.iPosX-1, stPos.iPosY));
    if(pstNearUnitObj && 
       ((bIsEnemy&&!IsUnitInSameCamp(stUnitObj,*pstNearUnitObj)) || (!bIsEnemy&&IsUnitInSameCamp(stUnitObj,*pstNearUnitObj))))
    {
        ++iTotalUnitNum;
    }

    //����ұ�
    pstNearUnitObj = GetCombatUnitByPos(TUNITPOSITION(stPos.iPosX+1, stPos.iPosY));
    if(pstNearUnitObj && 
       ((bIsEnemy&&!IsUnitInSameCamp(stUnitObj,*pstNearUnitObj)) || (!bIsEnemy&&IsUnitInSameCamp(stUnitObj,*pstNearUnitObj))))
    {
        ++iTotalUnitNum;
    }

    //����ϱ�
    pstNearUnitObj = GetCombatUnitByPos(TUNITPOSITION(stPos.iPosX, stPos.iPosY+1));
    if(pstNearUnitObj && 
       ((bIsEnemy&&!IsUnitInSameCamp(stUnitObj,*pstNearUnitObj)) || (!bIsEnemy&&IsUnitInSameCamp(stUnitObj,*pstNearUnitObj))))
    {
        ++iTotalUnitNum;
    }

    //����±�
    pstNearUnitObj = GetCombatUnitByPos(TUNITPOSITION(stPos.iPosX, stPos.iPosY-1));
    if(pstNearUnitObj && 
       ((bIsEnemy&&!IsUnitInSameCamp(stUnitObj,*pstNearUnitObj)) || (!bIsEnemy&&IsUnitInSameCamp(stUnitObj,*pstNearUnitObj))))
    {
        ++iTotalUnitNum;
    }

    return iTotalUnitNum;
}

//��ȡս����λб��λ�õ���Ӫ��λ����
int CBattlefieldObj::GetAngleNearByUnitNum(CCombatUnitObj& stUnitObj, bool bIsEnemy)
{
    int iTotalUnitNum = 0;
    TUNITPOSITION& stPos = stUnitObj.GetUnitPosition();

    //������±�
    CCombatUnitObj* pstNearUnitObj = GetCombatUnitByPos(TUNITPOSITION(stPos.iPosX-1, stPos.iPosY-1));
    if(pstNearUnitObj && 
       ((bIsEnemy&&!IsUnitInSameCamp(stUnitObj,*pstNearUnitObj)) || (!bIsEnemy&&IsUnitInSameCamp(stUnitObj,*pstNearUnitObj))))
    {
        ++iTotalUnitNum;
    }

    //������ϱ�
    pstNearUnitObj = GetCombatUnitByPos(TUNITPOSITION(stPos.iPosX-1, stPos.iPosY+1));
    if(pstNearUnitObj && 
       ((bIsEnemy&&!IsUnitInSameCamp(stUnitObj,*pstNearUnitObj)) || (!bIsEnemy&&IsUnitInSameCamp(stUnitObj,*pstNearUnitObj))))
    {
        ++iTotalUnitNum;
    }

    //�������
    pstNearUnitObj = GetCombatUnitByPos(TUNITPOSITION(stPos.iPosX+1, stPos.iPosY-1));
    if(pstNearUnitObj && 
       ((bIsEnemy&&!IsUnitInSameCamp(stUnitObj,*pstNearUnitObj)) || (!bIsEnemy&&IsUnitInSameCamp(stUnitObj,*pstNearUnitObj))))
    {
        ++iTotalUnitNum;
    }

    //�������
    pstNearUnitObj = GetCombatUnitByPos(TUNITPOSITION(stPos.iPosX+1, stPos.iPosY+1));
    if(pstNearUnitObj && 
       ((bIsEnemy&&!IsUnitInSameCamp(stUnitObj,*pstNearUnitObj)) || (!bIsEnemy&&IsUnitInSameCamp(stUnitObj,*pstNearUnitObj))))
    {
        ++iTotalUnitNum;
    }

    return iTotalUnitNum;
}

//ս����ĳ��λ���Ƿ��������
bool CBattlefieldObj::IsPosCanWalk(const TUNITPOSITION& stPos, int iUnitID)
{
    //��λ�ò������赲���赲���͵Ļ���
    if(!m_pstBattlePathManager->BattlefieldPosCanWalk(stPos) || GetBlockTrapByPos(stPos))
    {
        return false;
    }

    //��λ�ò������ˣ������iUnitID��������Ϊ����
    CCombatUnitObj* pstUnitObj = GetCombatUnitByPos(stPos);
    if(pstUnitObj && pstUnitObj->GetCombatUnitID()!=iUnitID)
    {
        return false;
    }

    return true;
}

//��ȡս����ͼ�赲����
const CScenePathManager& CBattlefieldObj::GetBattlePathManager()
{
    return *m_pstBattlePathManager;
}

//ս�������ӻ���,����TrapObj�����ID
int CBattlefieldObj::AddCombatTrap(int iConfigID, int iCamp, const TUNITPOSITION& stPos, int iDirection)
{
    //�Ƿ񳬹�֧�ֵ��������
    if(m_iTrapNum >= MAX_COMBAT_TRAP_NUM)
    {
        LOGERROR("Failed to add combat trap, number reach max %d, uin %u\n", MAX_COMBAT_TRAP_NUM, m_uiActiveUin);
        return -1;
    }

    //��λ���Ƿ��赲��������
    if(!IsPosCanWalk(stPos))
    {
        LOGERROR("Failed to add combat trap, pos %d:%d, battlefield id %d, uin %u\n", stPos.iPosX, stPos.iPosY, m_iCrossID, m_uiActiveUin);
        return -1;
    }

    //��λ���Ƿ��л���
    if(GetTrapByPos(stPos))
    {
        //��λ���Ѿ��л���
        LOGERROR("Failed to add combat trap, pos %d:%d already have trap, uin %u\n", stPos.iPosX, stPos.iPosY, m_uiActiveUin);
        return -1;
    }

    //���û���
    m_aiCombatTrapIndex[m_iTrapNum] = GameType<CCombatTrapObj>::Create();
    if(m_aiCombatTrapIndex[m_iTrapNum] < 0)
    {
        LOGERROR("Failed to create combat trap obj, uin %u\n", m_uiActiveUin);
        return -1;
    }

    CCombatTrapObj* pstTrapObj = GameType<CCombatTrapObj>::Get(m_aiCombatTrapIndex[m_iTrapNum]);
    if(!pstTrapObj)
    {
        LOGERROR("Failed to get combat trap obj, uin %u\n", m_uiActiveUin);
        return -1;
    }

    ++m_iTrapNum;

    //��ʼ������
    int iRet = pstTrapObj->InitTrap(iConfigID, iCamp, stPos, iDirection);
    if(iRet)
    {
        LOGERROR("Failed to init combat trap, ret %d, trap id %d, uin %u\n", iRet, iConfigID, m_uiActiveUin);
        return -1;
    }

    //�������ӻ��ص�֪ͨ
    static GameProtocolMsg stMsg;
    CZoneMsgHelper::GenerateMsgHead(stMsg, MSGID_ZONE_ADDCOMBATTRAP_NOTIFY);

    Zone_AddCombatTrap_Notify* pstNotify = stMsg.mutable_m_stmsgbody()->mutable_m_stzone_addcombattrap_notify();
    pstNotify->set_iconfigid(iConfigID);
    pstNotify->set_icamp(iCamp);
    pstNotify->set_itrapobjid(m_aiCombatTrapIndex[m_iTrapNum-1]);
    pstNotify->set_idirection(iDirection);
    pstNotify->mutable_stpos()->set_iposx(stPos.iPosX);
    pstNotify->mutable_stpos()->set_iposy(stPos.iPosY);

    SendNotifyToBattlefield(stMsg);

    return m_aiCombatTrapIndex[m_iTrapNum-1];
}

//����ս���ϵĻ���
int CBattlefieldObj::TriggerCombatTrap(int iTriggerObjID, int iTrapObjID, int iTriggerType)
{
    //��ȡTrapObj
    CCombatTrapObj* pstTrapObj = GameType<CCombatTrapObj>::Get(iTrapObjID);
    if(!pstTrapObj)
    {
        LOGERROR("Failed to get combat trap obj, obj id %d, uin %u\n", iTrapObjID, m_uiActiveUin);
        return T_ZONE_SYSTEM_PARA_ERR;
    }

    //�жϴ�������
    if(pstTrapObj->GetConfig()->iTriggerType != iTriggerType)
    {
        return T_SERVER_SUCESS;
    }

    //�������������������ֹͣ�ж����ƶ����߼�
    if(iTriggerType != TRAP_TRIGGER_PASSIVE)
    {
        CCombatUnitObj* pstUnitObj = CCombatUtility::GetCombatUnitObj(iTriggerObjID);
        if(!pstUnitObj)
        {
            LOGERROR("Failed to get trap trigger unit, obj id %d, uin %u\n", iTriggerObjID, m_uiActiveUin);
            return T_ZONE_SYSTEM_PARA_ERR;
        }

        switch(pstTrapObj->GetConfig()->iInterruptType)
        {
            case TRAP_INTERRUPT_MOVE:
                {
                    // ���ò����ƶ���״̬
                    pstUnitObj->SetCombatUnitStatus(COMBAT_UNIT_STATUS_NOTMOVE, true);
                }
                break;

            case TRAP_INTERRUPT_ACTION:
                {
                    //���ò����ж�״̬
                    pstUnitObj->SetCombatUnitStatus(COMBAT_UNIT_STATUS_NOTACTION, true);
                }
                break;

            case TRAP_INTERRUPT_ALL:
                {
                    //���ò����ƶ����Ҳ����ж�
                    pstUnitObj->SetCombatUnitStatus(COMBAT_UNIT_STATUS_NOTMOVE, true);
                    pstUnitObj->SetCombatUnitStatus(COMBAT_UNIT_STATUS_NOTACTION, true);
                }
                break;

            default:
                {
                    //������κ���Ϊ
                    ;
                }
                break;
        }
    }

    //���͸��ͻ��˵���Ϣ
    static GameProtocolMsg stMsg;
    CZoneMsgHelper::GenerateMsgHead(stMsg, MSGID_ZONE_TRIGGERTRAP_NOTIFY);

    Zone_TriggerCombatTrap_Notify* pstNotify = stMsg.mutable_m_stmsgbody()->mutable_m_stzone_triggercombattrap_notify();
    pstNotify->set_itrapobjid(iTrapObjID);
    pstNotify->set_itriggerobjid(iTriggerObjID);
    pstNotify->set_itriggertype(iTriggerType);
    pstNotify->set_iinterrupttype(pstTrapObj->GetConfig()->iInterruptType);

    SendNotifyToBattlefield(stMsg);

    //todo jasonxiong5 ���¿����ű�
    //ִ�нű�����
    //CModuleHelper::GetStoryFramework()->DoTrapTriggerScript(m_uiActiveUin, iTriggerObjID, iTrapObjID, pstTrapObj->GetTrapConfigID(), iTriggerType);

    return T_SERVER_SUCESS;
}

//���ս���ϵĻ���
void CBattlefieldObj::DeleteCombatTrap(int iTrapObjID)
{
    //�ȴ�ս����Ϣ��ɾ��
    for(int i=0; i<m_iTrapNum; ++i)
    {
        if(m_aiCombatTrapIndex[i] == iTrapObjID)
        {
            m_aiCombatTrapIndex[i] = m_aiCombatTrapIndex[--m_iTrapNum];
            break;
        }
    }
    
    //ɾ������
    GameType<CCombatTrapObj>::Del(iTrapObjID);

    //���͸��ͻ��˵��Ƴ�֪ͨ
    static GameProtocolMsg stGameMsg;
    CZoneMsgHelper::GenerateMsgHead(stGameMsg, MSGID_ZONE_DELETECOMBATTRAP_NOTIFY);

    Zone_DeleteCombatTrap_Notify* pstNotify = stGameMsg.mutable_m_stmsgbody()->mutable_m_stzone_deletecombattrap_notify();
    pstNotify->set_itrapobjid(iTrapObjID);

    SendNotifyToBattlefield(stGameMsg);

    return;
}

//�����������͵Ļ���
int CBattlefieldObj::TriggerPitfallTrap(int iTriggerType)
{
    //��ȡս����λ
    CCombatUnitObj* pstActionUnitObj = CCombatUtility::GetCombatUnitObj(m_iActionUnit);
    if(!pstActionUnitObj)
    {
        LOGERROR("Failed to get action unit obj, invalid unit id %d, uin %u\n", m_iActionUnit, m_uiActiveUin);
        return T_ZONE_SYSTEM_PARA_ERR;
    }

    //��ȡ����
    CCombatTrapObj* pstTrapObj = GetTrapByPos(pstActionUnitObj->GetUnitPosition());
    if(!pstTrapObj || pstTrapObj->GetConfig()->iTriggerType != iTriggerType)
    {
        return T_SERVER_SUCESS;
    }

    return TriggerCombatTrap(m_iActionUnit, GetTrapIndexByPos(pstActionUnitObj->GetUnitPosition()), iTriggerType);
}

//�ֶ���������
int CBattlefieldObj::ManualTriggerTrap(const TUNITPOSITION& stPos)
{
    //��ȡ�ж���λ����
    CCombatUnitObj* pstActionUnitObj = CCombatUtility::GetCombatUnitObj(m_iActionUnit);
    if(!pstActionUnitObj)
    {
        LOGERROR("Failed to get action unit obj, unit id %d, uin %u\n", m_iActionUnit, m_uiActiveUin);
        return T_ZONE_SYSTEM_PARA_ERR;
    }

    //��ȡ���ض���
    CCombatTrapObj* pstTrapObj = GetTrapByPos(stPos);
    if(!pstTrapObj)
    {
        LOGERROR("Failed to get combat trap obj, pos %d:%d, uin %u\n", stPos.iPosX, stPos.iPosY, m_uiActiveUin);
        return T_ZONE_SYSTEM_PARA_ERR;
    }

    //�жϻ����Ƿ�ɼ����Ƿ�����
    if(!IsTrapVisible(*pstActionUnitObj, *pstTrapObj) || pstTrapObj->IsTrapDead())
    {
        LOGERROR("Failed to trigger combat trap, unit id %d, trap pos %d:%d, uin %u\n", m_iActionUnit, stPos.iPosX, stPos.iPosY, m_uiActiveUin);
        return T_ZONE_SYSTEM_PARA_ERR;
    }

    //�жϴ��������Ƿ�Ϸ�
    int iDistance = CCombatUtility::GetAttackDistance(pstActionUnitObj->GetUnitPosition(), stPos, pstTrapObj->GetConfig()->iRangeID);
    if(iDistance == 0)
    {
        //�������벻�Ϸ�
        LOGERROR("Failed to trigger combat trap, invalid distance, uin %u\n", m_uiActiveUin);
        return T_ZONE_SYSTEM_PARA_ERR;
    }

    //��������
    int iRet = TriggerCombatTrap(m_iActionUnit, GetTrapIndexByPos(stPos), TRAP_TRIGGER_PASSIVE);
    if(iRet)
    {
        LOGERROR("Failed to trigger combat trap, type %d, uin %u\n", TRAP_TRIGGER_PASSIVE, m_uiActiveUin);
        return iRet;
    }

    return T_SERVER_SUCESS;
}

//����ս������
int CBattlefieldObj::AttackCombatTrap(const TUNITPOSITION& stPos)
{
    //���͵Ĺ�����Ϣ
    static GameProtocolMsg stGameMsg;
    CZoneMsgHelper::GenerateMsgHead(stGameMsg, MSGID_ZONE_COMBATACTION_NOTIFY);

    Zone_CombatAction_Notify* pstNotify = stGameMsg.mutable_m_stmsgbody()->mutable_m_stzone_combataction_notify();
    pstNotify->set_iactionunitid(m_iActionUnit);
    pstNotify->mutable_sttargetpos()->set_iposx(stPos.iPosX);
    pstNotify->mutable_sttargetpos()->set_iposy(stPos.iPosY);
    pstNotify->set_etype(COMBAT_ACTION_CASTSKILL);
    pstNotify->set_iuseskilltype(SKILL_USE_ACTIVE);

    //��ȡ�ж���λ����
    CCombatUnitObj* pstActionUnitObj = CCombatUtility::GetCombatUnitObj(m_iActionUnit);
    if(!pstActionUnitObj)
    {
        LOGERROR("Failed to get action unit obj, unit id %d, uin %u\n", m_iActionUnit, m_uiActiveUin);
        return T_ZONE_SYSTEM_PARA_ERR;
    }

    //��ȡ���ض���
    CCombatTrapObj* pstTrapObj = GetTrapByPos(stPos);
    if(!pstTrapObj)
    {
        LOGERROR("Failed to get combat trap obj, pos %d:%d, uin %u\n", stPos.iPosX, stPos.iPosY, m_uiActiveUin);
        return T_ZONE_SYSTEM_PARA_ERR;
    }

    //��ȡ�ж���λ���չ�����
    const SFightUnitSkillConfig* pstSkillConfig = FightSkillCfgMgr().GetConfig(pstActionUnitObj->GetNormalSkillID());
    if(!pstSkillConfig)
    {
        LOGERROR("Failed to get fight skill config, skill id %d\n", pstActionUnitObj->GetNormalSkillID());
        return T_ZONE_SYSTEM_INVALID_CFG;
    }

    pstNotify->set_icastskillid(pstActionUnitObj->GetNormalSkillID());

    //�жϻ����Ƿ�ɼ�
    if(!IsTrapVisible(*pstActionUnitObj, *pstTrapObj) || pstTrapObj->IsTrapDead() || !pstTrapObj->IsTrapCanAttacked())
    {
        LOGERROR("Failed to trigger combat trap, unit id %d, trap pos %d:%d, uin %u\n", m_iActionUnit, stPos.iPosX, stPos.iPosY, m_uiActiveUin);
        return T_ZONE_SYSTEM_PARA_ERR;
    }

    //�жϴ��������Ƿ�Ϸ�
    int iDistance = CCombatUtility::GetAttackDistance(pstActionUnitObj->GetUnitPosition(), stPos, pstSkillConfig->iTargetRangeID);
    if(iDistance == 0)
    {
        //�������벻�Ϸ�
        LOGERROR("Failed to attack combat trap, invalid distance, uin %u\n", m_uiActiveUin);
        return T_ZONE_SYSTEM_PARA_ERR;
    }

    //�۳�����Ѫ��
    pstTrapObj->AddTrapHP(-1);
    
    pstNotify->set_inewtraphp(pstTrapObj->GetTrapHP());

    if(pstTrapObj->IsTrapDead())
    {
        //�Ѿ���������������
        int iRet = TriggerCombatTrap(m_iActionUnit, GetTrapIndexByPos(stPos), TRAP_TRIGGER_DESTROY);
        if(iRet)
        {
            LOGERROR("Failed to trigger combat trap, type %d, uin %u\n", TRAP_TRIGGER_DESTROY, m_uiActiveUin);
            return iRet;
        }
    }

    //���ͻ��ر���������Ϣ
    SendNotifyToBattlefield(stGameMsg);

    return T_SERVER_SUCESS;
}

//�жϻ����Ƿ�ɼ�
bool CBattlefieldObj::IsTrapVisible(CCombatUnitObj& stUnitObj, CCombatTrapObj& stTrapObj)
{
    //��ȡ��λ����Ӫ
    int iActionCamp = GetCombatUnitCamp(stUnitObj.GetCombatUnitID());

    //��ȡ���ص���Ӫ
    int iTrapCamp = stTrapObj.GetTrapCamp();
    
    switch(stTrapObj.GetConfig()->iVisiableType)
    {
        case TRAP_VISIBALE_ALL:
            {
                //�����˿ɼ�
                return true;
            }
            break;

        case TRAP_VISIBALE_SELF:
            {
                //������Ӫ�ɼ�
                if(iActionCamp == iTrapCamp)
                {
                    return true;
                }
            }
            break;

        default:
            {
                ;
            }
            break;
    }

    return false;
}

bool CBattlefieldObj::IsTrapVisibleToActionUnit(CCombatTrapObj& stTrapObj)
{
    CCombatUnitObj* pstActionUnitObj = CCombatUtility::GetCombatUnitObj(m_iActionUnit);
    if(!pstActionUnitObj)
    {
        return false;
    }

    return IsTrapVisible(*pstActionUnitObj, stTrapObj);
}

//��ȡս��������ϸ��Ϣ
void CBattlefieldObj::GetCombatTrapInfo(std::vector<TCOMBATTRAPINFO>& vTrapInfo)
{
    for(int i=0; i<m_iTrapNum; ++i)
    {
        CCombatTrapObj* pstTrapObj = GameType<CCombatTrapObj>::Get(m_aiCombatTrapIndex[i]);
        if(!pstTrapObj)
        {
            continue;
        }

        TCOMBATTRAPINFO stTrapInfo;
        stTrapInfo.iConfigID = pstTrapObj->GetTrapConfigID();
        stTrapInfo.iTrapObjID = m_aiCombatTrapIndex[i];

        vTrapInfo.push_back(stTrapInfo);
    }

    return;
}

void CBattlefieldObj::AddCombatUnitAttr(CCombatUnitObj& stUnitObj, int iAttrType, int iAddValue)
{
    if(iAttrType<0 || iAttrType>=FIGHT_ATTR_MAX)
    {
        return;
    }

    int iRealAddValue = 0;
    stUnitObj.AddFightAttr(iAttrType, iAddValue, &iRealAddValue);

    if(iRealAddValue != 0)
    {
        //������Ϣ֪ͨ
        static GameProtocolMsg stMsg;
        CZoneMsgHelper::GenerateMsgHead(stMsg, MSGID_ZONE_UNITATTRCHANGE_NOTIFY);
    
        Zone_UnitAttrChange_Notify* pstNotify = stMsg.mutable_m_stmsgbody()->mutable_m_stzone_unitattrchange_notify();
        pstNotify->set_iunitid(stUnitObj.GetCombatUnitID());
        pstNotify->set_iattrtype(iAttrType);
        pstNotify->set_iattrchange(iRealAddValue);
    
        SendNotifyToBattlefield(stMsg);
    }

    return;
}

//ɾ��ս����λ���ϵ�BUFF
void CBattlefieldObj::DelUnitBuff(int iUnitID, int iBuffID)
{
    CCombatUnitObj* pstUnitObj = CCombatUtility::GetCombatUnitObj(iUnitID);
    if(!pstUnitObj || !pstUnitObj->HasBuffOfSameID (iBuffID))
    {
        //ս����λ�����ڻ�������û��BUFF
        return;
    }

    static GameProtocolMsg stMsg;
    CZoneMsgHelper::GenerateMsgHead(stMsg, MSGID_ZONE_REMOVEBUFF_NOTIFY);

    Zone_RemoveBuff_Notify* pstNotify = stMsg.mutable_m_stmsgbody()->mutable_m_stzone_removebuff_notify();
    pstNotify->set_itargetunitid(iUnitID);

    pstUnitObj->DelUnitBuff(iBuffID, *pstNotify->add_steffects());

    SendNotifyToBattlefield(stMsg);

    return;
}

//������Ϣ��ս�����������
void CBattlefieldObj::SendNotifyToBattlefield(GameProtocolMsg& stNotify)
{
    CZoneMsgHelper::SendZoneMsgToRole(stNotify, CUnitUtility::GetRoleByUin(m_uiActiveUin));
    CZoneMsgHelper::SendZoneMsgToRole(stNotify, CUnitUtility::GetRoleByUin(m_uiPassiveUin));

    return;
}

//��ǰ�ж�����ҽ�ɫ�Ƿ���Ч
bool CBattlefieldObj::IsValidActionRole(unsigned int uin)
{
    if((GetCombatUnitCamp(m_iActionUnit)==FIGHT_CAMP_ACTIVE && uin==m_uiActiveUin)
       || (GetCombatUnitCamp(m_iActionUnit)==FIGHT_CAMP_PASSIVE && uin==m_uiPassiveUin))
    {
        return true;
    }

    return false;
}

//�Ƿ���Ҫ��������
bool CBattlefieldObj::IsNeedSetForm()
{
    return (m_bActiveNeedSetForm || m_bPassiveNeedSetForm);
}

//����ս������
int CBattlefieldObj::SetCombatForm(unsigned int uin, const CombatForm& stForm)
{
    //��鵱ǰս��״̬
    if(GetActionUnitStatus() != ROLE_COMBAT_UNIT_STAT_SETFORM)
    {
        LOGERROR("Failed to set combat form, invalid battle status, uin %u\n", uin);
        return T_ZONE_SYSTEM_PARA_ERR;
    }

    //���unitid�Ƿ�Ϸ�
    int* paiCombatObjIndex = (uin==m_uiActiveUin) ? m_aiActiveCombatObjIndex : m_aiPassiveCombatObjIndex;

    int iTotalUnitNum = 0;
    for(int i=0; i<MAX_CAMP_FIGHT_UNIT_NUM; ++i)
    {
        if(paiCombatObjIndex[i] < 0)
        {
            break;
        }

        ++iTotalUnitNum;
    }

    if(stForm.stunitinfos_size() != iTotalUnitNum)
    {
        LOGERROR("Failed to set combat unit form, invalid set form number %d, uin %u\n", stForm.stunitinfos_size(), uin);
        return T_ZONE_SYSTEM_PARA_ERR;;
    }

    for(int i=0; i<MAX_CAMP_FIGHT_UNIT_NUM; ++i)
    {
        if(paiCombatObjIndex[i] < 0)
        {
            break;
        }

        if(paiCombatObjIndex[i] != stForm.stunitinfos(i).iunitid())
        {
            LOGERROR("Failed to set combat form, invalid unit id %d:%d, uin %u\n", paiCombatObjIndex[i], stForm.stunitinfos(i).iunitid(), uin);
            return T_ZONE_SYSTEM_PARA_ERR;
        }
    }
    
    //��������ս����λ��λ��
    for(int i=0; i<MAX_CAMP_FIGHT_UNIT_NUM; ++i)
    {
        if(paiCombatObjIndex[i] < 0)
        {
            break;
        }

        CCombatUnitObj* pstCombatUnitObj = CCombatUtility::GetCombatUnitObj(paiCombatObjIndex[i]);
        if(!pstCombatUnitObj)
        {
            return T_ZONE_SYSTEM_PARA_ERR;
        }

        pstCombatUnitObj->SetUnitPosition(TUNITPOSITION(stForm.stunitinfos(i).stpos().iposx(), stForm.stunitinfos(i).stpos().iposy()));
    }

    //���ñ��
    if(uin == m_uiActiveUin)
    {
        m_bActiveNeedSetForm = false;
    }
    else
    {
        m_bPassiveNeedSetForm = false;
    }

    if(!IsNeedSetForm())
    {
        //�����Ѿ�������ɣ�����ս��״̬
        SetActionUnitStatus(ROLE_COMBAT_UNIT_STAT_INVALID);
    }

    return T_SERVER_SUCESS;
}

//����ս�����͵�֪ͨ
void CBattlefieldObj::SendCombatFormNotify()
{
    static GameProtocolMsg stMsg;
    CZoneMsgHelper::GenerateMsgHead(stMsg, MSGID_ZONE_SETCOMBATFORM_NOTIFY);

    Zone_SetCombatForm_Notify* pstNotify = stMsg.mutable_m_stmsgbody()->mutable_m_stzone_setcombatform_notify();

    for(int i=0; i<MAX_CAMP_FIGHT_UNIT_NUM; ++i)
    {
        if(m_aiActiveCombatObjIndex[i] >= 0)
        {
            CCombatUnitObj* pstUnitObj = CCombatUtility::GetCombatUnitObj(m_aiActiveCombatObjIndex[i]);
            if(pstUnitObj && !pstUnitObj->IsCombatUnitDead())
            {
                OneCombatUnitInfo* pstFormInfo = pstNotify->mutable_stactiveform()->add_stunitinfos();
                pstFormInfo->set_iunitid(m_aiActiveCombatObjIndex[i]);
                pstFormInfo->mutable_stpos()->set_iposx(pstUnitObj->GetUnitPosition().iPosX);
                pstFormInfo->mutable_stpos()->set_iposy(pstUnitObj->GetUnitPosition().iPosY);
            }
        }

        if(m_aiPassiveCombatObjIndex[i] >= 0)
        {
            CCombatUnitObj* pstUnitObj = CCombatUtility::GetCombatUnitObj(m_aiPassiveCombatObjIndex[i]);
            if(pstUnitObj && !pstUnitObj->IsCombatUnitDead())
            {
                OneCombatUnitInfo* pstFormInfo = pstNotify->mutable_stpassiveform()->add_stunitinfos();
                pstFormInfo->set_iunitid(m_aiPassiveCombatObjIndex[i]);
                pstFormInfo->mutable_stpos()->set_iposx(pstUnitObj->GetUnitPosition().iPosX);
                pstFormInfo->mutable_stpos()->set_iposy(pstUnitObj->GetUnitPosition().iPosY);
            }
        }
    }

    SendNotifyToBattlefield(stMsg);

    return;
}

//���͵�ǰս���ĳ�����Ϣ
void CBattlefieldObj::SendActionUnitNotify()
{
    static GameProtocolMsg stMsg;
    CZoneMsgHelper::GenerateMsgHead(stMsg, MSGID_ZONE_ACTIONUNITINFO_NOTIFY);

    Zone_ActionUnitInfo_Notify* pstNotify = stMsg.mutable_m_stmsgbody()->mutable_m_stzone_actionunitinfo_notify();

    for(int i=0; i<m_iCanCombatUnitNum; ++i)
    {
        if(m_aiCanCombatUnit[i] < 0)
        {
            continue;
        }

        CCombatUnitObj* pstUnitObj = CCombatUtility::GetCombatUnitObj(m_aiCanCombatUnit[i]);
        if(!pstUnitObj || pstUnitObj->IsCombatUnitDead())
        {
            continue;
        }

        OneActionInfo* pstOneInfo = pstNotify->add_stinactioninfos();
        pstOneInfo->set_iunitid(m_aiCanCombatUnit[i]);
    }

    for(int i=0; i<m_iActionDoneUnitNum; ++i)
    {
        if(m_aiActionDoneUnit[i] < 0)
        {
            continue;
        }

        CCombatUnitObj* pstUnitObj = CCombatUtility::GetCombatUnitObj(m_aiActionDoneUnit[i]);
        if(!pstUnitObj || pstUnitObj->IsCombatUnitDead())
        {
            continue;
        }

        OneActionInfo* pstOneInfo = pstNotify->add_stactiondoneinfos();
        pstOneInfo->set_iunitid(m_aiActionDoneUnit[i]);
    }

    SendNotifyToBattlefield(stMsg);

    return;
}

//����ս��ս����λ��ϸ��Ϣ
void CBattlefieldObj::SaveCombatUnitInfo()
{
    CGameRoleObj* pstRoleObj = CUnitUtility::GetRoleByUin(m_uiActiveUin);
    if(!pstRoleObj)
    {
        return;
    }

    //�����������������г��ű�ɱ���ĵ�λ��HP��MPΪ1
    std::vector<int> vExcludeConfigID;
    for(int i=0; i<m_iActiveScriptKilledNum; ++i)
    {
        vExcludeConfigID.push_back(m_aiActiveScriptKilledConfigID[i]);
    }

    //������������λ��HP��MP
    for(int i=0; i<MAX_CAMP_FIGHT_UNIT_NUM; ++i)
    {
        if(m_aiActiveCombatObjIndex[i] < 0)
        {
            continue;
        }

        CCombatUnitObj* pstCombatUnitObj = CCombatUtility::GetCombatUnitObj(m_aiActiveCombatObjIndex[i]);
        if(!pstCombatUnitObj)
        {
            continue;
        }

        CFightUnitUtility::SaveCombatUnitAttr(*pstRoleObj, pstCombatUnitObj->GetConfigID(), FIGHT_ATTR_HP, pstCombatUnitObj->GetFightAttr(FIGHT_ATTR_HP));
    }

    return;
}

//����ս���������ĵ�λ
void CBattlefieldObj::ClearDeadCombatUnit()
{
   CCombatUnitObj* pstUnitObj = NULL;

    //��������������
    for(int i=0; i<MAX_CAMP_FIGHT_UNIT_NUM;)
    {
        if(m_aiActiveCombatObjIndex[i] < 0)
        {
            break;
        }
        
        pstUnitObj = CCombatUtility::GetCombatUnitObj(m_aiActiveCombatObjIndex[i]);
        if(!pstUnitObj || !pstUnitObj->IsCombatUnitDead())
        {
            ++i;
            continue;
        }

        //��λ�Ѿ�����������
        ClearOneCombatUnitObj(m_aiActiveCombatObjIndex[i]);

        //�ӿ��ж�������ɾ��
        for(int j=0; j<m_iCanCombatUnitNum; ++j)
        {
            if(m_aiCanCombatUnit[j] == m_aiActiveCombatObjIndex[i])
            {
                m_aiCanCombatUnit[j] = m_aiCanCombatUnit[m_iCanCombatUnitNum-1];
                --m_iCanCombatUnitNum;

                break;
            }
        }

        //��������ǰ�ƣ���֤˳��
        for(int j=i; j<(MAX_CAMP_FIGHT_UNIT_NUM-1); ++j)
        {
            m_aiActiveCombatObjIndex[j] = m_aiActiveCombatObjIndex[j+1];
        }

        m_aiActiveCombatObjIndex[MAX_CAMP_FIGHT_UNIT_NUM-1] = -1;
    }

    //������������
    for(int i=0; i<MAX_CAMP_FIGHT_UNIT_NUM;)
    {
        if(m_aiPassiveCombatObjIndex[i] < 0)
        {
            break;
        }
        
        pstUnitObj = CCombatUtility::GetCombatUnitObj(m_aiPassiveCombatObjIndex[i]);
        if(!pstUnitObj || !pstUnitObj->IsCombatUnitDead())
        {
            ++i;
            continue;
        }

        //��λ�Ѿ�����������
        ClearOneCombatUnitObj(m_aiPassiveCombatObjIndex[i]);

        //�ӿ��ж�������ɾ��
        for(int j=0; j<m_iCanCombatUnitNum; ++j)
        {
            if(m_aiCanCombatUnit[j] == m_aiPassiveCombatObjIndex[i])
            {
                m_aiCanCombatUnit[j] = m_aiCanCombatUnit[m_iCanCombatUnitNum-1];
                --m_iCanCombatUnitNum;

                break;
            }
        }

        //��������ǰ�ƣ���֤˳��
        for(int j=i; j<(MAX_CAMP_FIGHT_UNIT_NUM-1); ++j)
        {
            m_aiPassiveCombatObjIndex[j] = m_aiPassiveCombatObjIndex[j+1];
        }

        m_aiPassiveCombatObjIndex[MAX_CAMP_FIGHT_UNIT_NUM-1] = -1;
    }

    return;
}

//�����ж���λ����CD�غ�
void CBattlefieldObj::DecreaseActionSkillCDRound()
{
    CCombatUnitObj* pstUnitObj = CCombatUtility::GetCombatUnitObj(m_iActionUnit);
    if(!pstUnitObj)
    {
        LOGERROR("Failed to decrease action skill cd round, uin %u\n", m_uiActiveUin);
        return;
    }

    static GameProtocolMsg stMsg;
    CZoneMsgHelper::GenerateMsgHead(stMsg, MSGID_ZONE_SKILLCDROUND_NOTIFY);

    Zone_SkillCDRound_Notify* pstNotify = stMsg.mutable_m_stmsgbody()->mutable_m_stzone_skillcdround_notify();

    pstUnitObj->DecreaseSkillCDRound(*pstNotify);
    SendNotifyToBattlefield(stMsg);

    return;
}

//��ʼ��������Ϣ
int CBattlefieldObj::InitBattlefieldInfo(int iCrossID)
{
    m_iCrossID = iCrossID;

    //��ȡս���ؿ�����
    const SBattleCrossConfig* pstCrossConfig = BattleCrossCfgMgr().GetConfig(m_iCrossID);
    if(!pstCrossConfig)
    {
        LOGERROR("Failed to get fight cross config, invalid cross id %d\n", m_iCrossID);
        return T_ZONE_SYSTEM_INVALID_CFG;
    }

    //�ж��Ƿ���Ҫ��������
    m_bActiveNeedSetForm = false;
    m_bPassiveNeedSetForm = false;
    if(pstCrossConfig->iNeedSetForm)
    {
        //��Ҫ��������
        m_bActiveNeedSetForm = true;

        if(m_iCrossID == PVP_COMBAT_CROSS_ID)
        {
            //�����PVPս��
            m_bPassiveNeedSetForm = true;
        }
    }

    //���õ�ͼID
    m_iMapID = pstCrossConfig->iMapID;

    //���õ�ͼ�赲����
    m_pstBattlePathManager = BattlefieldPathCfgMgr().GetBattlefieldPathConfigManager(m_iMapID);
    if(!m_pstBattlePathManager)
    {
        LOGERROR("Failed to get battlefiled path config, map id %d\n", m_iMapID);
        return T_ZONE_SYSTEM_INVALID_CFG;
    }

    //��ȡ˫���Ϸ���վλλ��
    GetBattlefieldValidPos(*pstCrossConfig);

    return T_SERVER_SUCESS;
}

//��ʼ��������Ϣ
int CBattlefieldObj::InitMonsterInfo(int iCrossID)
{
    //��ȡս���ؿ�����
    const SBattleCrossConfig* pstCrossConfig = BattleCrossCfgMgr().GetConfig(iCrossID);
    if(!pstCrossConfig)
    {
        LOGERROR("Failed to get pve fight cross config, invalid cross id %d\n", iCrossID);
        return T_ZONE_SYSTEM_INVALID_CFG;
    }

    //��ȡ�ؿ�����������
    const SCrossMonsterConfig* pstMonsterGroupConfig = CrossMonterCfgMgr().GetConfig(pstCrossConfig->iMonsterGroup);
    if(!pstMonsterGroupConfig)
    {
        LOGERROR("Failed to get monster group config, invalid id %d\n", pstCrossConfig->iMonsterGroup);
        return T_ZONE_SYSTEM_PARA_ERR;
    }

    int iRet = T_SERVER_SUCESS;

    //��ʼ������ս����Ϣ
    for(int i=0; i<pstMonsterGroupConfig->iMonsterNum; ++i)
    {
        const OneCrossMonster& stOneConfig = pstMonsterGroupConfig->astMonsterInfo[i];
        if(stOneConfig.iMonsterID==0 || m_astPassivePos[i].iPosX<0)
        {
            //��Ч����������������ģ�����IDΪ0��Ϊ������
            break;
        }

        iRet = InitOneMonsterInfo(i, stOneConfig, m_astPassivePos[i]);
        if(iRet)
        {
            LOGERROR("Failed to init one monster info, monster id %d, ret %d, active uin %u\n", stOneConfig.iMonsterID, iRet, m_uiActiveUin);
            return iRet;
        }
    }

    return T_SERVER_SUCESS;
}

//��ʼ����ֻ������Ϣ
int CBattlefieldObj::InitOneMonsterInfo(int iUnitIndex, const OneCrossMonster& stOneConfig, const SFightPosInfo& stPosInfo)    
{
    //����CCombatUnitObj
    CCombatUnitObj* pstCombatUnitObj = CreateCombatUnitObj(m_aiPassiveCombatObjIndex[iUnitIndex]);
    if(!pstCombatUnitObj)
    {
        LOGERROR("Failed to create combat unit obj!\n");
        return T_ZONE_SYSTEM_PARA_ERR;
    }

    int iRet = pstCombatUnitObj->InitMonsterUnitObj(m_aiPassiveCombatObjIndex[iUnitIndex], stOneConfig, stPosInfo, COMBAT_UNIT_TYPE_MONSTER);
    if(iRet)
    {
        LOGERROR("Failed to init monster unit obj, monster id %d, ret %d, uin %u\n", stOneConfig.iMonsterID, iRet, m_uiActiveUin);
        return iRet;
    }

    //�ͷ�ս����λ�ĵǳ�����
    std::vector<int> vSkillIDs;
    pstCombatUnitObj->GetSkillByType(SKILL_USE_ON_STAGE, vSkillIDs);

    for(unsigned i=0; i<vSkillIDs.size(); ++i)
    {
        iRet = CCombatSkill::CastSkill(m_iBattlefieldObjID, m_aiPassiveCombatObjIndex[iUnitIndex], pstCombatUnitObj->GetUnitPosition(), vSkillIDs[i], SKILL_USE_ON_STAGE);
        if(iRet)
        {
            LOGERROR("Failed to cast combat unit skill, unit %d, skill %d, uin %u\n", m_aiPassiveCombatObjIndex[iUnitIndex], vSkillIDs[i], m_uiActiveUin);
            return iRet;
        }
    }

    LOGDEBUG("Success to init one monster info, monster id %d, uin %u\n", stOneConfig.iMonsterID, m_uiActiveUin);

    return T_SERVER_SUCESS;
}

//��ʼ��������Ӫ��Ϣ
int CBattlefieldObj::InitFightCampInfo(CGameRoleObj& rstRoleObj, bool bIsActive)   
{
    //��ȡ��ɫս����λ������
    CFightUnitManager& rstFightUnitManager = rstRoleObj.GetFightUnitManager();

    //��ȡ������ս����λ����Ϣ
    std::vector<int> vFormUnitInfo;
    rstFightUnitManager.GetFormFightUnitInfo(vFormUnitInfo);

    int iOnFightNum = (vFormUnitInfo.size()>MAX_FIGHTUNIT_ON_FORM) ? MAX_FIGHTUNIT_ON_FORM : vFormUnitInfo.size();

    const SFightPosInfo* pstCampPosConfig = NULL;
    if(bIsActive)
    {
        pstCampPosConfig = m_astActivePos;
    }
    else
    {
        pstCampPosConfig = m_astPassivePos;
    }

    int iRet = T_SERVER_SUCESS;
    for(int i=0; i<iOnFightNum; ++i)
    {
        if(pstCampPosConfig->iPosX < 0)
        {
            break;
        }

        iRet = InitOneFightUnitInfo(i, rstRoleObj, vFormUnitInfo[i], pstCampPosConfig[i], bIsActive);
        if(iRet)
        {
            LOGERROR("Failed to init one active fight unit info, index %d, ret %d, uin %u\n", vFormUnitInfo[i], iRet, rstRoleObj.GetUin());
            return iRet;
        }
    }

    return T_SERVER_SUCESS;
}

//��ʼ������ս����λ��Ϣ
int CBattlefieldObj::InitOneFightUnitInfo(int iUnitIndex, CGameRoleObj& rstRoleObj, int iFightUnitID, const SFightPosInfo& stPosInfo, bool bIsActive)
{
    int* paiCombatObjIndex = NULL;
    if(bIsActive)
    {
        paiCombatObjIndex = m_aiActiveCombatObjIndex;
    }
    else
    {
        paiCombatObjIndex = m_aiPassiveCombatObjIndex;
    }

    //����ս��ս����λ����
    CCombatUnitObj* pstCombatUnitObj = CreateCombatUnitObj(paiCombatObjIndex[iUnitIndex]);
    if(!pstCombatUnitObj)
    {
        LOGERROR("Failed to create fight unit obj!\n");
        return T_ZONE_SYSTEM_PARA_ERR;
    }

    //��ʼ��ս��ս����λ����
    int iRet = pstCombatUnitObj->InitRoleUnitObj(paiCombatObjIndex[iUnitIndex], rstRoleObj, iFightUnitID, stPosInfo);
    if(iRet)
    {
        LOGERROR("Failed to init combat unit obj, unit id %d, ret %d, uin %u\n", iFightUnitID, iRet, rstRoleObj.GetUin());
        return iRet;
    }

    //�ͷ�ս����λ�ĵǳ�����
    std::vector<int> vSkillIDs;
    pstCombatUnitObj->GetSkillByType(SKILL_USE_ON_STAGE, vSkillIDs);

    for(unsigned i=0; i<vSkillIDs.size(); ++i)
    {
        iRet = CCombatSkill::CastSkill(m_iBattlefieldObjID, paiCombatObjIndex[iUnitIndex], pstCombatUnitObj->GetUnitPosition(), vSkillIDs[i], SKILL_USE_ON_STAGE);
        if(iRet)
        {
            LOGERROR("Failed to cast combat unit skill, unit %d, skill %d, uin %u\n", paiCombatObjIndex[iUnitIndex], vSkillIDs[i], rstRoleObj.GetUin());
            return iRet;
        }
    }

    return T_SERVER_SUCESS;
}

//������ص���ս��������Ϣ
int CBattlefieldObj::PackOneCombatUnitInfo(int iCamp, int iUnitObjIndex, CombatUnit& stUnitInfo)
{
    if(iUnitObjIndex < 0)
    {
        return T_SERVER_SUCESS;
    }

    CCombatUnitObj* pstCombatUnitObj = GameType<CCombatUnitObj>::Get(iUnitObjIndex);
    if(!pstCombatUnitObj)
    {
        return T_ZONE_SYSTEM_PARA_ERR;
    }

    pstCombatUnitObj->PackCombatUnitInfo(iCamp, stUnitInfo);

    return T_SERVER_SUCESS;
}

//��ʼ���غϳ��ֵ�λ��Ϣ
void CBattlefieldObj::InitRoundActionUnits()
{
    m_iCanCombatUnitNum = 0;
    m_iActionDoneUnitNum = 0;

    for(int i=0; i<MAX_CAMP_FIGHT_UNIT_NUM; ++i)
    {
        if(m_aiActiveCombatObjIndex[i] == -1)
        {
            break;
        }

        AddOneUnitToRoundActionList(m_aiActiveCombatObjIndex[i]);
    }

    for(int i=0; i<MAX_CAMP_FIGHT_UNIT_NUM; ++i)
    {
        if(m_aiPassiveCombatObjIndex[i] == -1)
        {
            break;
        }

        AddOneUnitToRoundActionList(m_aiPassiveCombatObjIndex[i]);
    }

    return;
}

//���һ��ս����λ���غϳ����б�
void CBattlefieldObj::AddOneUnitToRoundActionList(int iUnitObjIndex)
{
    CCombatUnitObj* pstCombatUnitObj = GameType<CCombatUnitObj>::Get(iUnitObjIndex);
    if(!pstCombatUnitObj || pstCombatUnitObj->IsCombatUnitDead())
    {
        //�Ҳ������ߵ�λ�Ѿ������������
        return;
    }

    m_aiCanCombatUnit[m_iCanCombatUnitNum++] = iUnitObjIndex;

    return;
}

//��ȡս����λ����Ӫ
int CBattlefieldObj::GetCombatUnitCamp(int iCombatUnitiD)
{
    //���ж��Ƿ����������ַ�
    for(int i=0; i<MAX_CAMP_FIGHT_UNIT_NUM; ++i)
    {
        if(m_aiActiveCombatObjIndex[i] == iCombatUnitiD)
        {
            return FIGHT_CAMP_ACTIVE;
        }

        if(m_aiPassiveCombatObjIndex[i] == iCombatUnitiD)
        {
            return FIGHT_CAMP_PASSIVE;
        }
    }

    return FIGHT_CAMP_INVALID;
}

//ս����λ�Ƿ�ͬһ��Ӫ
bool CBattlefieldObj::IsUnitInSameCamp(CCombatUnitObj& stUnitObj, CCombatUnitObj& stOtherUnitObj)
{
    return (GetCombatUnitCamp(stUnitObj.GetCombatUnitID())==GetCombatUnitCamp(stOtherUnitObj.GetCombatUnitID()));
}

//���û���ս��AI
int CBattlefieldObj::SetPartnerAI(int iPartnerAIID)
{
    //�жϵ�ǰս����λ�Ƿ��ǻ��
    if(GetActionUnitType() != COMBAT_UNIT_TYPE_PARTNER)
    {
        LOGERROR("Failed to set partner AI, action unit is not partner, uin %u\n", m_uiActiveUin);
        return T_ZONE_SYSTEM_PARA_ERR;
    }

    CCombatUnitObj* pstPartnerUnitObj = CCombatUtility::GetCombatUnitObj(m_iActionUnit);
    if(!pstPartnerUnitObj)
    {
        LOGERROR("Failed to get partner unit obj, uin %u\n", m_uiActiveUin);
        return T_ZONE_SYSTEM_PARA_ERR;
    }

    return pstPartnerUnitObj->SetFightAI(iPartnerAIID);
}

//�жϵ�ǰ�Ƿ���δ���ֵ�λ
bool CBattlefieldObj::HasActionCombatUnit()
{
    for(int i=0; i<m_iCanCombatUnitNum; ++i)
    {
        if(m_aiCanCombatUnit[i] >= 0)
        {
            CCombatUnitObj* pstUnitObj = CCombatUtility::GetCombatUnitObj(m_aiCanCombatUnit[i]);
            if(pstUnitObj && !pstUnitObj->IsCombatUnitDead())
            {
                return true;
            }
        }
    }

    return false;
}

//todo jasonxiong5 ���Ҫ���µ��ȹ����¿���
//���õ�ǰ���ֵ�ս����λ
void CBattlefieldObj::SetActionCombatUnit()
{
    if(m_iActionUnit >= 0)
    {
        LOGERROR("Failed to set action combat unit, current action unit %d, uin %u\n", m_iActionUnit, m_uiActiveUin);
        return;
    }

    //������ǰ�ж����飬�ҳ���ǰ���ֵ�ս����λ
    int iActionIndex = -1;
    for(int i=0; i<m_iCanCombatUnitNum; ++i)
    {
        if(m_aiCanCombatUnit[i] < 0)
        {
            //�Ѿ��ж���
            continue;
        }

        CCombatUnitObj* pstUnitObj = GameType<CCombatUnitObj>::Get(m_aiCanCombatUnit[i]);
        if(!pstUnitObj)
        {
            LOGERROR("Failed to get combat unit obj, uin %u\n", m_uiActiveUin);
            return;
        }

        if(pstUnitObj->IsCombatUnitDead())
        {
            m_aiCanCombatUnit[i] = -1;

            continue;
        }

        if(iActionIndex < 0)
        {
            iActionIndex = i;

            continue;
        }

        CCombatUnitObj* pstActionUnitObj = GameType<CCombatUnitObj>::Get(m_aiCanCombatUnit[iActionIndex]);
        if(!pstActionUnitObj)
        {
            LOGERROR("Failed to get combat unit obj, uin %u\n", m_uiActiveUin);
            return;
        }

        if(pstActionUnitObj->IsCombatUnitDead())
        {
            //��λ�����������ж�
            m_aiCanCombatUnit[i] = -1;

            continue;
        }
    }

    if(iActionIndex >= 0)
    {
        m_iActionUnit = m_aiCanCombatUnit[iActionIndex];

        m_aiCanCombatUnit[iActionIndex] = -1;
    }

    return;
}

//�����ǰ���ֵ�ս����λ
void CBattlefieldObj::ClearActionCombatUnit()
{
    //���õ�ǰ�ж���λ���ѳ�����
    m_aiActionDoneUnit[m_iActionDoneUnitNum++] = m_iActionUnit;
    m_iActionUnit = -1;

    return;
}

//��ȡ��ǰ���ֵ�ս����λ������
int CBattlefieldObj::GetActionUnitType()
{
    CCombatUnitObj* pstActionUnitObj = GameType<CCombatUnitObj>::Get(m_iActionUnit);
    if(!pstActionUnitObj)
    {
        LOGERROR("Failed to get action unit obj, index %d, uin %u\n", m_iActionUnit, m_uiActiveUin);
        return COMBAT_UNIT_TYPE_INVALID;
    }

    return pstActionUnitObj->GetCombatUnitType();
}

//��ȡ��ǰ���ֵ�λ��ID
int CBattlefieldObj::GetActionUnitID()
{
    return m_iActionUnit;
}

//����ս����λ��ǰ��״̬
int CBattlefieldObj::GetActionUnitStatus()
{
    return m_iRoleCombatUnitStatus;
}

void CBattlefieldObj::SetActionUnitStatus(int iStatus)
{
    m_iRoleCombatUnitStatus = iStatus;
}

//�ж��Ƿ��ܹ�����ս��
bool CBattlefieldObj::CanEndCombat()
{
    return (m_iCombatResult != COMBAT_RESULT_INVALID);
}

//���Ϳͻ������ǲ�����ָ��
void CBattlefieldObj::NotifyRoleUnitDirective(int iDirective)
{
    static GameProtocolMsg stUnitDirectiveNotify;

    CZoneMsgHelper::GenerateMsgHead(stUnitDirectiveNotify, MSGID_ZONE_UNITDIRECTIVE_NOTIFY);

    Zone_UnitDirective_Notify* pstNotify = stUnitDirectiveNotify.mutable_m_stmsgbody()->mutable_m_stzone_unitdirective_notify();
    pstNotify->set_idirective((UnitDirectiveType)iDirective);
    pstNotify->set_icombatunitid(m_iActionUnit);

    //����֪ͨ
    SendNotifyToBattlefield(stUnitDirectiveNotify);

    return;
}

//��ʼս����λ���ƶ�
int CBattlefieldObj::DoCombatUnitMove(const UnitPath& stPath)
{
    if(stPath.stposes_size() == 0)
    {
        //ս����λ���ƶ�
        return T_SERVER_SUCESS;
    }

    //��ȡ��ǰ�ж���ս����λObj
    CCombatUnitObj* pstActionUnitObj = GameType<CCombatUnitObj>::Get(m_iActionUnit);
    if(!pstActionUnitObj)
    {
        LOGERROR("Failed to get action unit obj, index %d, uin %u\n", m_iActionUnit, m_uiActiveUin);
        return T_ZONE_SYSTEM_PARA_ERR;
    }

    //�ж��Ƿ�����ƶ�
    if(pstActionUnitObj->GetCombatUnitStatus(COMBAT_UNIT_STATUS_NOTMOVE))
    {
        //�����ƶ�
        pstActionUnitObj->SetCombatUnitStatus(COMBAT_UNIT_STATUS_NOTMOVE, false);

        return T_SERVER_SUCESS;
    }

    //��ȡս����λ����ƶ�����
    int iMaxWalkPosNum = GetMaxMoveDistance(*pstActionUnitObj);

    //��鲽��
    if(stPath.stposes_size()>iMaxWalkPosNum || stPath.stposes_size()>MAX_POSITION_NUMBER_IN_PATH)
    {
        LOGERROR("Failed to do combat unit move, pos num reach max %d, unit index %d, uin %u\n", iMaxWalkPosNum, m_iActionUnit, m_uiActiveUin);
        return T_ZONE_SYSTEM_PARA_ERR;
    }

    //��װ�µ�·��
    TUNITPATH stUnitPath;
    for(int i=0; i<stPath.stposes_size(); ++i)
    {
        stUnitPath.astPos[i].iPosX = stPath.stposes(i).iposx();
        stUnitPath.astPos[i].iPosY = stPath.stposes(i).iposy();

        ++stUnitPath.iPosNum;
    }

    //���·���Ƿ����������
    if(!m_pstBattlePathManager->CanContinueWalk(pstActionUnitObj->GetUnitPosition(), stUnitPath))
    {
        LOGERROR("Failed to do combat unit move, path can not walk, uin %u\n", m_uiActiveUin);
        return T_ZONE_SYSTEM_PARA_ERR;
    }

    //���·���ϵĵ��Ƿ������
    for(int i=0; i<stUnitPath.iPosNum; ++i)
    {
        if(!IsPosCanWalk(stUnitPath.astPos[i], m_iActionUnit))
        {
            LOGERROR("Failed to do combat unit move, path can not walk, uin %u\n", m_uiActiveUin);
            return T_ZONE_SYSTEM_PARA_ERR;
        }
    }

    //����·���ϵĻ���
    int iTargetIndex = stUnitPath.iPosNum-1;
    for(int i=0; i<stUnitPath.iPosNum; ++i)
    {
        CCombatTrapObj* pstTrapObj = GetTrapByPos(stUnitPath.astPos[i]);
        if(!pstTrapObj)
        {
            //û�л���
            continue;
        }

        //���Դ���ֹͣ�������͵Ļ���
        if(pstTrapObj->GetConfig()->iTriggerType == TRAP_TRIGGER_MOVEON)
        {
            TriggerCombatTrap(m_iActionUnit, pstTrapObj->GetObjectID(), TRAP_TRIGGER_MOVEON);
        }

        //�ж��Ƿ����ж�
        if(pstTrapObj->GetConfig()->iInterruptType == TRAP_INTERRUPT_MOVE)
        {
            //����ƶ�
            iTargetIndex = i;

            break;
        }
    }

    return DoCombatUnitMove(iTargetIndex+1, stUnitPath.astPos);
}

//��ʼս����λ���ƶ�,AI�ƶ�ֻ��Ŀ���
int CBattlefieldObj::DoCombatUnitMove(int iMoveDistance, const TUNITPOSITION* pstMovePath)
{
    //��ȡ��ǰ�ж���ս����λObj
    CCombatUnitObj* pstActionUnitObj = GameType<CCombatUnitObj>::Get(m_iActionUnit);
    if(!pstActionUnitObj)
    {
        LOGERROR("Failed to get action unit obj, index %d, uin %u\n", m_iActionUnit, m_uiActiveUin);
        return T_ZONE_SYSTEM_PARA_ERR;
    }

    if(pstActionUnitObj->GetUnitPosition().iPosX == pstMovePath[iMoveDistance-1].iPosX
       && pstActionUnitObj->GetUnitPosition().iPosY == pstMovePath[iMoveDistance-1].iPosY)
    {
        //Ŀ��λ�þ��ǵ�ǰλ��
        return T_SERVER_SUCESS;
    }

    //�������µķ���
    CCombatUtility::UpdateUnitDirection(*pstActionUnitObj, pstMovePath[iMoveDistance-1]);

    //�������µ�λ��
    pstActionUnitObj->SetUnitPosition(pstMovePath[iMoveDistance-1]);

    //�����ƶ���֪ͨ
    static GameProtocolMsg stCombatUnitMoveNotify;

    CZoneMsgHelper::GenerateMsgHead(stCombatUnitMoveNotify, MSGID_ZONE_COMBATMOVE_NOTIFY);

    Zone_CombatMove_Notify* pstNotify = stCombatUnitMoveNotify.mutable_m_stmsgbody()->mutable_m_stzone_combatmove_notify();
    pstNotify->set_iunitid(m_iActionUnit);
    pstNotify->set_idirection(pstActionUnitObj->GetUnitDirection());
    pstNotify->set_imovetype(COMBAT_MOVE_NORMAL);

    //��װ·�����ظ��ͻ���
    for(int i=0; i<iMoveDistance; ++i)
    {
        UnitPosition* pstPos = pstNotify->mutable_stpath()->add_stposes();
        pstPos->set_iposx(pstMovePath[i].iPosX);
        pstPos->set_iposy(pstMovePath[i].iPosY);
    }

    SendNotifyToBattlefield(stCombatUnitMoveNotify);

    //��Ч�ƶ�������BUFF,�ƶ�N�񴥷�N��
    for(int i=0; i<iMoveDistance; ++i)
    {
        int iRet = DoBuffEffectByType(BUFF_TRIGGER_MOVE, m_iActionUnit, m_iActionUnit);
        if(iRet)
        {
            LOGERROR("Failed to trigger move buff, ret %d, unit id %d, uin %u\n", iRet, m_iActionUnit, m_uiActiveUin);
            return iRet;
        }
    }

    return T_SERVER_SUCESS;
}

//ս����λʹ�ü���
int CBattlefieldObj::DoCombatCastSkill(int iSkillID, const TUNITPOSITION& stTargetPos, int iSkillUseType)
{
    CCombatUnitObj* pstActionUnitObj = CCombatUtility::GetCombatUnitObj(m_iActionUnit);
    if(!pstActionUnitObj)
    {
        LOGERROR("Failed to get combat unit obj, unit id %d, uin %u\n", m_iActionUnit, m_uiActiveUin);
        return T_ZONE_SYSTEM_PARA_ERR;
    }

    //���Դ������ģʽ
    if(pstActionUnitObj->GetMotorDistance() >= 0)
    {
        if(pstActionUnitObj->GetMotorLeftLen() <= pstActionUnitObj->GetMotorDistance())
        {
            //������ȡ������ģʽ������ȡ������ģʽ
            pstActionUnitObj->SetMotorMode(TUNITPOSITION(-1,0), -1);
        }

        return T_SERVER_SUCESS;
    }

    if(pstActionUnitObj->GetCombatUnitStatus(COMBAT_UNIT_STATUS_NOTACTION))
    {
        //��������ǰ�����ж�
        pstActionUnitObj->SetCombatUnitStatus(COMBAT_UNIT_STATUS_NOTACTION, false);

        return T_SERVER_SUCESS;
    }

    CCombatUnitObj* pstTargetUnitObj = GetCombatUnitByPos(stTargetPos);
    int iRet = T_SERVER_SUCESS;
    if(!pstTargetUnitObj)
    {
        //Ŀ��λ�ò���ս����λ�Կ�λ�ż���
        iRet = CCombatSkill::CastSkill(m_iBattlefieldObjID, m_iActionUnit, stTargetPos, iSkillID, iSkillUseType);
        if(iRet)
        {
            LOGERROR("Failed to cast combat skill, ret %d, target pos %d:%d, skill %d, uin %u\n", iRet, stTargetPos.iPosX, stTargetPos.iPosY, iSkillID, m_uiActiveUin);
            return iRet;
        }
    }
    else
    {
        int iTargetUnitID = pstTargetUnitObj->GetCombatUnitID();

        //Ŀ��λ����ս����λ
        int iRet = CCombatSkill::CastSkill(m_iBattlefieldObjID, m_iActionUnit, pstTargetUnitObj->GetUnitPosition(), iSkillID, iSkillUseType);
        if(iRet)
        {
            LOGERROR("Failed to cast combat skill, ret %d, target %d, skill %d, uin %u\n", iRet, iTargetUnitID, iSkillID, m_uiActiveUin);
            return iRet;
        }
    
        //���Ŀ���Ѿ�����������������Ĵ���
        if(IsCombatUnitDead(iTargetUnitID))
        {
            return T_SERVER_SUCESS;
        }
    
        //���ܵ���ҪĿ���Ƿ��������
        int iChiefTargetDodge = CCombatSkill::m_iChiefTargetDodge;
    
        //��������λ���ܵ�����
        iRet = DoComboAttack(*pstTargetUnitObj, iSkillID);
    
        //���ܹ����������,����ͬһ����Ӫ�������չ�,������ѵļл�
        if(!IsUnitInSameCamp(*pstActionUnitObj,*pstTargetUnitObj) && pstActionUnitObj->GetNormalSkillID()==iSkillID)
        {
            iRet = DoPincerAttack(*pstTargetUnitObj);
            if(iRet)
            {
                LOGERROR("Failed to process pincer attack, unit id %d, target %d, uin %u\n", m_iActionUnit, iTargetUnitID, m_uiActiveUin);
                return iRet;
            }
        }
    
        //�����ҪĿ��������ܣ����Ҳ��Ƕ��Լ�,���Դ�������
        if(iChiefTargetDodge==SKILL_HIT_CHIEFDODGE && pstTargetUnitObj->GetCombatUnitID()!=m_iActionUnit)
        {
            iRet = DoCounterAttack(*pstTargetUnitObj, *pstActionUnitObj);
            if(iRet)
            {
                LOGERROR("Failed to do counterattack, unit id %d, uin %u\n", iTargetUnitID, m_uiActiveUin);
                return iRet;
            }
        }
    
        //�������߼��ܵĻ��ϻ�
        iRet = DoJointAttack(*pstTargetUnitObj, iSkillID);
        if(iRet)
        {
            LOGERROR("Failed to process joint attack, unit id %d, skill id %d, uin %u\n", m_iActionUnit, iSkillID, m_uiActiveUin);
            return iRet;
        }
    }

    return T_SERVER_SUCESS;
}

//ս����λʹ�õ���,����BUFF���߷��û���
int CBattlefieldObj::DoCombatUseItem(int iItemID, const TUNITPOSITION& stTargetPos)
{
    //todo jasonxiong5 ����ʹ�����Ҫ���¿����߼�
    /*
    //���Ϳ�ʼʹ�õ��ߵ���Ϣ
    static GameProtocolMsg stActionBeginMsg;
    CZoneMsgHelper::GenerateMsgHead(stActionBeginMsg, MSGID_ZONE_BEGINCOMBATACTION_NOTIFY);

    SendNotifyToBattlefield(stActionBeginMsg);

    //��ȡ��������
    const SPropItemConfig* pstPropConfig = PropItemCfgMgr().Get(iItemID);
    if(!pstPropConfig)
    {
        LOGERROR("Failed to get prop item config, invalid prop id %d\n", iItemID);
        return T_ZONE_SYSTEM_INVALID_CFG;
    }

    //��ȡʹ�õ���ս����λObj
    CCombatUnitObj* pstCastUnitObj = CCombatUtility::GetCombatUnitObj(m_iActionUnit);
    if(!pstCastUnitObj)
    {
        LOGERROR("Failed to get cast fight unit obj, invalid unit id %d, uin %u\n", m_iActionUnit, m_uiActiveUin);
        return T_ZONE_SYSTEM_PARA_ERR;
    }

    //�ж��Ƿ��ܹ�ʹ��
    int iDistance = CCombatUtility::GetAttackDistance(pstCastUnitObj->GetUnitPosition(), stTargetPos, pstPropConfig->iRangeID);
    if(iDistance == 0)
    {
        LOGERROR("Failed to use item on target, invalid pos, item id %d, uin %u\n", iItemID, m_uiActiveUin);
        return T_ZONE_SYSTEM_PARA_ERR;
    }

    //�۳���ҵ���
    CGameRoleObj* pstRoleObj = NULL;
    int iCastUnitCamp = GetCombatUnitCamp(m_iActionUnit);
    switch(iCastUnitCamp)
    {
        case FIGHT_CAMP_ACTIVE:
            {
                pstRoleObj = CUnitUtility::GetRoleByUin(m_uiActiveUin);
            }
            break;

        case FIGHT_CAMP_PASSIVE:
            {
                pstRoleObj = CUnitUtility::GetRoleByUin(m_uiPassiveUin);
            }
            break;

        default:
            {
                ;
            }
            break;
    }
    
    if(!pstRoleObj)
    {
        LOGERROR("Failed to get game role obj, active %u, passive %u\n", m_uiActiveUin, m_uiPassiveUin);
        return T_ZONE_SYSTEM_PARA_ERR;
    }

    int iRet = pstRoleObj->GetRepThingsManager().DeleteRepItemByID(iItemID, ITEM_DELETE_BATTLE);
    if(iRet)
    {
        LOGERROR("Failed to delete rep item, item id %d, uin %u\n", iItemID, pstRoleObj->GetUin());
        return iRet;
    }

    switch(pstPropConfig->iPropType)
    {
        case PROP_ITEM_YAOPIN:
            {
                //ҩƷ

                //�жϷ��õĵ�λ�Ƿ�������ļ���
                if(pstPropConfig->iTrapNeedSkill!=0 && !pstCastUnitObj->HasFightSkill(pstPropConfig->iTrapNeedSkill))
                {
                    //û������ļ���
                    LOGERROR("Failed to use prop item, no need skill, skill id %d, item id %d, uin %u\n", pstPropConfig->iTrapNeedSkill, iItemID, m_uiActiveUin);
                    return T_ZONE_SYSTEM_PARA_ERR;
                }

                static GameProtocolMsg stCombatAddBuffNotify;
                CZoneMsgHelper::GenerateMsgHead(stCombatAddBuffNotify, MSGID_ZONE_COMBATADDBUFF_NOTIFY);
            
                Zone_CombatAddBuff_Notify* pstNotify = stCombatAddBuffNotify.mutable_m_stmsgbody()->mutable_m_stzone_combataddbuff_notify();
            
                CCombatUnitObj* pstTargetUnitObj = GetCombatUnitByPos(stTargetPos);
                if(!pstTargetUnitObj)
                {
                    LOGERROR("Failed to use prop item, item id %d, target pos %d:%d, uin %u\n", iItemID, stTargetPos.iPosX, stTargetPos.iPosY, m_uiActiveUin);
                    return T_ZONE_SYSTEM_PARA_ERR;
                }

                //��Ч����Ч��
                iRet = pstTargetUnitObj->AddUnitBuff(m_uiActiveUin, m_iCrossID, pstPropConfig->iBuffID, m_iActionUnit, *pstNotify);
                if(iRet)
                {
                    LOGERROR("Failed to add combat unit buff, ret %d, item id %d, buff id %d, uin %u\n", iRet, iItemID, pstPropConfig->iBuffID, pstRoleObj->GetUin());
                    return iRet;
                }
            
                SendNotifyToBattlefield(stCombatAddBuffNotify);
            }
            break;

        case PROP_ITEM_JIGUAN:
            {
                //�жϷ��õĵ�λ�Ƿ�������ļ���
                if(pstPropConfig->iTrapNeedSkill!=0 && !pstCastUnitObj->HasFightSkill(pstPropConfig->iTrapNeedSkill))
                {
                    //û������ļ���
                    LOGERROR("Failed to add combat trap, no need skill, skill id %d, item id %d, uin %u\n", pstPropConfig->iTrapNeedSkill, iItemID, m_uiActiveUin);
                    return T_ZONE_SYSTEM_PARA_ERR;
                }

                //��ս���Ϸ��û���
                int iTrapObjID = AddCombatTrap(pstPropConfig->iAddTrapID, GetCombatUnitCamp(m_iActionUnit), stTargetPos, pstPropConfig->iTrapDirection);
                if(iTrapObjID < 0)
                {
                    LOGERROR("Failed to add combat trap, trap obj id %d, item id %d, uin %u\n", iTrapObjID, iItemID, m_uiActiveUin);
                    return T_ZONE_SYSTEM_PARA_ERR;
                }
            }
            break;

        default:
            {
                LOGERROR("Failed to use combat prop item, prop id %d, uin %u\n", iItemID, m_uiActiveUin);
                return T_ZONE_SYSTEM_INVALID_CFG;
            }
            break;
    }

    //����ActionNotify��Ϣ
    static GameProtocolMsg stActionNotifyMsg;
    CZoneMsgHelper::GenerateMsgHead(stActionNotifyMsg, MSGID_ZONE_COMBATACTION_NOTIFY);

    Zone_CombatAction_Notify* pstActionNotify = stActionNotifyMsg.mutable_m_stmsgbody()->mutable_m_stzone_combataction_notify();
    pstActionNotify->set_iactionunitid(m_iActionUnit);

    pstActionNotify->mutable_sttargetpos()->set_iposx(stTargetPos.iPosX);
    pstActionNotify->mutable_sttargetpos()->set_iposy(stTargetPos.iPosY);

    pstActionNotify->set_etype(COMBAT_ACTION_USEITEM);
    pstActionNotify->set_iitemid(iItemID);

    SendNotifyToBattlefield(stActionNotifyMsg);

    //����ʹ����Ʒ�ű��ӿ�
    CModuleHelper::GetStoryFramework()->DoUseItemScript(m_uiActiveUin, m_iCrossID, iItemID, m_iActionUnit, stTargetPos.iPosX, stTargetPos.iPosY);
    */


    return T_SERVER_SUCESS;
}

//�����ܵ�����
int CBattlefieldObj::DoComboAttack(CCombatUnitObj& stTargetUnitObj, int iSkillID)
{
    const SFightUnitSkillConfig* pstSkillConfig = FightSkillCfgMgr().GetConfig(iSkillID);
    if(!pstSkillConfig)
    {
        LOGERROR("Failed to get fight skill config, invalid id %d\n", iSkillID);
        return T_ZONE_SYSTEM_INVALID_CFG;
    }

    CCombatUnitObj* pstActionUnitObj = CCombatUtility::GetCombatUnitObj(m_iActionUnit);
    if(!pstActionUnitObj)
    {
        LOGERROR("Failed to get action unit obj, unit id %d, uin %u\n", m_iActionUnit, m_uiActiveUin);
        return T_ZONE_SYSTEM_PARA_ERR;
    }

    int iComboAttr = pstActionUnitObj->GetFightAttr(FIGHT_ATTR_LIANJI);
    int iRet = T_SERVER_SUCESS;
    for(int i=0; i<pstSkillConfig->iComboNum; ++i)
    {
        //�ж��Ƿ���������
        if(CRandomCalculator::GetRandomNumberInRange(100) >= iComboAttr)
        {
            //û�����������
            break;
        }

        //�����ܵ�����
        iRet = CCombatSkill::CastSkill(m_iBattlefieldObjID, m_iActionUnit, stTargetUnitObj.GetUnitPosition(), iSkillID, SKILL_USE_LIANJI);
        if(iRet)
        {
            LOGERROR("Failed to cast combat skill, unit id %d, target id %d, skill id %d, uin %u\n", m_iActionUnit, stTargetUnitObj.GetCombatUnitID(), iSkillID, m_uiActiveUin);
            return iRet;
        }
    }

    return T_SERVER_SUCESS;
}

//�����ܵļл�
int CBattlefieldObj::DoPincerAttack(CCombatUnitObj& stTargetUnitObj)
{
    CCombatUnitObj* pstActionUnitObj = CCombatUtility::GetCombatUnitObj(m_iActionUnit);
    if(!pstActionUnitObj)
    {
        LOGERROR("Failed to get combat unit obj, unit id %d, uin %u\n", m_iActionUnit, m_uiActiveUin);
        return T_ZONE_SYSTEM_PARA_ERR;
    }

    TUNITPOSITION& stActionPos = pstActionUnitObj->GetUnitPosition();
    TUNITPOSITION& stTargetPos = stTargetUnitObj.GetUnitPosition();

    //��ȡ����λ�õ�����
    TUNITPOSITION stMirrorPos;
    stMirrorPos.iPosX = stTargetPos.iPosX*2-stActionPos.iPosX;
    stMirrorPos.iPosY = stTargetPos.iPosY*2-stActionPos.iPosY;

    //��ȡ����λ�õĵ�λ
    CCombatUnitObj* pstMirrorUnitObj = GetCombatUnitByPos(stMirrorPos);
    if(!pstMirrorUnitObj || !IsUnitInSameCamp(*pstActionUnitObj,*pstMirrorUnitObj))
    {
        //����λ��û�е�λ����ͬ��Ӫ
        return T_SERVER_SUCESS;
    }

    //�����չ�����
    int iRet = CCombatSkill::CastSkill(m_iBattlefieldObjID, pstMirrorUnitObj->GetCombatUnitID(), stTargetUnitObj.GetUnitPosition(), pstMirrorUnitObj->GetNormalSkillID(), SKILL_USE_JIAJI);
    if(iRet)
    {
        LOGERROR("Failed to process pincer attack, unit id %d, skill %d, uin %u\n", pstMirrorUnitObj->GetCombatUnitID(), pstMirrorUnitObj->GetNormalSkillID(), m_uiActiveUin);
        return iRet;
    }

    return T_SERVER_SUCESS;
}

//�����ܵķ���
int CBattlefieldObj::DoCounterAttack(CCombatUnitObj& stCounterAtkUnitObj, CCombatUnitObj& stTargetObj)
{
    //�ж��Ƿ񴥷�����
    if(CRandomCalculator::GetRandomNumberInRange(100) >= stCounterAtkUnitObj.GetFightAttr(FIGHT_ATTR_FANJI))
    {
        //û�д�������
        return T_SERVER_SUCESS;
    }

    //������������
    int iSkillID = stCounterAtkUnitObj.GetCounterAtkSkill();
    int iRet = CCombatSkill::CastSkill(m_iBattlefieldObjID, stCounterAtkUnitObj.GetCombatUnitID(), stTargetObj.GetUnitPosition(), iSkillID, SKILL_USE_FANJI);
    if(iRet)
    {
        LOGERROR("Failed to process counteratk skill, uint id %d, skill id %d, uin %u\n", stCounterAtkUnitObj.GetCombatUnitID(), iSkillID, m_uiActiveUin);
        return iRet;
    }

    return T_SERVER_SUCESS;
}

//�����鼼�ܵĺϻ�
int CBattlefieldObj::DoJointAttack(CCombatUnitObj& stTargetUnitObj, int iSkillID)
{
    //��ȡ�������ܵ�����
    const SFightUnitSkillConfig* pstSkillConfig = FightSkillCfgMgr().GetConfig(iSkillID);
    if(!pstSkillConfig)
    {
        LOGERROR("Failed to get fight skill config, invalid skill id %d\n", iSkillID);
        return T_ZONE_SYSTEM_INVALID_CFG;
    }

    //�ü����Ƿ��кϻ�
    if(pstSkillConfig->iJointAtkID == 0)
    {
        return T_SERVER_SUCESS;
    }

    //��ȡ�ϻ����ܵ�����
    const SJointAttackConfig* pstJointAtkConfig = JointAtkCfgMgr().GetConfig(pstSkillConfig->iJointAtkID);
    if(!pstJointAtkConfig)
    {
        LOGERROR("Failed to get joint attack config, invalid joint atk id %d\n", pstSkillConfig->iJointAtkID);
        return T_ZONE_SYSTEM_INVALID_CFG;
    }

    CCombatUnitObj* pstActionUnitObj = CCombatUtility::GetCombatUnitObj(m_iActionUnit);
    if(!pstActionUnitObj)
    {
        LOGERROR("Failed to get unit obj, unit id %d, uin %u\n", m_iActionUnit, m_uiActiveUin);
        return T_ZONE_SYSTEM_PARA_ERR;
    }

    //����������λ�ܱߵľŹ���
    int iRet = T_SERVER_SUCESS;
    TUNITPOSITION& stActionPos = pstActionUnitObj->GetUnitPosition();
    for(int iDeltaX = -1; iDeltaX <= 1; ++iDeltaX)
    {
        for(int iDeltaY = -1; iDeltaY <= 1; ++iDeltaY)
        {
            TUNITPOSITION stPos(stActionPos.iPosX+iDeltaX, stActionPos.iPosY+iDeltaY);
            CCombatUnitObj* pstJointUnitObj = GetCombatUnitByPos(stPos);
            if(!pstJointUnitObj || !IsUnitInSameCamp(*pstActionUnitObj, *pstJointUnitObj))
            {
                //û�е�λ���߲��ǹ����Ķ���
                continue;
            }

            //�жϵ�λ�����Ƿ�����ϻ�Ҫ��
            for(int i=0; i<pstJointAtkConfig->iNeedSkillNum; ++i)
            {
                if(pstJointAtkConfig->aiNeedSkills[i] == 0)
                {
                    continue;
                }

                if(!pstJointUnitObj->HasFightSkill(pstJointAtkConfig->aiNeedSkills[i]))
                {
                    //û������ϻ�����
                    continue;
                }

                //�ҵ����ϻ����ʹ�úϻ�����
                iRet = CCombatSkill::CastSkill(m_iBattlefieldObjID, pstJointUnitObj->GetCombatUnitID(), stTargetUnitObj.GetUnitPosition(), pstJointAtkConfig->aiNeedSkills[i], SKILL_USE_HEJI);
                if(iRet)
                {
                    LOGERROR("Failed to process joint attack skill, unit id %d, skill id %d, uin %u\n", pstJointUnitObj->GetCombatUnitID(), pstJointAtkConfig->aiNeedSkills[i], m_uiActiveUin);
                }
            }
        }
    }

    return T_SERVER_SUCESS;
}

//�ж�ս����λ�Ƿ�����
bool CBattlefieldObj::IsCombatUnitDead(int iUnitID)
{
    CCombatUnitObj* pstUnitObj = CCombatUtility::GetCombatUnitObj(iUnitID);
    if(!pstUnitObj)
    {
        return true;
    }

    return pstUnitObj->IsCombatUnitDead();
}

//��ȡ�赲���͵Ļ���
CCombatTrapObj* CBattlefieldObj::GetBlockTrapByPos(const TUNITPOSITION& stPos)
{
    CCombatTrapObj* pstTrapObj = GetTrapByPos(stPos);
    if(!pstTrapObj || pstTrapObj->GetConfig()->iTrapType!=COMBAT_TRAP_ORGAN)
    {
        return NULL;
    }

    return pstTrapObj;
}

//���ݻ��ص�λ�û�ȡ����
CCombatTrapObj* CBattlefieldObj::GetTrapByPos(const TUNITPOSITION& stPos)
{
    for(int i=0; i<m_iTrapNum; ++i)
    {
        CCombatTrapObj* pstTrapObj = GameType<CCombatTrapObj>::Get(m_aiCombatTrapIndex[i]);
        if(!pstTrapObj)
        {
            continue;
        }

        if(pstTrapObj->GetPosition().iPosX == stPos.iPosX && 
           pstTrapObj->GetPosition().iPosY == stPos.iPosY)
        {
            return pstTrapObj;
        }
    }

    return NULL;
}

int CBattlefieldObj::GetTrapIndexByPos(const TUNITPOSITION& stPos)
{
    for(int i=0; i<m_iTrapNum; ++i)
    {
        CCombatTrapObj* pstTrapObj = GameType<CCombatTrapObj>::Get(m_aiCombatTrapIndex[i]);
        if(!pstTrapObj)
        {
            continue;
        }

        if(pstTrapObj->GetPosition().iPosX == stPos.iPosX && 
           pstTrapObj->GetPosition().iPosY == stPos.iPosY)
        {
            return m_aiCombatTrapIndex[i];
        }
    }

    return -1;
}

//ս��״̬�����ű�ʹ��
bool CBattlefieldObj::GetCombatStatus(int iStatusType)
{
    return ((m_ucCombatStatus>>iStatusType) & 0x01);
}

void CBattlefieldObj::SetCombatStatus(int iStatusType, bool bIsSet)
{
    if(bIsSet)
    {
        m_ucCombatStatus = (m_ucCombatStatus | (0x01<<iStatusType));
    }
    else
    {
        m_ucCombatStatus = (m_ucCombatStatus & ~(0x01<<iStatusType));
    }

    return;
}

//��ȡ��ͼ����Чվλλ��
void CBattlefieldObj::GetBattlefieldValidPos(const SBattleCrossConfig& stCrossConfig)
{
    //��ʼ��������վλλ��
    int iValidPosNum = 0;
    for(int i=0; i<stCrossConfig.iSelfUnitNum; ++i)
    {
        if(stCrossConfig.astFightPosInfo[i].iPosX < 0)
        {
            continue;
        }

        m_astActivePos[iValidPosNum].iPosX = stCrossConfig.astFightPosInfo[i].iPosX;
        m_astActivePos[iValidPosNum].iPosY = stCrossConfig.astFightPosInfo[i].iPosY;
        m_astActivePos[iValidPosNum].iDirection = stCrossConfig.astFightPosInfo[i].iDirection;
        ++iValidPosNum;
    }

    //��ʼ��������վλλ��
    iValidPosNum = 0;
    for(int i=0; i<stCrossConfig.iEnemyUnitNum; ++i)
    {
        if(stCrossConfig.astEnemyFightPosInfo[i].iPosX < 0)
        {
            continue;
        }

        m_astPassivePos[iValidPosNum].iPosX = stCrossConfig.astEnemyFightPosInfo[i].iPosX;
        m_astPassivePos[iValidPosNum].iPosY = stCrossConfig.astEnemyFightPosInfo[i].iPosY;
        m_astPassivePos[iValidPosNum].iDirection = stCrossConfig.astEnemyFightPosInfo[i].iDirection;
        ++iValidPosNum;
    }

    return;
}

//����CCombatUnitObj,��������ObjIndex
CCombatUnitObj* CBattlefieldObj::CreateCombatUnitObj(int& iCombatUnitObjIndex)
{
    iCombatUnitObjIndex = GameType<CCombatUnitObj>::Create();
    if(iCombatUnitObjIndex < 0)
    {
        LOGERROR("Failed to create combat unit obj!\n");
        return NULL;
    }

    CCombatUnitObj* pstCombatUnitObj = GameType<CCombatUnitObj>::Get(iCombatUnitObjIndex);
    if(!pstCombatUnitObj)
    {
        LOGERROR("Failed to get combat unit obj!\n");
        return NULL;
    }

    return pstCombatUnitObj;
}

//ɾ��һ��ս����λ
void CBattlefieldObj::ClearOneCombatUnitObj(int iCombatUnitID)
{
    CCombatUnitObj* pstUnitObj = CCombatUtility::GetCombatUnitObj(iCombatUnitID);
    if(!pstUnitObj)
    {
        return;
    }

    //����BUFF
    pstUnitObj->ClearUnitBuff();

    GameType<CCombatUnitObj>::Del(iCombatUnitID);

    return;
}
