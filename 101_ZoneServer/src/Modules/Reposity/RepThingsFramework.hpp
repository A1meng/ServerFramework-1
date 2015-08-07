#ifndef __REP_THINGS_FRAMEWORK_HPP__
#define __REP_THINGS_FRAMEWORK_HPP__

#include "GameProtocol.hpp"
#include "RepThingsManager.hpp"

class CGameRoleObj;
class CFightUnitObj;

class CRepThingsFramework
{
public:

    static CRepThingsFramework* Instance();

    ~CRepThingsFramework();

    //��ҶԱ�����Ʒ�Ĳ���
    int OperaRepItem(unsigned int uin, const Zone_RepOpera_Request& rstRequest);

    //�����ȡ������Ʒ����ϸ��Ϣ
    int GetRepItemInfo(unsigned int uin, const Zone_GetRepInfo_Request& rstRequest);

    //��Ҵӱ�����װ��
    int WearEquipment(unsigned int uin, const Zone_WearEquip_Request& rstRequest);

    //��ҳ��۱�����Ʒ
    int SellRepItem(unsigned int uin, const Zone_SellItem_Request& rstRequest);

private:
    
    //��ֱ��������е���Ʒ
    int SplitRepItem(CGameRoleObj& stRoleObj, int iSlotIndex, int iItemNum, Zone_RepOpera_Response& rstResponse);

    //�����ͺϲ����������е���Ʒ
    int ExchangeRepItem(CGameRoleObj& stRoleObj, int iSlotIndex, int iOtherSlotIndex, Zone_RepOpera_Response& rstResponse);

    //ʹ�ñ����е���Ʒ
    int UseRepItem(CGameRoleObj& stRoleObj, int iSlotIndex, int iItemNum, int iFightUnitID, Zone_RepOpera_Response& rstResponse);

    //ɾ�������е���Ʒ
    int DeleteRepItem(CGameRoleObj& stRoleObj, int iSlotIndex, int iItemNum, Zone_RepOpera_Response& rstResponse);

    //������Ʒ����
    int SortRepItem(CGameRoleObj& stRoleObj, Zone_RepOpera_Response& rstResponse);

    //���ʹ���ɹ��ķ���
    int SendSuccessResponse();

    CRepThingsFramework();

private:
    unsigned int m_uiUin;

    static GameProtocolMsg m_stGameMsg;
};

#endif

