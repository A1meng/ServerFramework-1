
#include "GameProtocol.hpp"
#include "ZoneErrorNumDef.hpp"
#include "LogAdapter.hpp"
#include "RepThingsUtility.hpp"
#include "ModuleShortcut.hpp"
#include "ZoneObjectHelper.hpp"
#include "ZoneMsgHelper.hpp"

#include "RepThingsManager.hpp"

using namespace GameConfig;
using namespace ServerLib;

//��ұ�����Ʒ�����仯��֪ͨ
GameProtocolMsg CRepThingsManager::m_stRepChangeNotify;

CRepThingsManager::CRepThingsManager()
{
    Initialize();
}

CRepThingsManager::~CRepThingsManager()
{

}

//��ʼ��
int CRepThingsManager::Initialize()
{
    m_uiUin = 0;
    m_iRepBlockNum = 0;

    memset(m_astRepItem, 0, sizeof(m_astRepItem));

    return T_SERVER_SUCESS;
}

void CRepThingsManager::SetOwner(unsigned int uin)
{
    m_uiUin = uin;
}

CGameRoleObj* CRepThingsManager::GetOwner()
{
    return GameTypeK32<CGameRoleObj>::GetByKey(m_uiUin);
}

//����������Ʒ
int CRepThingsManager::AddRepItem(int iItemID, int iItemNum, int iItemChannel)
{
    if(iItemID<=0 || iItemNum<=0)
    {
        return T_ZONE_SYSTEM_PARA_ERR;
    }

    //�޸ĵ�slot��Ϣ
    std::vector<int> vItemSlot;

    int iType = CRepThingsUtility::GetItemType(iItemID);
    switch(iType)
    {
    case REP_ITEM_TYPE_EQUIP:
        {
            //��װ��,���ܵ���
            if(iItemNum != 1)
            {
                LOGERROR("Failed to add rep equipment, id %d, num %d, uin %u\n", iItemID, iItemNum, m_uiUin);
                return T_ZONE_SYSTEM_PARA_ERR;
            }

            int iItemSlot = GetEmptyItemSlot();
            if(iItemSlot == -1)
            {
                LOGERROR("Failed to get empty item slot, uin %u\n", m_uiUin);
                return T_ZONE_USER_REP_FULL;
            }
        
            //������Ʒ
            m_astRepItem[iItemSlot].iItemID = iItemID;
            m_astRepItem[iItemSlot].iItemNum = iItemNum;

            vItemSlot.push_back(iItemSlot);
        }
        break;

    case REP_ITEM_TYPE_PROP:
        {
            /*
            const SPropItemConfig* pstPropConfig = PropItemCfgMgr().GetConfig(iItemID);
            if(!pstPropConfig)
            {
                return T_ZONE_SYSTEM_INVALID_CFG;
            }

            //�ӵ��ߣ����Ե���
            for(int i=0; i<m_iRepBlockNum; ++i)
            {
                if(iItemNum == 0)
                {
                    break;
                }

                //�ȳ��Ե��ӵ���
                if(m_astRepItem[i].iItemID==iItemID && 
                   m_astRepItem[i].bIsBinded==bIsBinded &&
                   m_astRepItem[i].iItemNum<pstPropConfig->iMaxNum)
                {
                    if(m_astRepItem[i].iItemNum+iItemNum<=pstPropConfig->iMaxNum)
                    {
                        //����󲻳�����������
                        m_astRepItem[i].iItemNum += iItemNum;
                        iItemNum = 0;

                        vItemSlot.push_back(i);
                    }
                    else
                    {
                        //����󳬹�������
                        iItemNum -= (pstPropConfig->iMaxNum - m_astRepItem[i].iItemNum);

                        m_astRepItem[i].iItemNum = pstPropConfig->iMaxNum;
                        vItemSlot.push_back(i);
                    }                    
                }
            }

            if(iItemNum == 0)
            {
                break;
            }

            //����δ����ģ��ҿ�λ����
            for(int i=0; i<m_iRepBlockNum; ++i)
            {
                if(iItemNum == 0)
                {
                    break;
                }

                if(m_astRepItem[i].iItemID == 0)
                {
                    m_astRepItem[i].iItemID = iItemID;
                    m_astRepItem[i].iItemNum = (pstPropConfig->iMaxNum<iItemNum ? pstPropConfig->iMaxNum : iItemNum);
                    m_astRepItem[i].bIsBinded = bIsBinded;

                    vItemSlot.push_back(i);

                    iItemNum -= m_astRepItem[i].iItemNum;
                }
            }
            */
        }
        break;

    default:
        {
            LOGERROR("Failed to add rep item, invalid type %d, item id %d, uin %u\n", iType, iItemID, m_uiUin);
            return T_ZONE_SYSTEM_PARA_ERR;
        }
        break;
    }

    //���ͱ����仯��֪ͨ
    SendRepChangeNotify(vItemSlot, iItemChannel);

    return T_SERVER_SUCESS;
}

//����������Ʒ
int CRepThingsManager::AddRepItem(const RepItem& stItem, int iItemSlot, int iItemChannel)
{
    if(iItemSlot<0 || iItemSlot>=m_iRepBlockNum)
    {
        //ûָ���ŵĸ���,����ҿո�
        iItemSlot = GetEmptyItemSlot();
        if(iItemSlot < 0)
        {
            LOGERROR("Failed to get empty item slot, uin %u\n", m_uiUin);
            return T_ZONE_USER_REP_FULL;
        }
    }
    else
    {
        //ָ���˸���λ�ã����ж�ָ����λ���Ƿ�����Ʒ
        if(m_astRepItem[iItemSlot].iItemID != 0)
        {
            return T_ZONE_SYSTEM_PARA_ERR;
        }
    }


    //ֱ�Ӱ���Ʒ��Ϣ������ȥ
    memcpy((void*)&m_astRepItem[iItemSlot], (void*)&stItem, sizeof(stItem));

    //���ͱ����仯��֪ͨ
    std::vector<int> vItemSlot;
    vItemSlot.push_back(iItemSlot);

    SendRepChangeNotify(vItemSlot, iItemChannel);

    return T_SERVER_SUCESS;
}

//����ɾ����Ʒ
int CRepThingsManager::DeleteRepItem(int iItemSlot, int iItemNum, int iItemChannel)
{
    if(iItemSlot<0 || iItemSlot>=m_iRepBlockNum)
    {
        LOGERROR("Failed to delete rep item, slot %d, num %u\n", iItemSlot, iItemNum);
        return T_ZONE_SYSTEM_PARA_ERR;
    }

    RepItem& stItem = m_astRepItem[iItemSlot];
    int iNewItemNum = stItem.iItemNum - iItemNum;
    if(iNewItemNum < 0)
    {
        LOGERROR("Failed to delete rep item, need:real %d:%d, slot %d, uin %u\n", iItemNum, stItem.iItemNum, iItemSlot, m_uiUin);
        return T_ZONE_SYSTEM_PARA_ERR;
    }
    
    stItem.iItemNum = iNewItemNum;
    if(stItem.iItemNum == 0)
    {
        stItem.Reset();
    }

    //���ͱ����仯��֪ͨ
    std::vector<int> vItemSlot;
    vItemSlot.push_back(iItemSlot);

    SendRepChangeNotify(vItemSlot, iItemChannel);

    return T_SERVER_SUCESS;
}

//����ɾ����Ʒ
int CRepThingsManager::DeleteRepItem(int iItemSlot, RepItem& stDeleteItem, int iItemChannel)
{
    if(iItemSlot<0 || iItemSlot>=m_iRepBlockNum)
    {
        LOGERROR("Failed to delete rep item, slot %d\n", iItemSlot);
        return T_ZONE_SYSTEM_PARA_ERR;
    }

    RepItem& stItem = m_astRepItem[iItemSlot];

    memcpy(&stDeleteItem, &stItem, sizeof(stItem));

    stItem.Reset();

    //���ͱ����仯��֪ͨ
    std::vector<int> vItemSlot;
    vItemSlot.push_back(iItemSlot);

    SendRepChangeNotify(vItemSlot, iItemChannel);

    return T_SERVER_SUCESS;
}

//����ɾ����Ʒ
int CRepThingsManager::DeleteRepItemByID(int iItemID, int iItemChannel)
{
    for(int i=0; i<m_iRepBlockNum; ++i)
    {
        if(m_astRepItem[i].iItemID==iItemID && m_astRepItem[i].iItemNum>0)
        {
            return DeleteRepItem(i, 1, iItemChannel);
        }
    }

    return T_ZONE_SYSTEM_PARA_ERR;
}

//����ɾ���̶���������Ʒ
int CRepThingsManager::DeleteRepItemByID(int iItemID, int iItemNum, int iItemChannel)
{
    //���ж������Ƿ��㹻
    int iTotalNum = 0;
    for(int i=0; i<m_iRepBlockNum; ++i)
    {
        if(m_astRepItem[i].iItemID==iItemID)
        {
            iTotalNum += m_astRepItem[i].iItemNum;
        }
    }

    if(iTotalNum < iItemNum)
    {
        return T_ZONE_SYSTEM_PARA_ERR;
    }

    //��ɾ��ʵ��ɾ����Ʒ
    for(int i=0; i<m_iRepBlockNum; ++i)
    {
        if(iItemNum == 0)
        {
            return T_SERVER_SUCESS;
        }

        if(m_astRepItem[i].iItemID == iItemID)
        {
            if(iItemNum > m_astRepItem[i].iItemNum)
            {
                DeleteRepItem(i, m_astRepItem[i].iItemNum, iItemChannel);

                iItemNum -= m_astRepItem[i].iItemNum;
            }
            else
            {
                DeleteRepItem(i, iItemNum, iItemChannel);

                return T_SERVER_SUCESS;
            }
        }
    }

    return T_SERVER_SUCESS;
}

//����������Ʒ,�������ͬID����ϲ� 2->1
int CRepThingsManager::ExchangeRepItem(int iItemSlot1, int iItemSlot2, bool bIsSortExchange)
{
    if(iItemSlot1<0 || iItemSlot1>=m_iRepBlockNum
       || iItemSlot2<0 || iItemSlot2>=m_iRepBlockNum)
    {
        LOGERROR("Failed to exchange rep item, invalid slot %d:%d, uin %u\n", iItemSlot1, iItemSlot2, m_uiUin);
        return T_ZONE_SYSTEM_PARA_ERR;
    }

    if(iItemSlot1 == iItemSlot2)
    {
        return T_SERVER_SUCESS;
    }

    RepItem& stItem1 = m_astRepItem[iItemSlot1];
    RepItem& stItem2 = m_astRepItem[iItemSlot2];

    if((stItem1.iItemID==stItem2.iItemID) && CRepThingsUtility::GetItemType(stItem1.iItemID)==REP_ITEM_TYPE_PROP)
    {
        /*
        const SPropItemConfig* pstPropConfig = PropItemCfgMgr().GetConfig(stItem1.iItemID);
        if(!pstPropConfig)
        {
            return T_ZONE_SYSTEM_INVALID_CFG;
        }

        //�������������ͬID�Ŀɵ�����Ʒ�����Ժϲ�
        int iNewItemNum = stItem1.iItemNum + stItem2.iItemNum;
        if(iNewItemNum > pstPropConfig->iMaxNum)
        {
            stItem1.iItemNum = pstPropConfig->iMaxNum;
            stItem2.iItemNum = iNewItemNum - stItem1.iItemNum;
        }
        else
        {
            stItem1.iItemNum = iNewItemNum;
            stItem2.iItemNum = 0;
        }

        //�ϲ���ɺ󶼱�ɰ�״̬
        stItem1.bIsBinded = true;

        //���Slot2��ĿΪ0�������
        if(stItem2.iItemNum == 0)
        {
            stItem2.Reset();
        }
        */
    }
    else
    {
        //����2����Ʒ�۵���Ʒ
        RepItem stTmpItem;
        
        memcpy((void*)&stTmpItem, (void*)&stItem1, sizeof(stItem1));
        memcpy((void*)&stItem1, (void*)&stItem2, sizeof(stItem2));
        memcpy((void*)&stItem2, (void*)&stTmpItem, sizeof(stTmpItem));
    }

    if(!bIsSortExchange)
    {
        //���ͱ����仯��֪ͨ
        std::vector<int> vItemSlot;
        vItemSlot.push_back(iItemSlot1);
        vItemSlot.push_back(iItemSlot2);
    
        SendRepChangeNotify(vItemSlot);
    }

    return T_SERVER_SUCESS;
}

//��ȡ��������Ʒ������
int CRepThingsManager::GetRepItemNum(int iItemSlot)
{
    if(iItemSlot < 0 || iItemSlot >= m_iRepBlockNum)
    {
        return 0;
    }

    return m_astRepItem[iItemSlot].iItemNum;
}

int CRepThingsManager::GetRepItemNumByID(int iItemID)
{
    int iItemNum = 0;

    for(int i=0; i<m_iRepBlockNum; ++i)
    {
        if(m_astRepItem[i].iItemID == iItemID)
        {
            iItemNum += m_astRepItem[i].iItemNum;
        }
    }

    return iItemNum;
}

//���ӱ�������Ʒ������Ϊ����ʾɾ��
int CRepThingsManager::AddItemNumByID(int iItemID, int iItemNum, int iItemChannel)
{
    if(iItemNum > 0)
    {
        //������Ʒ
        return AddRepItem(iItemID, iItemNum, iItemChannel);
    }
    else if(iItemNum < 0)
    {
        iItemNum = -iItemNum;

        //ɾ����Ʒ
        std::vector<int> vItemSlot;

        //��ɾ���ǰ󶨵�,�Ӻ���ǰɾ
        for(int i=m_iRepBlockNum-1; i>=0; --i)
        {
            if(iItemNum == 0)
            {
                break;
            }

            if(m_astRepItem[i].iItemID == iItemID)
            {
                if(m_astRepItem[i].iItemNum <= iItemNum)
                {
                    iItemNum -= m_astRepItem[i].iItemNum;
                    m_astRepItem[i].Reset();
                }
                else
                {
                    m_astRepItem[i].iItemNum -= iItemNum;
                    iItemNum = 0;
                }

                vItemSlot.push_back(i);
            }
        }

        //��ɾ���󶨵ģ��Ӻ���ǰɾ
        for(int i=m_iRepBlockNum-1; i>=0; --i)
        {
            if(iItemNum == 0)
            {
                break;
            }

            if(m_astRepItem[i].iItemID == iItemID)
            {
                if(m_astRepItem[i].iItemNum <= iItemNum)
                {
                    iItemNum -= m_astRepItem[i].iItemNum;
                    m_astRepItem[i].Reset();
                }
                else
                {
                    m_astRepItem[i].iItemNum -= iItemNum;
                    iItemNum = 0;
                }

                vItemSlot.push_back(i);
            }
        }

        if(iItemNum > 0)
        {
            return T_ZONE_SYSTEM_PARA_ERR;
        }

        //���ͱ����仯��֪ͨ
        SendRepChangeNotify(vItemSlot, iItemChannel);
    }

    return T_SERVER_SUCESS;
}

//��ȡ��������Ʒ��ID
int CRepThingsManager::GetRepItemID(int iItemSlot)
{
    if(iItemSlot < 0 || iItemSlot >= m_iRepBlockNum)
    {
        return 0;
    }

    return m_astRepItem[iItemSlot].iItemID;
}

//��ȡ�����ո��ӵ�SlotIndex
int CRepThingsManager::GetEmptyItemSlotIndex()
{
    for(int i=0; i<m_iRepBlockNum; ++i)
    {
        if(m_astRepItem[i].iItemID == 0)
        {
            return i;
        }
    }

    return -1;
}

//����������Ʒ
int CRepThingsManager::SortRepItem()
{
    //int iNewNum = 0;

    //todo jasonxiong4 ������������
    /*
    //�ȴ���ͬ���͵���Ʒ�ϲ�,Ŀǰֻ�е���
    for(int i=0; i<m_iRepBlockNum; ++i)
    {
        int iItemType = CRepThingsUtility::GetItemType(m_astRepItem[i].iItemID);
        if(iItemType != REP_ITEM_TYPE_PROP)
        {
            //���ǵ���
            continue;
        }

        const SPropItemConfig* pstPropConfig = PropItemCfgMgr().GetConfig(m_astRepItem[i].iItemID);
        if(!pstPropConfig || m_astRepItem[i].iItemNum>=pstPropConfig->iMaxNum)
        {
            continue;
        }

        for(int j=i+1; j<m_iRepBlockNum; ++j)
        {
            if(m_astRepItem[i].iItemID!=m_astRepItem[j].iItemID)
            {
                continue;
            }
            
            iNewNum = m_astRepItem[i].iItemNum + m_astRepItem[j].iItemNum;
            if(iNewNum > pstPropConfig->iMaxNum)
            {
                m_astRepItem[i].iItemNum = pstPropConfig->iMaxNum;
            }
            else
            {
                m_astRepItem[i].iItemNum = iNewNum;
            }

            m_astRepItem[j].iItemNum = iNewNum - m_astRepItem[i].iItemNum;
            if(m_astRepItem[j].iItemNum == 0)
            {
                m_astRepItem[j].Reset();
            }
        }
    }
    */

    //�������Ϊ װ����������ߡ���ͨ����
    int iCurrentIndex = 0;
    
    int iRet = T_SERVER_SUCESS;

    //������װ���Ĵ���
    int iEquipIndex = iCurrentIndex;
    for(int i=iCurrentIndex; i<m_iRepBlockNum; ++i)
    {
        int iItemType = CRepThingsUtility::GetItemType(m_astRepItem[i].iItemID);
        if(iItemType == REP_ITEM_TYPE_EQUIP)
        {
            if(i != iCurrentIndex)
            {
                iRet = ExchangeRepItem(iCurrentIndex, i, true);
                if(iRet)
                {
                    LOGERROR("Failed to exchange rep item, slot1:slot2 %d:%d, uin %u\n", iCurrentIndex, i, m_uiUin);
                    return iRet;
                }
            }

            ++iCurrentIndex;
        }
    }

    //����װ����С��
    SortEquipBySubType(iEquipIndex, iCurrentIndex, EQUIP_SLOT_TYPE_SHIZHUANG);
    SortEquipBySubType(iEquipIndex, iCurrentIndex, EQUIP_SLOT_TYPE_WUQI);
    SortEquipBySubType(iEquipIndex, iCurrentIndex, EQUIP_SLOT_TYPE_SHANGZHUANG);
    SortEquipBySubType(iEquipIndex, iCurrentIndex, EQUIP_SLOT_TYPE_XIAZHUANG);
    SortEquipBySubType(iEquipIndex, iCurrentIndex, EQUIP_SLOT_TYPE_XIANGLIAN);
    SortEquipBySubType(iEquipIndex, iCurrentIndex, EQUIP_SLOT_TYPE_JIEZHI);
    SortEquipBySubType(iEquipIndex, iCurrentIndex, EQUIP_SLOT_TYPE_YAODAI);

    //������ߵĴ���
    int iPropIndex = iCurrentIndex;
    for(int i=iPropIndex; i<m_iRepBlockNum; ++i)
    {
        int iItemType = CRepThingsUtility::GetItemType(m_astRepItem[i].iItemID);
        if(iItemType==REP_ITEM_TYPE_PROP)
        {
            if(i != iPropIndex)
            {
                iRet = ExchangeRepItem(iCurrentIndex, i, true);
                if(iRet)
                {
                    LOGERROR("Failed to exchange rep item, slot1:slot2 %d:%d, uin %u\n", iCurrentIndex, i, m_uiUin);
                    return iRet;
                }
            }

            ++iCurrentIndex;
        }
    }

    //������ߵ�С��
    SortPropItemBySubType(iPropIndex, iCurrentIndex, 4);    //�������
    SortPropItemBySubType(iPropIndex, iCurrentIndex, 1);    //ҩƷ����
    SortPropItemBySubType(iPropIndex, iCurrentIndex, 2);    //���ܵ���
    SortPropItemBySubType(iPropIndex, iCurrentIndex, 3);    //���ϵ���

    return T_SERVER_SUCESS;
}

//������������
int CRepThingsManager::OpenRepBlock()
{
    if(m_iRepBlockNum >= MAX_REP_BLOCK_NUM)
    {
        LOGERROR("Failed to open rep block, block num %d, max %d, uin %u\n", m_iRepBlockNum, MAX_REP_BLOCK_NUM, m_uiUin);
        return T_ZONE_SYSTEM_PARA_ERR;
    }

    //todo jasonxiong4 ������������
    /*
    m_iRepBlockNum += REP_ONE_OPEN_BLOCK_NUM;
    if(m_iRepBlockNum > MAX_REP_BLOCK_NUM)
    {
        m_iRepBlockNum = MAX_REP_BLOCK_NUM;
    }
    */

    return T_SERVER_SUCESS;
}

//�������еı�������
int CRepThingsManager::OpenAllRepBlock()
{
    if(m_iRepBlockNum >= MAX_REP_BLOCK_NUM)
    {
        LOGERROR("Failed to open rep block, block num %d, max %d, uin %u\n", m_iRepBlockNum, MAX_REP_BLOCK_NUM, m_uiUin);
        return T_ZONE_SYSTEM_PARA_ERR;    
    }

    m_iRepBlockNum = MAX_REP_BLOCK_NUM;

    return T_SERVER_SUCESS;
}

//������Ʒ���ݿ��������
void CRepThingsManager::UpdateRepItemToDB(ITEMDBINFO& rstItemInfo)
{
    rstItemInfo.set_m_iopenedslotnum(m_iRepBlockNum);

    ItemSlotInfo* pstBlockInfo = rstItemInfo.mutable_stitemslot();
    for(int i=0; i<m_iRepBlockNum; ++i)
    {
        OneSlotInfo* pstOneBlockInfo = pstBlockInfo->add_stslots();

        if(m_astRepItem[i].iItemID == 0)
        {
            //�յ���Ʒ��
            continue;
        }

        pstOneBlockInfo->set_iitemid(m_astRepItem[i].iItemID);
        pstOneBlockInfo->set_iitemnum(m_astRepItem[i].iItemNum);

        //��װ���ߺ�װ������ϸ��Ϣ
        int iItemType = CRepThingsUtility::GetItemType(m_astRepItem[i].iItemID);
        switch (iItemType)
        {
        case REP_ITEM_TYPE_EQUIP:
            {
                ;
            }
            break;

        case REP_ITEM_TYPE_PROP:
            {
                ;
            }
            break;

        default:
            {
                ;
            }
            break;
        }
    }

    return;
}

void CRepThingsManager::InitRepItemFromDB(const ITEMDBINFO& stItemInfo)
{
    m_iRepBlockNum = stItemInfo.m_iopenedslotnum();

    const ItemSlotInfo& stBlockInfo = stItemInfo.stitemslot();
    for(int i=0; i<stBlockInfo.stslots_size(); ++i)
    {
        m_astRepItem[i].iItemID = stBlockInfo.stslots(i).iitemid();
        m_astRepItem[i].iItemNum = stBlockInfo.stslots(i).iitemnum();

        int iItemType = CRepThingsUtility::GetItemType(m_astRepItem[i].iItemID);
        switch (iItemType)
        {
        case REP_ITEM_TYPE_EQUIP:
            {
                ;
            }
            break;

        case REP_ITEM_TYPE_PROP:
            {
                ;
            }
            break;

        default:
            {
                ;
            }
            break;
        }
    }

    return;
}

int CRepThingsManager::GetEmptyItemSlot()
{
    for(int i=0; i<m_iRepBlockNum; ++i)
    {
        if(m_astRepItem[i].iItemID == 0)
        {
            return i;
        }
    }

    return -1;
}

//��װ�������ͽ�������,��һ�������������ʼ��λ��
int CRepThingsManager::SortEquipBySubType(int& iBeginIndex, int iMaxIndex, int iSubType)
{
    //todo jasonxiong5 ��������
    /*
    int iRet = T_SERVER_SUCESS;

    int iCurrentIndex = iBeginIndex;
    for(int i=iBeginIndex; i<iMaxIndex; ++i)
    {
        const SEquipmentConfig* pstEquipConfig = EquipmentCfgMgr().GetConfig(m_astRepItem[i].iItemID);
        if(!pstEquipConfig)
        {
            LOGERROR("Failed to get equipment config, invalid id %d\n", m_astRepItem[i].iItemID);
            return T_ZONE_SYSTEM_INVALID_CFG;
        }

        if(pstEquipConfig->iEquipType==iSubType)
        {
            if(i != iCurrentIndex)
            {
                iRet = ExchangeRepItem(iCurrentIndex, i, true);
                if(iRet)
                {
                    LOGERROR("Failed to exchange rep item, slot1:slot2 %d:%d, uin %u\n", iCurrentIndex, i, m_uiUin);
                    return iRet;
                }
            }

            ++iCurrentIndex;
        }
    }

    iBeginIndex = iCurrentIndex; 
    */ 

    return T_SERVER_SUCESS;
}

//�����������ͽ������򣬵�һ�������������ʼ��λ��
int CRepThingsManager::SortPropItemBySubType(int& iBeginIndex, int iMaxIndex, int iSubType)
{
    //int iRet = T_SERVER_SUCESS;

    int iCurrentIndex = iBeginIndex;
    for(int i=iBeginIndex; i<iMaxIndex; ++i)
    {
        //todo jasonxiong4 
        /*
        const SPropItemConfig* pstPropConfig = PropItemCfgMgr().GetConfig(m_astRepItem[i].iItemID);
        if(!pstPropConfig)
        {
            LOGERROR("Failed to get prop config, invalid id %d\n", m_astRepItem[i].iItemID);
            return T_ZONE_SYSTEM_INVALID_CFG;
        }

        if(pstPropConfig->iPropType==iSubType)
        {
            if(i != iCurrentIndex)
            {
                iRet = ExchangeRepItem(iCurrentIndex, i, true);
                if(iRet)
                {
                    LOGERROR("Failed to exchange rep item, slot1:slot2 %d:%d, uin %u\n", iCurrentIndex, i, m_uiUin);
                    return iRet;
                }
            }

            ++iCurrentIndex;
        }
        */
    }

    iBeginIndex = iCurrentIndex;

    return T_SERVER_SUCESS;
}

//���ͱ�����Ʒ�仯��֪ͨ
void CRepThingsManager::SendRepChangeNotify(std::vector<int>& vItemSlot, int iItemChannel)
{
    CZoneMsgHelper::GenerateMsgHead(m_stRepChangeNotify, MSGID_ZONE_REPCHANGE_NOTIFY);

    Zone_RepChange_Notify* pstNotify = m_stRepChangeNotify.mutable_m_stmsgbody()->mutable_m_stzone_repchange_notify();

    for(unsigned i=0; i<vItemSlot.size(); ++i)
    {
        if(vItemSlot[i] == -1)
        {
            continue;
        }

        OneRepSlot* pstRepSlot = pstNotify->add_stitems();
        pstRepSlot->set_irepslot(vItemSlot[i]);
        pstRepSlot->set_ichangechannel(iItemChannel);
        
        //��Ʒ����ϸ��Ϣ
        OneSlotInfo* pstSlotInfo = pstRepSlot->mutable_stslotinfo();
        RepItem& stItemInfo = m_astRepItem[vItemSlot[i]];

        pstSlotInfo->set_iitemid(stItemInfo.iItemID);
        pstSlotInfo->set_iitemnum(stItemInfo.iItemNum);

        int iType = CRepThingsUtility::GetItemType(vItemSlot[i]);
        switch(iType)
        {
        case REP_ITEM_TYPE_EQUIP:
            {
                ;
            }
            break;

        case REP_ITEM_TYPE_PROP:
            {
                ;
            }
            break;

        default:
            {
                continue;
            }
            break;
        }
    }

    //������Ϣ���ͻ���
    CZoneMsgHelper::SendZoneMsgToRole(m_stRepChangeNotify, GetOwner());

    return;
}
