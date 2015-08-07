
#include "GameProtocol.hpp"
#include "GameRole.hpp"
#include "ZoneErrorNumDef.hpp"
#include "LogAdapter.hpp"
#include "RepThingsManager.hpp"
#include "ModuleShortcut.hpp"

#include "RepThingsUtility.hpp"
   
using namespace GameConfig;   

//��ȡ��Ʒ�Ĵ�����
int CRepThingsUtility::GetItemType(int iItemID)
{
    if(iItemID == 0)
    {
        return REP_ITEM_TYPE_INVALID;
    }

    //IDΪ3000000���µ��ǵ���
    if(iItemID<3000000)
    {
        return REP_ITEM_TYPE_PROP;
    }
    
    return REP_ITEM_TYPE_EQUIP;
}

//��ȡ��Ʒ����
int CRepThingsUtility::GetItemNumByID(CGameRoleObj& rstRoleObj, int iItemID)
{
    //��ȡ������Ʒ������
    CRepThingsManager& rstThingsManager = rstRoleObj.GetRepThingsManager();

    return rstThingsManager.GetRepItemNumByID(iItemID);
}

//��ɾ��Ʒ�Ľӿ�,���iAddNumС��0��ʾɾ��
int CRepThingsUtility::AddItemNumByID(CGameRoleObj& stRoleObj, int iItemID, int iAddNum, int iItemChannel)
{
    //��ȡ������Ʒ������
    CRepThingsManager& rstThingsManager = stRoleObj.GetRepThingsManager();

    return rstThingsManager.AddItemNumByID(iItemID, iAddNum, iItemChannel);
}

//������Ʒ���������Ϊ0��ɾ�����и�ID����Ʒ
void CRepThingsUtility::SetItemNumByID(CGameRoleObj& rstRoleObj, int iItemID, int iItemNum, int iItemChannel)
{
    int iRepItemNum = CRepThingsUtility::GetItemNumByID(rstRoleObj, iItemID);

    CRepThingsUtility::AddItemNumByID(rstRoleObj, iItemID, iItemNum-iRepItemNum, iItemChannel);

    return;
}

