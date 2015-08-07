#ifndef __REP_THINGS_UTILITY_HPP__
#define __REP_THINGS_UTILITY_HPP__

//��ұ�����Ʒ����������

#include "GameConfigDefine.hpp"

class CGameRoleObj;
class CRepThingsUtility
{
public:
   
    //��ȡ��Ʒ�Ĵ�����
    static int GetItemType(int iItemID);

    //��ȡ��Ʒ����
    static int GetItemNumByID(CGameRoleObj& rstRoleObj, int iItemID);

    //��ɾ��Ʒ�Ľӿ�,���iAddNumС��0��ʾɾ��
    static int AddItemNumByID(CGameRoleObj& stRoleObj, int iItemID, int iAddNum, int iItemChannel);

    //������Ʒ���������Ϊ0��ɾ�����и�ID����Ʒ
    static void SetItemNumByID(CGameRoleObj& rstRoleObj, int iItemID, int iItemNum, int iItemChannel);
};

#endif
