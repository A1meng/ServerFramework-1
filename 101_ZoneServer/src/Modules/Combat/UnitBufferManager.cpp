
#include "GameProtocol.hpp"
#include "LogAdapter.hpp"
#include "ZoneErrorNumDef.hpp"
#include "ZoneObjectHelper.hpp"
#include "UnitBufferObj.hpp"
#include "ModuleShortcut.hpp"
#include "CombatUnitObj.hpp"
#include "CombatUtility.hpp"
#include "ModuleHelper.hpp"

#include "UnitBufferManager.hpp"

using namespace ServerLib;

CUnitBufferManager::CUnitBufferManager()
{
    Initialize();
}

CUnitBufferManager::~CUnitBufferManager()
{

}

int CUnitBufferManager::Initialize()
{
    m_iUnitID = -1;

    for(int i=0; i<MAX_FIGHT_UNIT_BUFF_NUM; ++i)
    {
        m_aiUnitBuffObjIndex[i] = -1;
    }

    m_iUnitBuffObjNum = 0;

    return 0;
}

//ӵ�е�ս����λ��UnitID
int CUnitBufferManager::GetOwnerUnitID()
{
    return m_iUnitID;
}

void CUnitBufferManager::SetOwnerUnitID(int iUnitID)
{
    m_iUnitID = iUnitID;
}

//����һ��Buff
int CUnitBufferManager::AddUnitBuff(unsigned int uin, int iCrossID, int iBuffID, int iCastUnitID, Zone_CombatAddBuff_Notify& stNotify)
{
    if(iBuffID == 0)
    {
        //û���¼�BUFF
        return T_SERVER_SUCESS;
    }

    //��ȡBuff����
    const SFightBuffConfig* pstBuffConfig = FightBuffCfgMgr().GetConfig(iBuffID);
    if(!pstBuffConfig)
    {
        LOGERROR("Failed to get fight buff config, invalid buff id %d\n", iBuffID);
        return T_ZONE_SYSTEM_INVALID_CFG;
    }

    //����֪ͨ������
    stNotify.set_icastunitid(iCastUnitID);

    //����һ��Ŀ��
    OneAddBuff* pstOneAddBuff = stNotify.add_staddbuffs();
    pstOneAddBuff->set_itargetunitid(m_iUnitID);

    //����һ�����ص�BUFF��Ϣ
    AddBuffInfo* pstOneBuffNotify = pstOneAddBuff->add_stbuffinfo();

    //����е������ͣ��������
    if(pstBuffConfig->iBuffType != BUFF_OVERLYING_ALL)
    {
        //�е������ͣ����ж�Buff����
        bool bCanOverlying = ProcessOverlyingBuff(pstBuffConfig->iBuffType, pstBuffConfig->iBuffLevel, *(pstOneBuffNotify->mutable_stremovebuff()));
        if(!bCanOverlying)
        {
            //�µ�Buff���ܵ���
            return T_SERVER_SUCESS;
        }
    }

    pstOneBuffNotify->set_ibuffid(iBuffID);

    if(pstBuffConfig->iEffectRound != 0)
    {
        //�лغ������Ƶ�BUFF����Ҫ��¼BUFF��Ϣ

        //�����µ�BuffObj
        CUnitBufferObj* pstNewBuffObj = CreateNewBufferObj();
        if(!pstNewBuffObj)
        {
            LOGERROR("Failed to create new buffer obj!\n");
            return T_ZONE_SYSTEM_PARA_ERR;
        }
    
        //��ʼ���µ�Buff
        pstNewBuffObj->InitUnitBuff(iBuffID, iCastUnitID, pstBuffConfig->iEffectRound);
    }

    int iRet = T_SERVER_SUCESS;
    //�Ƿ�ʱ��Ч��Buff
    if(pstBuffConfig->iTriggerType == BUFF_TRIGGER_IMMEDIATELY)
    {
        //��ʱ��Ч��Buff������BuffЧ��
        iRet = DoOneBuffEffect(uin, iCrossID, *pstBuffConfig, *(pstOneBuffNotify->mutable_steffects()), iCastUnitID);
        if(iRet)
        {
            LOGERROR("Failed to process one buff effect, ret %d\n", iRet);
            return iRet;
        }

        //todo jasonxiong5 ���¿����ű�ϵͳ
        //����BUFF��Ч�����Ľű�
        //CModuleHelper::GetStoryFramework()->DoBuffScript(uin, iCrossID, pstBuffConfig->iConfigID, m_iUnitID, iCastUnitID, iCastUnitID);
    }

    //�ж��Ƿ���ͬ����Buff������������
    if(pstBuffConfig->iAddBuffID != 0)
    {
        iRet = AddUnitBuff(uin, iCrossID, pstBuffConfig->iAddBuffID, iCastUnitID, stNotify);
        if(iRet)
        {
            LOGERROR("Failed to add new unit buff, ret %d\n", iRet);
            return iRet;
        }
    }

    return T_SERVER_SUCESS;
}

//��������ЧBuff
int CUnitBufferManager::DoUnitBuffEffect(unsigned int uin, int iCrossID, int iTriggerType, int iTriggerUnitID, Zone_DoBuffEffect_Notify& stNotify, int* pDamageNum)
{
    stNotify.set_itriggerunitid(iTriggerUnitID);

    for(int i=0; i<m_iUnitBuffObjNum; ++i)
    {
        CUnitBufferObj* pstBuffObj = GameType<CUnitBufferObj>::Get(m_aiUnitBuffObjIndex[i]);
        if(!pstBuffObj)
        {
            LOGERROR("Failed to get unit buff obj, invalid index %d\n", m_aiUnitBuffObjIndex[i]);

            continue;
        }

        //�ж��Ƿ�ʣ��غ���
        if(pstBuffObj->GetRemainRound() <= 0)
        {
            continue;
        }

        //��ȡBuff����
        const SFightBuffConfig* pstBuffConfig = FightBuffCfgMgr().GetConfig(pstBuffObj->GetUnitBuffID());
        if(!pstBuffConfig)
        {
            LOGERROR("Failed to get fight buff config, invalid buff id %d\n", pstBuffObj->GetUnitBuffID());
            return T_ZONE_SYSTEM_INVALID_CFG;
        }

        //�жϴ��������Ƿ�һ��
        if(pstBuffConfig->iTriggerType != iTriggerType)
        {
            continue;
        }

        //��Ч����Buff
        int iRet = DoOneBuffEffect(uin, iCrossID, *pstBuffConfig, *(stNotify.add_steffects()), pstBuffObj->GetCastUnitID(), iTriggerUnitID, pDamageNum);
        if(iRet)
        {
            LOGERROR("Failed to process one buff effect, ret %d, buff id %d\n", iRet, pstBuffObj->GetUnitBuffID());
            return iRet;
        }

        //todo jasonxiong5 ���¿����ű�
        //����BUFF��Ч�����Ľű�
        //CModuleHelper::GetStoryFramework()->DoBuffScript(uin, iCrossID, pstBuffObj->GetUnitBuffID(), m_iUnitID, pstBuffObj->GetCastUnitID(), iTriggerUnitID);
    }

    return T_SERVER_SUCESS;
}

//��������BUFF�Ļغ���
int CUnitBufferManager::DecreaseAllBuffRound(Zone_RemoveBuff_Notify& stNotify)
{
    int iRet = T_SERVER_SUCESS;

    stNotify.set_itargetunitid(m_iUnitID);

    //�������е�BUFF
    for(int i=0; i<m_iUnitBuffObjNum;)
    {
        CUnitBufferObj* pstBuffObj = GameType<CUnitBufferObj>::Get(m_aiUnitBuffObjIndex[i]);
        if(!pstBuffObj)
        {
            ++i;
            continue;
        }
        
        if(pstBuffObj->GetRemainRound() > 1)
        {
            //BUFF����Ч�����ٻغ���
            pstBuffObj->SetRemainRound(pstBuffObj->GetRemainRound()-1);

            ++i;
            continue;
        }

        //BUFFʧЧ���Ƴ�BUFF
        iRet = DelUnitBuffByIndex(m_aiUnitBuffObjIndex[i], *stNotify.add_steffects());
        if(iRet)
        {
            LOGERROR("Failed to delete unit buff, ret %d\n", iRet);
            return iRet;
        }
    }

    return T_SERVER_SUCESS;
}

//��ȡ����BUFF������ID
void CUnitBufferManager::GetUnitBuffID(std::vector<int>& vBuffIDs)
{
    for(int i=0; i<m_iUnitBuffObjNum; ++i)
    {
        CUnitBufferObj* pstBuffObj = GameType<CUnitBufferObj>::Get(m_aiUnitBuffObjIndex[i]);
        if(!pstBuffObj)
        {
            continue;
        }

        vBuffIDs.push_back(pstBuffObj->GetUnitBuffID());
    }

    return;
}

//�Ƿ���BUFFIDͬ���͵�BUFF
bool CUnitBufferManager::HasBuffOfSameType(int iBuffID)
{
    //��ȡBUFF����
    const SFightBuffConfig* pstNewBuffConfig = FightBuffCfgMgr().GetConfig(iBuffID);
    if(!pstNewBuffConfig)
    {
        return false;
    }

    for(int i=0; i<m_iUnitBuffObjNum; ++i)
    {
        if(m_aiUnitBuffObjIndex[i] < 0)
        {
            continue;
        }

        CUnitBufferObj* pstBuffObj = GameType<CUnitBufferObj>::Get(m_aiUnitBuffObjIndex[i]);
        if(!pstBuffObj)
        {
            continue;
        }

        //�ж��Ƿ�ͬ���͵�Buff
        const SFightBuffConfig* pstBuffConfig = FightBuffCfgMgr().GetConfig(pstBuffObj->GetUnitBuffID());
        if(!pstBuffConfig)
        {
            LOGERROR("Failed to get fight unit buff config, invalid buff id %d\n", pstBuffObj->GetUnitBuffID());
            continue;
        }

        if(pstBuffConfig->iBuffType == pstNewBuffConfig->iBuffType)
        {
            return true;
        }
    }

    return false;
}

//�Ƿ�����ͬID��BUFF
bool CUnitBufferManager::HasBuffOfSameID(int iBuffID)
{
    if(iBuffID <= 0)
    {
        return false;
    }

    for(int i=0; i<m_iUnitBuffObjNum; ++i)
    {
        if(m_aiUnitBuffObjIndex[i] < 0)
        {
            continue;
        }

        CUnitBufferObj* pstBuffObj = GameType<CUnitBufferObj>::Get(m_aiUnitBuffObjIndex[i]);
        if(!pstBuffObj)
        {
            continue;
        }

        if(pstBuffObj->GetUnitBuffID() == iBuffID)
        {
            return true;
        }
    }

    return false;
}

//����BuffManager�����е�BuffObj
void CUnitBufferManager::ClearBuffObj()
{
    for(int i=0; i<MAX_FIGHT_UNIT_BUFF_NUM; ++i)
    {
        if(m_aiUnitBuffObjIndex[i] < 0)
        {
            continue;
        }

        GameType<CUnitBufferObj>::Del(m_aiUnitBuffObjIndex[i]);

        m_aiUnitBuffObjIndex[i] = -1;
    }

    return;
}

//����Buff��Ч��ʵ��Ч��
int CUnitBufferManager::DoOneBuffEffect(unsigned int uin, int iCrossID, const SFightBuffConfig& stBuffConfig, BuffEffect& stEffectNotify, int iCastUnitID, int iTriggerUnitID, int* pDamageNum)
{
    int iRet = T_SERVER_SUCESS;

    //����Buff��Ч��Ŀ�ִ꣬����Ӧ�Ĳ���
    switch(stBuffConfig.aiProcessID[BUFF_PARAM_TARGET])
    {
        case BUFF_TARGET_SELF:
            {
                //��Buff�ĵ�λ����
                iRet = ProcessRealBuffEffect(uin, iCrossID, stBuffConfig, m_iUnitID, stEffectNotify);
            }
            break;

        case BUFF_TARGET_CAST:
            {
                //ʩ����,��������ֻ���Ǽ�BUFF
                if(stBuffConfig.iProcessType!=BUFF_OPERA_ADDBUFF && stBuffConfig.iProcessType!=BUFF_OPERA_GUARD)
                {
                    iRet = T_ZONE_SYSTEM_INVALID_CFG;
                }
                else
                {
                    iRet = ProcessRealBuffEffect(uin, iCrossID, stBuffConfig, iCastUnitID, stEffectNotify, pDamageNum);
                }   
            }
            break;

        case BUFF_TARGET_TRIGGER:
            {
                //������,��������ֻ���Ǽ�BUFF
                if(stBuffConfig.iProcessType != BUFF_OPERA_ADDBUFF && stBuffConfig.iProcessType!=BUFF_OPERA_GUARD)
                {
                    iRet = T_ZONE_SYSTEM_INVALID_CFG;
                }
                else
                {
                    iRet = ProcessRealBuffEffect(uin, iCrossID, stBuffConfig, iTriggerUnitID, stEffectNotify, pDamageNum);
                }   
            }
            break;

        default:
            {
                LOGERROR("Failed to do buff effect, invalid target type %d, buff id %d\n", stBuffConfig.aiProcessID[BUFF_PARAM_TARGET], stBuffConfig.iConfigID);
                return T_ZONE_SYSTEM_INVALID_CFG;
            }
            break;
    }

    if(iRet)
    {
        LOGERROR("Failed to process buff effect, ret %d, unit id %d, buff id %d\n", iRet, m_iUnitID, stBuffConfig.iConfigID);
        return iRet;
    }

    return T_SERVER_SUCESS;
}

//������Ե��ӵ�Buff,����true��ʾ��Buff���Ե�����ȥ
bool CUnitBufferManager::ProcessOverlyingBuff(int iBuffOverlyingType, int iBuffLevel, RemoveBuffEffect& stRemoveBuffNotify)
{
    for(int i=0; i<m_iUnitBuffObjNum; ++i)
    {
        if(m_aiUnitBuffObjIndex[i] < 0)
        {
            continue;
        }

        CUnitBufferObj* pstBuffObj = GameType<CUnitBufferObj>::Get(m_aiUnitBuffObjIndex[i]);
        if(!pstBuffObj)
        {
            continue;
        }

        //�ж��Ƿ�ͬ���͵�Buff
        const SFightBuffConfig* pstBuffConfig = FightBuffCfgMgr().GetConfig(pstBuffObj->GetUnitBuffID());
        if(!pstBuffConfig)
        {
            LOGERROR("Failed to get fight unit buff config, invalid buff id %d\n", pstBuffObj->GetUnitBuffID());
            continue;
        }

        if(pstBuffConfig->iBuffType != iBuffOverlyingType)
        {
            continue;
        }

        //�ж���Buffǿ��
        if(iBuffLevel < pstBuffConfig->iBuffLevel)
        {
            //ͬ���͵Ĳ����滻
            return false;
        }

        //�µ��ܹ��滻�ϵģ��Ƴ��ϵ�Buff
        DelUnitBuffByIndex(m_aiUnitBuffObjIndex[i], stRemoveBuffNotify);

        //���ؿ����滻
        return true;
    }

    return true;
}

//��Ŀ����Чһ��BUFF��ʵ��Ч��
int CUnitBufferManager::ProcessRealBuffEffect(unsigned int uin, int iCrossID, const SFightBuffConfig& stBuffConfig, int iTargetUnitID, BuffEffect& stEffectNotify, int* pDamageNum)
{
    //�ж�Buff�Ĳ��������Ƿ���ȷ
    if(iTargetUnitID==m_iUnitID && (stBuffConfig.iProcessType==BUFF_OPERA_ADDBUFF || stBuffConfig.iProcessType==BUFF_OPERA_GUARD))
    {
        LOGERROR("Failed to process attr change buff on other units, buff id %d\n", stBuffConfig.iConfigID);
        return T_ZONE_SYSTEM_INVALID_CFG;
    }

    //��ȡĿ�����
    CCombatUnitObj* pstTargetUnitObj = CCombatUtility::GetCombatUnitObj(iTargetUnitID);
    if(!pstTargetUnitObj)
    {
        LOGERROR("Failed to get combat unit obj, invalid unit id %d\n", iTargetUnitID);
        return T_ZONE_SYSTEM_PARA_ERR;
    }

    //������Ч��BUFF ID��Ŀ��
    stEffectNotify.set_ibuffid(stBuffConfig.iConfigID);
    stEffectNotify.set_itargetunitid(iTargetUnitID);

    int iRet = T_SERVER_SUCESS;

    //���ݲ������ͣ�ִ����Ӧ�Ĳ���
    switch(stBuffConfig.iProcessType)
    {
        case BUFF_OPERA_ADDATTR:
            {
                //��Ŀ�������
                int iEffect = 0;
                pstTargetUnitObj->AddFightAttr(stBuffConfig.aiProcessID[BUFF_PARAM_ATTRBUFF], stBuffConfig.aiProcessID[BUFF_PARAM_VALUE], &iEffect);

                if(stBuffConfig.iEffectType == BUFF_EFFECT_ROUNDS)
                {
                    //�����ã���Ҫ��¼��Ч��Ч��
                    //��ȡBuffObj
                    CUnitBufferObj* pstBuffObj = GetUnitBufferObjByID(stBuffConfig.iConfigID);
                    if(!pstBuffObj)
                    {
                        LOGERROR("Failed to do one buff effect, invalid buff id %d\n", stBuffConfig.iConfigID);
                        return T_ZONE_SYSTEM_PARA_ERR;
                    }

                    pstBuffObj->SetBuffEffect(pstBuffObj->GetBuffEffect()+iEffect);
                }

                //���������滻����Buff���滻ǰ��ֵ
                ProcessReplaceAttrBuffValue(stBuffConfig.aiProcessID[BUFF_PARAM_ATTRBUFF], iEffect);

                stEffectNotify.set_iattrtype(stBuffConfig.aiProcessID[BUFF_PARAM_ATTRBUFF]);
                stEffectNotify.set_iattrchange(iEffect);
            }
            break;

        case BUFF_OPERA_ADDRATEATTR:
            {
                //��Ŀ��İٷֱ�����
                int iAddAttr = 0;
                if(stBuffConfig.aiProcessID[BUFF_PARAM_ATTRBUFF] == FIGHT_ATTR_HP)
                {
                    //Ѫ��ȡ���Ѫ��
                    iAddAttr = pstTargetUnitObj->GetFightAttr(FIGHT_ATTR_HPMAX)*stBuffConfig.aiProcessID[BUFF_PARAM_VALUE]/100;
                }
                else
                {
                    //���������Զ�ȡ��ǰֵ
                    iAddAttr = pstTargetUnitObj->GetFightAttr(stBuffConfig.aiProcessID[BUFF_PARAM_ATTRBUFF])*stBuffConfig.aiProcessID[BUFF_PARAM_VALUE]/100;
                }

                int iEffect = 0;
                pstTargetUnitObj->AddFightAttr(stBuffConfig.aiProcessID[BUFF_PARAM_ATTRBUFF], iAddAttr, &iEffect);

                if(stBuffConfig.iEffectType == BUFF_EFFECT_ROUNDS)
                {
                    //�����ã���Ҫ��¼��Ч��Ч��

                    //��ȡBuffObj
                    CUnitBufferObj* pstBuffObj = GetUnitBufferObjByID(stBuffConfig.iConfigID);
                    if(!pstBuffObj)
                    {
                        LOGERROR("Failed to do one buff effect, invalid buff id %d\n", stBuffConfig.iConfigID);
                        return T_ZONE_SYSTEM_PARA_ERR;
                    }

                    pstBuffObj->SetBuffEffect(pstBuffObj->GetBuffEffect()+iEffect);
                }

                //���������滻����Buff���滻ǰ��ֵ
                ProcessReplaceAttrBuffValue(stBuffConfig.aiProcessID[BUFF_PARAM_ATTRBUFF], iEffect);

                stEffectNotify.set_iattrtype(stBuffConfig.aiProcessID[BUFF_PARAM_ATTRBUFF]);
                stEffectNotify.set_iattrchange(iEffect);
            }
            break;

        case BUFF_OPERA_REPLACEATTR:
            {
                //�滻Ŀ������
                int iOldAttr = pstTargetUnitObj->GetFightAttr(stBuffConfig.aiProcessID[BUFF_PARAM_ATTRBUFF]);

                int iAddAttr = stBuffConfig.aiProcessID[BUFF_PARAM_VALUE] - iOldAttr;

                pstTargetUnitObj->AddFightAttr(stBuffConfig.aiProcessID[BUFF_PARAM_ATTRBUFF], iAddAttr);

                if(stBuffConfig.iEffectType == BUFF_EFFECT_ROUNDS)
                {
                    //�����ã���Ҫ��¼ԭ��������ֵ

                    //��ȡBuffObj
                    CUnitBufferObj* pstBuffObj = GetUnitBufferObjByID(stBuffConfig.iConfigID);
                    if(!pstBuffObj)
                    {
                        LOGERROR("Failed to do one buff effect, invalid buff id %d\n", stBuffConfig.iConfigID);
                        return T_ZONE_SYSTEM_PARA_ERR;
                    }

                    pstBuffObj->SetBuffEffect(iOldAttr);
                }

                stEffectNotify.set_iattrtype(stBuffConfig.aiProcessID[BUFF_PARAM_ATTRBUFF]);
                stEffectNotify.set_iattrchange(iAddAttr);
            }
            break;

        case BUFF_OPERA_LOCKATTR:
            {
                //��ʱû�У�����
                ;
            }
            break;

        case BUFF_OPERA_ADDBUFF:
            {
                //��Ŀ������һ��Buff
                iRet = pstTargetUnitObj->AddUnitBuff(uin, iCrossID, stBuffConfig.aiProcessID[BUFF_PARAM_ATTRBUFF], m_iUnitID, *(stEffectNotify.mutable_staddbuff()));
            }
            break;

        case BUFF_OPERA_UNARM:
            {
                //��е������������Ч
                CCombatUtility::ProcessWeaponEffect(*pstTargetUnitObj, false);
            }
            break;

        case BUFF_OPERA_FORBIDACTION:
            {
                //��ֹ�ж�
                pstTargetUnitObj->SetCombatUnitStatus(COMBAT_UNIT_STATUS_NOTACTION, true);
            }
            break;

        case BUFF_OPERA_FORBIDROUND:
            {
                //��ֹ�����ж��غ�
                pstTargetUnitObj->SetCombatUnitStatus(COMBAT_UNIT_STATUS_NOTROUND, true);
            }
            break;

        case BUFF_OPERA_FORBIDMOVE:
            {
                //��ֹ�ƶ�
                pstTargetUnitObj->SetCombatUnitStatus(COMBAT_UNIT_STATUS_NOTMOVE, true);
            }
            break;

        case BUFF_OPERA_IGNOREJOINTATK:
            {
                //���Ӻ�Χ
                pstTargetUnitObj->SetCombatUnitStatus(COMBAT_UNIT_STATUS_IGNOREJOINT, true);
            }
            break;

        case BUFF_OPERA_UNBENDING:
            {
                //����
                pstTargetUnitObj->SetCombatUnitStatus(COMBAT_UNIT_STATUS_UNBENDING, true);
            }
            break;

        case BUFF_OPERA_GUARD:
            {
                //�ػ����͵�BUFF
                if(!pDamageNum)
                {
                    return T_ZONE_SYSTEM_PARA_ERR;
                }

                if(pstTargetUnitObj->IsCombatUnitDead())
                {
                    //Ŀ���Ѿ�����
                    return T_SERVER_SUCESS;
                }

                int iGuardDamage = (*pDamageNum) * stBuffConfig.aiProcessID[BUFF_PARAM_VALUE]/100;
                *pDamageNum -= iGuardDamage;

                int iRealGuardDamage = 0;
                pstTargetUnitObj->AddFightAttr(FIGHT_ATTR_HP, -iGuardDamage, &iRealGuardDamage);

                stEffectNotify.set_iattrtype(FIGHT_ATTR_HP);
                stEffectNotify.set_iattrchange(iRealGuardDamage);
            }
            break;

        default:
            {
                LOGERROR("Failed to do one buff effect, invalid opera type %d, buff id %d\n", stBuffConfig.iProcessType, stBuffConfig.iConfigID);
                return T_ZONE_SYSTEM_INVALID_CFG;
            }
            break;
    }

    if(iRet)
    {
        LOGERROR("Failed to do one buff effect, ret %d, buff id %d\n", iRet, stBuffConfig.iConfigID);
        return iRet;
    }

    return T_SERVER_SUCESS;
}

//���������滻����Buff���滻ǰ��ֵ
void CUnitBufferManager::ProcessReplaceAttrBuffValue(int iAttrType, int iAddNum)
{
   //�������е�BUFF�����������滻���͵�ֵ
    for(int i=0; i<m_iUnitBuffObjNum; ++i)
    {
        CUnitBufferObj* pstBuffObj= GameType<CUnitBufferObj>::Get(m_aiUnitBuffObjIndex[i]);
        if(!pstBuffObj)
        {
            continue;
        }

        const SFightBuffConfig* pstFightBuffConfig = FightBuffCfgMgr().GetConfig(pstBuffObj->GetUnitBuffID());
        if(!pstFightBuffConfig)
        {
            LOGERROR("Failed to get fight buff config, invalid buff id %d\n", pstBuffObj->GetUnitBuffID());
            return;
        }

        if(pstFightBuffConfig->iProcessType==BUFF_OPERA_REPLACEATTR && 
           pstFightBuffConfig->aiProcessID[BUFF_PARAM_ATTRBUFF]==iAttrType)
        {
            //��BUFF�м�¼��ֵ����ͬ���Ĵ���
            pstBuffObj->SetBuffEffect(pstBuffObj->GetBuffEffect()+iAddNum);
        }
    }

    return;
}

//����BuffObj��Indexɾ��Buff
int CUnitBufferManager::DelUnitBuffByIndex(int& iBuffObjIndex, RemoveBuffEffect& stRemoveBuffNotify)
{
    //��ȡBuffObj
    CUnitBufferObj* pstBuffObj = GameType<CUnitBufferObj>::Get(iBuffObjIndex);
    if(!pstBuffObj)
    {
        LOGERROR("Failed to get buffer unit obj, invalid obj index %d\n", iBuffObjIndex);
        return T_ZONE_SYSTEM_PARA_ERR;
    }

    //��ȡBUFF����
    const SFightBuffConfig* pstFightBuffConfig = FightBuffCfgMgr().GetConfig(pstBuffObj->GetUnitBuffID());
    if(!pstFightBuffConfig)
    {
        LOGERROR("Failed to get fight buff config, invalid buff id %d\n", pstBuffObj->GetUnitBuffID());
        return T_ZONE_SYSTEM_INVALID_CFG;
    }

    //���÷���
    RemoveOneBuffEffect* pstRemoveOneNotify = stRemoveBuffNotify.add_stremoveones();
    pstRemoveOneNotify->set_ibuffid(pstBuffObj->GetUnitBuffID());

    if(pstFightBuffConfig->iEffectType == BUFF_EFFECT_ROUNDS)
    {
        //��������Ч����Ҫ�Ƴ�BUFFЧ��
        int iRet = RemoveBuffEffects(*pstBuffObj, *pstFightBuffConfig, *pstRemoveOneNotify);
        if(iRet)
        {
            LOGERROR("Failed to remove buff effect, ret %d\n", iRet);
            return iRet;
        }
    }

    //ɾ��BUFF
    GameType<CUnitBufferObj>::Del(iBuffObjIndex);
    iBuffObjIndex = m_aiUnitBuffObjIndex[--m_iUnitBuffObjNum];

    //����й���BUFF������ɾ������BUFF
    if(pstFightBuffConfig->iDelBuffID != 0)
    {
        return DelUnitBuffByID(pstFightBuffConfig->iDelBuffID, stRemoveBuffNotify);
    }

    return T_SERVER_SUCESS;
}

//�Ƴ�BUFFЧ��������ID
int CUnitBufferManager::DelUnitBuffByID(int iBuffID, RemoveBuffEffect& stRemoveBuffNotify)
{
    for(int i=0; i<m_iUnitBuffObjNum; ++i)
    {
        CUnitBufferObj* pstBuffObj = GameType<CUnitBufferObj>::Get(m_aiUnitBuffObjIndex[i]);
        if(!pstBuffObj || pstBuffObj->GetUnitBuffID()!=iBuffID)
        {
            continue;
        }

        //ɾ����ӦID��BUFF
        return DelUnitBuffByIndex(m_aiUnitBuffObjIndex[i], stRemoveBuffNotify);
    }

    return T_SERVER_SUCESS;
}

//�Ƴ�BUFF��Ч��
int CUnitBufferManager::RemoveBuffEffects(CUnitBufferObj& stBuffObj, const SFightBuffConfig& stFightBuffConfig, RemoveOneBuffEffect& stRemoveOneNotify)
{
    //�ж�BUFF��Ч�����ͷ����ã�����Ŀ�����Լ�
    if(stFightBuffConfig.iEffectType == BUFF_EFFECT_FOREVER
       || stFightBuffConfig.aiProcessID[BUFF_PARAM_TARGET]!=BUFF_TARGET_SELF)
    {
        //���������ͣ����Ƴ�
        return T_SERVER_SUCESS;
    }

    //��ȡս����λCombatUnitObj
    CCombatUnitObj* pstCombatObj = CCombatUtility::GetCombatUnitObj(m_iUnitID);
    if(!pstCombatObj)
    {
        LOGERROR("Failed to get combat unit obj, combat unit id %d\n", m_iUnitID);
        return T_ZONE_SYSTEM_PARA_ERR;
    }

    //����BUFF���ͣ��Ƴ�BUFFЧ��
    switch(stFightBuffConfig.iProcessType)
    {
        case BUFF_OPERA_ADDATTR:
        case BUFF_OPERA_ADDRATEATTR:
            {
                int iRealEffect = 0;
                pstCombatObj->AddFightAttr(stFightBuffConfig.aiProcessID[BUFF_PARAM_ATTRBUFF], -stBuffObj.GetBuffEffect(), &iRealEffect);
                
                stRemoveOneNotify.set_iattrtype(stFightBuffConfig.aiProcessID[BUFF_PARAM_ATTRBUFF]);
                stRemoveOneNotify.set_iattrchange(iRealEffect);
            }
            break;

        case BUFF_OPERA_REPLACEATTR:
            {
                int iNewAttr = stBuffObj.GetBuffEffect();
                if(iNewAttr < 0)
                {
                    iNewAttr = 0;
                }

                int iAddAttr = iNewAttr - pstCombatObj->GetFightAttr(stFightBuffConfig.aiProcessID[BUFF_PARAM_ATTRBUFF]);
                pstCombatObj->AddFightAttr(stFightBuffConfig.aiProcessID[BUFF_PARAM_ATTRBUFF], iAddAttr);

                stRemoveOneNotify.set_iattrtype(stFightBuffConfig.aiProcessID[BUFF_PARAM_ATTRBUFF]);
                stRemoveOneNotify.set_iattrchange(iAddAttr);
            }
            break;

        case BUFF_OPERA_UNARM:
            {
                //�۳���е��Ч��
                CCombatUtility::ProcessWeaponEffect(*pstCombatObj, true);
            }
            break;

        case BUFF_OPERA_FORBIDACTION:
            {
                pstCombatObj->SetCombatUnitStatus(COMBAT_UNIT_STATUS_NOTACTION, false);
            }
            break;

        case BUFF_OPERA_FORBIDMOVE:
            {
                pstCombatObj->SetCombatUnitStatus(COMBAT_UNIT_STATUS_NOTMOVE, false);
            }
            break;

        case BUFF_OPERA_FORBIDROUND:
            {
                pstCombatObj->SetCombatUnitStatus(COMBAT_UNIT_STATUS_NOTROUND, false);
            }
            break;

        case BUFF_OPERA_UNBENDING:
            {
                pstCombatObj->SetCombatUnitStatus(COMBAT_UNIT_STATUS_UNBENDING, false);
            }
            break;

        case BUFF_OPERA_IGNOREJOINTATK:
            {
                pstCombatObj->SetCombatUnitStatus(COMBAT_UNIT_STATUS_IGNOREJOINT, false);
            }
            break;

        case BUFF_OPERA_GUARD:
            {
                ;
            }
            break;

        case BUFF_OPERA_ADDBUFF:
        case BUFF_OPERA_LOCKATTR:
        default:
            {
                LOGERROR("Failed to remove buff effect, invalid buff opera type %d, buff id %d\n", stFightBuffConfig.iProcessType, stBuffObj.GetUnitBuffID());
                return T_ZONE_SYSTEM_INVALID_CFG;
            }
            break;
    }

    return T_SERVER_SUCESS;
}

//����ID����BuffObj
CUnitBufferObj* CUnitBufferManager::GetUnitBufferObjByID(int iBuffID)
{
    for(int i=0; i<m_iUnitBuffObjNum; ++i)
    {
        CUnitBufferObj* pstBuffObj = GameType<CUnitBufferObj>::Get(m_aiUnitBuffObjIndex[i]);
        if(!pstBuffObj)
        {
            continue;
        }

        if(pstBuffObj->GetUnitBuffID() == iBuffID)
        {
            return pstBuffObj;
        }
    }

    return NULL;
}

//����һ���µ�BufferObj
CUnitBufferObj* CUnitBufferManager::CreateNewBufferObj()
{
    if(m_iUnitBuffObjNum >= MAX_FIGHT_UNIT_BUFF_NUM)
    {
        //����֧�ֵ�Buff����
        return NULL;
    }

    //��������ʼ���µ�BuffObj
    m_aiUnitBuffObjIndex[m_iUnitBuffObjNum] = GameType<CUnitBufferObj>::Create();
    if(m_aiUnitBuffObjIndex[m_iUnitBuffObjNum] < 0)
    {
        LOGERROR("Failed to create CUnitBufferObj!\n");
        return NULL;
    }

    CUnitBufferObj* pstNewBuffObj = GameType<CUnitBufferObj>::Get(m_aiUnitBuffObjIndex[m_iUnitBuffObjNum]);
    if(!pstNewBuffObj)
    {
        LOGERROR("Failed to get new buffer obj!\n");
        return NULL;
    }

    ++m_iUnitBuffObjNum;

    return pstNewBuffObj;
}
