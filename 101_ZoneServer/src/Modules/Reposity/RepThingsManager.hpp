#ifndef __REP_THINGS_MANAGER_HPP__
#define __REP_THINGS_MANAGER_HPP__

#include <vector>

#include "GameConfigDefine.hpp"
#include "GameProtocol.hpp"

using namespace GameConfig;

struct RepItem
{
    int iItemID;
    int iItemNum;

    RepItem()
    {
        Reset();
    };

    void Reset()
    {
        iItemID = 0;
        iItemNum = 0;
    };
};

class CGameRoleObj;
class CRepThingsManager
{
public:
    CRepThingsManager();
    ~CRepThingsManager();

public:

    //��ʼ��
    int Initialize();

    void SetOwner(unsigned int uin);
    CGameRoleObj* GetOwner();

    //����������Ʒ
    int AddRepItem(int iItemID, int iItemNum, int iItemChannel = 0);

    //����������Ʒ, iItemSlot��ʾ�ŵ�ָ����λ��, -1��ʾ��ָ��slotλ��
    int AddRepItem(const RepItem& stItem, int iItemSlot = -1, int iItemChannel = 0);

    //����ɾ����Ʒ
    int DeleteRepItem(int iItemSlot, int iItemNum, int iItemChannel = 0);

    //����ɾ����Ʒ
    int DeleteRepItem(int iItemSlot, RepItem& stDeleteItem, int iItemChannel = 0);

    //����ɾ����Ʒ
    int DeleteRepItemByID(int iItemID, int iItemChannel = 0);
    int DeleteRepItemByID(int iItemID, int iItemNum, int iItemChannel);

    //����������Ʒ,�������ͬID����ϲ� 2->1
    int ExchangeRepItem(int iItemSlot1, int iItemSlot2, bool bIsSortExchange=false);

    //��ȡ��������Ʒ������
    int GetRepItemNum(int iItemSlot);

    int GetRepItemNumByID(int iItemID);

    //���ӱ�������Ʒ������Ϊ����ʾɾ��
    int AddItemNumByID(int iItemID, int iItemNum, int iItemChannel = 0);

    //��ȡ��������Ʒ��ID
    int GetRepItemID(int iItemSlot);

    //��ȡ�����ո��ӵ�SlotIndex,С��0 ��ʾ����û�пո���
    int GetEmptyItemSlotIndex();

    //����������Ʒ
    int SortRepItem();

    //������������
    int OpenRepBlock();

    //�������еı�������
    int OpenAllRepBlock();

    //������Ʒ���ݿ��������
    void UpdateRepItemToDB(ITEMDBINFO& rstItemInfo);
    void InitRepItemFromDB(const ITEMDBINFO& stItemInfo);

private:

    //��ȡ�ո��ӵ�Slot������-1��ʾû���ҵ�
    int GetEmptyItemSlot();

    //��װ�������ͽ�������,��һ�������������ʼ��λ��
    int SortEquipBySubType(int& iBeginIndex, int iMaxIndex, int iSubType);

    //�����������ͽ������򣬵�һ�������������ʼ��λ��
    int SortPropItemBySubType(int& iBeginIndex, int iMaxIndex, int iSubType);

    //���ͱ�����Ʒ�仯��֪ͨ
    void SendRepChangeNotify(std::vector<int>& vItemSlot, int iItemChannel = 0);

private:

    //ӵ�иñ����Ľ�ɫ��uin
    unsigned int m_uiUin;

    //��ǰ���������ĸ�����Ŀ
    int m_iRepBlockNum;

    //���������е���Ʒ��Ϣ
    RepItem m_astRepItem[MAX_REP_BLOCK_NUM]; 

    //��ұ�����Ʒ�����仯��֪ͨ
    static GameProtocolMsg m_stRepChangeNotify;
};

#endif
