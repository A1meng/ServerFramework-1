
#include "GameProtocol.hpp"
#include "LogAdapter.hpp"
#include "ZoneErrorNumDef.hpp"
#include "ZoneMsgHelper.hpp"
#include "ZoneObjectHelper.hpp"
#include "GameRole.hpp"
#include "ModuleShortcut.hpp"
#include "FightUnitObj.hpp"
#include "RepThingsUtility.hpp"
#include "ModuleHelper.hpp"
#include "ZoneOssLog.hpp"
#include "DropRewardUtility.hpp"

#include "RepThingsFramework.hpp"

using namespace ServerLib;

GameProtocolMsg CRepThingsFramework::m_stGameMsg;

CRepThingsFramework* CRepThingsFramework::Instance()
{
    static CRepThingsFramework* pInstance = NULL;
    if(!pInstance)
    {
        pInstance = new CRepThingsFramework();
    }

    return pInstance;
}

CRepThingsFramework::CRepThingsFramework()
{

}

CRepThingsFramework::~CRepThingsFramework()
{

}

//��ҶԱ�����Ʒ�Ĳ���
int CRepThingsFramework::OperaRepItem(unsigned int uin, const Zone_RepOpera_Request& rstRequest)
{
    //��ʼ����Ա����
    m_uiUin = uin;
    m_stGameMsg.Clear();

    CZoneMsgHelper::GenerateMsgHead(m_stGameMsg, MSGID_ZONE_REPOPERA_RESPONSE);

    CGameRoleObj* pstRoleObj = GameTypeK32<CGameRoleObj>::GetByKey(m_uiUin);
    if(!pstRoleObj)
    {
        LOGERROR("Failed to get game role obj, uin %u\n", m_uiUin);
        return T_ZONE_GAMEROLE_NOT_EXIST;
    }

    //��ȡ��Ӧ����Ϣ��
    Zone_RepOpera_Response* pstResp = m_stGameMsg.mutable_m_stmsgbody()->mutable_m_stzone_repopera_response();

    ReqOperaType eType = rstRequest.etype();

    int iRet = T_SERVER_SUCESS;
    switch(eType)
    {
        case REQ_OPERA_TYPE_SPLIT:
            {
                iRet = SplitRepItem(*pstRoleObj, rstRequest.islotindex(), rstRequest.iitemnum(), *pstResp);
            }
            break;

        case REQ_OPERA_TYPE_EXCHANGE:
            {
                iRet = ExchangeRepItem(*pstRoleObj, rstRequest.islotindex(), rstRequest.iotherslotindex(), *pstResp);
            }
            break;

        case REQ_OPERA_TYPE_USE:
            {
                iRet = UseRepItem(*pstRoleObj, rstRequest.islotindex(), rstRequest.iitemnum(), rstRequest.ifightunitid(), *pstResp);
            }
            break;

        case REQ_OPERA_TYPE_DELETE:
            {
                iRet = DeleteRepItem(*pstRoleObj, rstRequest.islotindex(), rstRequest.iitemnum(), *pstResp);
            }
            break;

        case REQ_OPERA_TYPE_SORT:
            {
                iRet = SortRepItem(*pstRoleObj, *pstResp);
            }
            break;

        default:
            {
                iRet = T_ZONE_SYSTEM_PARA_ERR;
            }
            break;
    }

    if(iRet)
    {
        LOGERROR("Failed to opera rep item, opera type %d, ret %d, uin %u\n", rstRequest.etype(), iRet, m_uiUin);
        return iRet;
    }

    //��䲢�ذ����ͻ���
    pstResp->set_etype(rstRequest.etype());
    pstResp->set_iresult(T_SERVER_SUCESS);

    SendSuccessResponse();

    return T_SERVER_SUCESS;
}

//�����ȡ������Ʒ����ϸ��Ϣ
int CRepThingsFramework::GetRepItemInfo(unsigned int uin, const Zone_GetRepInfo_Request& rstRequest)
{
    //��ʼ����Ա����
    m_uiUin = uin;
    m_stGameMsg.Clear();

    CZoneMsgHelper::GenerateMsgHead(m_stGameMsg, MSGID_ZONE_GETREPINFO_RESPONSE);

    CGameRoleObj* pstRoleObj = GameTypeK32<CGameRoleObj>::GetByKey(m_uiUin);
    if(!pstRoleObj)
    {
        LOGERROR("Failed to get game role obj, uin %u\n", m_uiUin);
        return T_ZONE_GAMEROLE_NOT_EXIST;
    }

    //��ȡ��Ӧ����Ϣ��
    Zone_GetRepInfo_Response* pstResp = m_stGameMsg.mutable_m_stmsgbody()->mutable_m_stzone_getrepinfo_response();

    //��ȡ����������
    pstRoleObj->GetRepThingsManager().UpdateRepItemToDB(*pstResp->mutable_stitems());

    //��䲢�ذ����ͻ���
    pstResp->set_iresult(T_SERVER_SUCESS);

    SendSuccessResponse();

    return T_SERVER_SUCESS;
}

//��Ҵӱ�����װ��
int CRepThingsFramework::WearEquipment(unsigned int uin, const Zone_WearEquip_Request& rstRequest)
{
    //todo jasonxiong5 ��������
    /*
    //��ʼ����Ա����
    m_uiUin = uin;
    m_stGameMsg.Clear();

    CZoneMsgHelper::GenerateMsgHead(m_stGameMsg, MSGID_ZONE_WEAREQUIP_RESPONSE);

    CGameRoleObj* pstRoleObj = GameTypeK32<CGameRoleObj>::GetByKey(m_uiUin);
    if(!pstRoleObj)
    {
        LOGERROR("Failed to get game role obj, uin %u\n", m_uiUin);
        return T_ZONE_GAMEROLE_NOT_EXIST;
    }

    int iRet = T_SERVER_SUCESS;

    //������װ����Ӧ����Ϣ��
    Zone_WearEquip_Response* pstResp = m_stGameMsg.mutable_m_stmsgbody()->mutable_m_stzone_wearequip_response();

    //��װ��
    if(rstRequest.biswear())
    {
        //��ȡ����������
        CRepThingsManager& stRepThingsManager = pstRoleObj->GetRepThingsManager();
    
        int iEquipItemID = stRepThingsManager.GetRepItemID(rstRequest.islotindex());
    
        const SEquipmentConfig* pstEquipConfig = EquipmentCfgMgr().GetConfig(iEquipItemID);
        if(!pstEquipConfig)
        {
            LOGERROR("Failed to get equipment config, equipment id %d\n", iEquipItemID);
            return T_ZONE_SYSTEM_INVALID_CFG;
        }
    
        //todo jasonxiong4 ����޸�Ϊ��IDȡ
        //��ȡս����λ����Ϣ
        CFightUnitObj* pstFightUnitObj = pstRoleObj->GetFightUnitManager().GetFightUnitByID(rstRequest.iunitindex());
        if(!pstFightUnitObj)
        {
            LOGERROR("Failed to get fight unit obj, index %d, uin %u\n", rstRequest.iunitindex(), m_uiUin);
            return T_ZONE_SYSTEM_PARA_ERR;
        }

        //��ɾ����λ�õ�װ��
        RepItem* stOldEquipment = pstFightUnitObj->GetItemInfo(pstEquipConfig->iEquipType);
        if(stOldEquipment->iItemID != 0)
        {
            //��ȡ��װ��������
            const SEquipmentConfig* pstOldEquipConfig = EquipmentCfgMgr().GetConfig(stOldEquipment->iItemID);
            if(!pstOldEquipConfig)
            {
                LOGERROR("Failed to get old equipment config, item id %d\n", stOldEquipment->iItemID);
                return T_ZONE_SYSTEM_INVALID_CFG;
            }

            iRet = pstFightUnitObj->UnitEquipItem(pstOldEquipConfig->iEquipType, *stOldEquipment, false);
            if(iRet)
            {
                LOGERROR("Failed to take off fight unit equipment, unit index %d, equip type %d, uin %u\n", rstRequest.iunitindex(), pstEquipConfig->iEquipType, m_uiUin);
                return iRet;
            }
        
            if(stOldEquipment->iItemID != 0)
            {
                //ս����λ�������µ�װ���ŵ�������
                if(stRepThingsManager.GetEmptyItemSlotIndex() < 0)
                {
                    //�����޿�λ������ж��
                    return T_ZONE_USER_REP_FULL;
                }
        
                iRet = stRepThingsManager.AddRepItem(*stOldEquipment, -1, 0);
                if(iRet)
                {
                    return iRet;
                }
            }
        }
    
        //�ӱ�����ɾ��װ��
        RepItem stWearEquip;
        iRet = stRepThingsManager.DeleteRepItem(rstRequest.islotindex(), stWearEquip, 0);
        if(iRet)
        {
            LOGERROR("Failed to delete rep equipment, slot %d, ret %d, uin %u\n", rstRequest.islotindex(), iRet, m_uiUin);
            return iRet;
        }
    
        //ս����λ���ϴ���װ��
        iRet = pstFightUnitObj->UnitEquipItem(pstEquipConfig->iEquipType, stWearEquip, true);
        if(iRet)
        {
            LOGERROR("Failed to wear equipment, unit index %d, equipment type %d, uin %u\n", rstRequest.iunitindex(), pstEquipConfig->iEquipType, m_uiUin);
            return iRet;
        }

        //��װ����
        OneSlotInfo* pstSlotInfo = pstResp->mutable_stslotinfo();
        pstSlotInfo->set_iitemid(stWearEquip.iItemID);
        pstSlotInfo->set_iitemnum(stWearEquip.iItemNum);
    }
    //��װ��
    else
    {
        CRepThingsManager& stRepTingsManager = pstRoleObj->GetRepThingsManager();

        //���жϱ������Ƿ��п�λ
        if(stRepTingsManager.GetEmptyItemSlotIndex() < 0)
        {
            LOGERROR("Failed to take off equipment, rep full, uin %u\n", m_uiUin);
            return T_ZONE_SYSTEM_PARA_ERR;
        }

        //��ȡս����λ����Ϣ
        CFightUnitManager& stFightUnitManager = pstRoleObj->GetFightUnitManager();

        //todo jasonxiong4 ����޸�Ϊ��IDȡ
        CFightUnitObj* pstUnitObj = stFightUnitManager.GetFightUnitByID(rstRequest.iunitindex());
        if(!pstUnitObj)
        {
            LOGERROR("Failed to get fight unit by index %d, uin %u\n", rstRequest.iunitindex(), m_uiUin);
            return T_ZONE_SYSTEM_PARA_ERR;
        }

        //��ȡ��װ����Ϣ
        RepItem* stOldEquipInfo = pstUnitObj->GetItemInfo(rstRequest.iequipsubtype());
        const SEquipmentConfig* pstEquipConfig = EquipmentCfgMgr().GetConfig(stOldEquipInfo->iItemID);
        if(!pstEquipConfig)
        {
            LOGERROR("Failed to get equipment config, invalid id %d\n", stOldEquipInfo->iItemID);
            return T_ZONE_SYSTEM_PARA_ERR;
        }

        //�ȴ�ս����λ��������װ��
        RepItem stEquipItem;
        iRet = pstUnitObj->UnitEquipItem(rstRequest.iequipsubtype(), stEquipItem, false);
        if(iRet)
        {
            LOGERROR("Failed to take off equipment, unit index %d, equip type %d, uin %u\n", rstRequest.iunitindex(), rstRequest.iequipsubtype(), m_uiUin);
            return iRet;
        }

        if(stEquipItem.iItemID==0 || stEquipItem.iItemNum==0)
        {
            LOGERROR("Failed to take off equipment, no equip, unit index %d, equip %d, uin %u\n", rstRequest.iunitindex(), rstRequest.iequipsubtype(), m_uiUin);
            return T_ZONE_SYSTEM_PARA_ERR;
        }

        //�����µ�װ���Ž�����
        iRet = stRepTingsManager.AddRepItem(stEquipItem, rstRequest.islotindex(), 0);
        if(iRet)
        {
            LOGERROR("Failed to add new rep item, ret %d, uin %u\n", iRet, m_uiUin);
            return iRet;
        }
    }

    //��䲢�ذ����ͻ���
    pstResp->set_biswear(rstRequest.biswear());
    pstResp->set_iunitindex(rstRequest.iunitindex());
    pstResp->set_iequipsubtype(rstRequest.iequipsubtype());
    pstResp->set_iresult(T_SERVER_SUCESS);

    SendSuccessResponse();
    */

    return T_SERVER_SUCESS;
}

//��ҳ��۱�����Ʒ
int CRepThingsFramework::SellRepItem(unsigned int uin, const Zone_SellItem_Request& rstRequest)
{
    //��ʼ����Ա����
    m_uiUin = uin;
    m_stGameMsg.Clear();

    CZoneMsgHelper::GenerateMsgHead(m_stGameMsg, MSGID_ZONE_SELLITEM_RESPONSE);

    CGameRoleObj* pstRoleObj = GameTypeK32<CGameRoleObj>::GetByKey(m_uiUin);
    if(!pstRoleObj)
    {
        LOGERROR("Failed to get game role obj, uin %u\n", m_uiUin);
        return T_ZONE_GAMEROLE_NOT_EXIST;
    }

    //��ȡ����������
    CRepThingsManager& rstRepThingsManager = pstRoleObj->GetRepThingsManager();

    int iRet = T_SERVER_SUCESS;

    //��ҳ��۱�����Ʒ�ķ���
    Zone_SellItem_Response* pstResp = m_stGameMsg.mutable_m_stmsgbody()->mutable_m_stzone_sellitem_response();

    //�۳���Ʒ
    for(int i=0; i<rstRequest.stitems_size(); ++i)
    {
        const SellItemInfo& rstSellInfo = rstRequest.stitems(i);

        //ɾ����Ʒ
        iRet = rstRepThingsManager.DeleteRepItem(rstSellInfo.iitemslot(), rstSellInfo.isellnum(), 0);
        if(iRet)
        {
            LOGERROR("Failed to delete rep item, item slot %d, num %d, uin %u\n", rstSellInfo.iitemslot(), rstSellInfo.isellnum(), m_uiUin);
            return iRet;
        }
    }

    //��䲢�ذ����ͻ���
    pstResp->set_iresult(T_SERVER_SUCESS);

    SendSuccessResponse();

    return T_SERVER_SUCESS;
}

//��ֱ��������е���Ʒ, iItemNum����Ҫ��ֵ�����Ʒ���������Ŀ
int CRepThingsFramework::SplitRepItem(CGameRoleObj& stRoleObj, int iSlotIndex, int iItemNum, Zone_RepOpera_Response& rstResponse)
{
    //�жϲ���
    if(iItemNum == 0)
    {
        LOGERROR("Failed to split rep item, invalid num %d, uin %u\n", iItemNum, m_uiUin);
        return T_ZONE_SYSTEM_PARA_ERR;
    }

    //��ȡ����������
    CRepThingsManager& stRepThingsManager = stRoleObj.GetRepThingsManager();

    int iRealItemNum = stRepThingsManager.GetRepItemNum(iSlotIndex);
    //�жϲ����Ʒ�������Ƿ�
    if(iRealItemNum <= iItemNum)
    {
        LOGERROR("Failed to spilt rep item, num need:real %d:%d, slot %d, uin %u\n", iItemNum, iRealItemNum, iSlotIndex, m_uiUin);
        return T_ZONE_SYSTEM_PARA_ERR;
    }

    //�жϲ�ֳ�������Ʒ�Ƿ��ܷŽ�ȥ
    int iEmptyRepSlot = stRepThingsManager.GetEmptyItemSlotIndex();
    if(iEmptyRepSlot < 0)
    {
        LOGERROR("Failed to split rep item, rep full, uin %u\n", m_uiUin);
        return T_ZONE_USER_REP_FULL;
    }

    //�ȼ�ԭ�����ӵ���Ʒ
    int iRet = stRepThingsManager.DeleteRepItem(iSlotIndex, iItemNum);
    if(iRet)
    {
        LOGERROR("Failed to delete rep item, slot %d, num %d, uin %u\n", iSlotIndex, iItemNum, m_uiUin);
        return iRet;
    }

    int iItemID = stRepThingsManager.GetRepItemID(iSlotIndex);

    //�����¸��������Ʒ
    RepItem stAddItem;
    stAddItem.iItemID = iItemID;
    stAddItem.iItemNum = iItemNum;

    iRet = stRepThingsManager.AddRepItem(stAddItem, iEmptyRepSlot);
    if(iRet)
    {
        LOGERROR("Failed to add rep item, item id %d, num %d, uin %u\n", iItemID, iItemNum, m_uiUin);
        return iRet;
    }

    LOGDEBUG("Success to split rep item, slot %d, num %d, uin %u\n", iSlotIndex, iItemNum, m_uiUin);

    return T_SERVER_SUCESS;
}

//�����ͺϲ����������е���Ʒ
int CRepThingsFramework::ExchangeRepItem(CGameRoleObj& stRoleObj, int iSlotIndex, int iOtherSlotIndex, Zone_RepOpera_Response& rstResponse)
{
    //��ȡ����������
    CRepThingsManager& stRepThingsManager = stRoleObj.GetRepThingsManager();

    //�����ͺϲ���������
    int iRet = stRepThingsManager.ExchangeRepItem(iSlotIndex, iOtherSlotIndex);
    if(iRet)
    {
        LOGERROR("Failed to exchange rep item, slot %d, other slot %d, uin %u\n", iSlotIndex, iOtherSlotIndex, m_uiUin);
        return iRet;
    }

    LOGDEBUG("Success to exchange rep item, slot %d, other slot %d, uin %u\n", iSlotIndex, iOtherSlotIndex, m_uiUin);

    return T_SERVER_SUCESS;
}

//ʹ�ñ����е���Ʒ
int CRepThingsFramework::UseRepItem(CGameRoleObj& stRoleObj, int iSlotIndex, int iItemNum, int iFightUnitID, Zone_RepOpera_Response& rstResponse)
{
    //��ȡ����������
    CRepThingsManager& stRepThingsManager = stRoleObj.GetRepThingsManager();

    //��ȡʹ����Ʒ��ID
    int iItemID = stRepThingsManager.GetRepItemID(iSlotIndex);

    //�����Ʒ�����Ƿ�
    if(iItemNum > stRepThingsManager.GetRepItemNum(iSlotIndex))
    {
        LOGERROR("Failed to use rep item, item id %d, uin %u\n", iItemID, m_uiUin);
        return T_ZONE_SYSTEM_PARA_ERR;
    }

    //�ȿ۳���Ʒ
    int iRet = stRepThingsManager.DeleteRepItem(iSlotIndex, iItemNum, 0);
    if(iRet)
    {
        LOGERROR("Failed to delete rep item, slot %d, num %d, ret %d, uin %u\n", iSlotIndex, iItemNum, iRet, m_uiUin);
        return iRet;
    }

    return T_SERVER_SUCESS;
}

//ɾ�������е���Ʒ
int CRepThingsFramework::DeleteRepItem(CGameRoleObj& stRoleObj, int iSlotIndex, int iItemNum, Zone_RepOpera_Response& rstResponse)
{
    //��ȡ����������
    CRepThingsManager& stRepThingsManager = stRoleObj.GetRepThingsManager();

    int iSlotItemNum = stRepThingsManager.GetRepItemNum(iSlotIndex);
    if(iSlotItemNum < iItemNum)
    {
        return T_ZONE_SYSTEM_PARA_ERR;
    }

    int iRet = stRepThingsManager.DeleteRepItem(iSlotIndex, iItemNum, 0);
    if(iRet)
    {
        LOGERROR("Failed to delete rep item, slot %d, uin %u\n", iSlotIndex, m_uiUin);
        return iRet;
    }

    LOGDEBUG("Success to delete rep item, slot %d, uin %u\n", iSlotIndex, m_uiUin);

    return T_SERVER_SUCESS;
}

//������Ʒ����
int CRepThingsFramework::SortRepItem(CGameRoleObj& stRoleObj, Zone_RepOpera_Response& rstResponse)
{
    //��ȡ����������
    CRepThingsManager& stRepThingsManager = stRoleObj.GetRepThingsManager();

    int iRet = stRepThingsManager.SortRepItem();
    if(iRet)
    {
        LOGERROR("Failed to sort rep item, ret %d, uin %u\n", iRet, m_uiUin);
        return iRet;
    }

    LOGDEBUG("Success to sort rep item, uin %u\n", m_uiUin);

    return T_SERVER_SUCESS;
}

int CRepThingsFramework::SendSuccessResponse()
{
    CGameRoleObj* pstRoleObj = GameTypeK32<CGameRoleObj>::GetByKey(m_uiUin);
    if(!pstRoleObj)
    {
        LOGERROR("Failed to get game role obj, uin %u\n", m_uiUin);
        return -1;
    }

    CZoneMsgHelper::SendZoneMsgToRole(m_stGameMsg, pstRoleObj);

    return 0;
}
