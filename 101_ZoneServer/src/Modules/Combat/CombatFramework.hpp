#ifndef __COMBAT_FRAMEWORK_HPP__
#define __COMBAT_FRAMEWORK_HPP__

#include "GameProtocol.hpp"

class CGameRoleObj;
class CBattlefieldObj;
class CCombatFramework
{
public:

    static CCombatFramework* Instance();

    ~CCombatFramework();

    //��ҿ�ʼս��������
    int DoCombat(CGameRoleObj& stRoleObj, const Zone_DoCombat_Request& rstRequest, bool bNeedResp=true);

    //�������ս����λ�ƶ�������
    int DoCombatMove(CGameRoleObj& stRoleObj, const Zone_CombatMove_Request& rstRequest);

    //�������ս����λ�ж�������
    int DoCombatAction(CGameRoleObj& stRoleObj, const Zone_CombatAction_Request& rstRequest);

    //�������
    int DoUpdatePartnerAI(CGameRoleObj& stRoleObj, const Zone_UpdatePartnerAI_Request& rstRequest);

    //����PVPս��������
    int StartPVPCombat(CGameRoleObj& stRoleObj, const Zone_StartPVPCombat_Request& rstRequest);

    //����PVPս��������
    int AcceptPVPCombat(CGameRoleObj& stRoleObj, const Zone_AcceptPVPCombat_Request& rstRequest);

    //����ս�����͵�����
    int SetCombatForm(CGameRoleObj& stRoleObj, const Zone_SetCombatForm_Request& rstRequest);

    //�������ս��
    void FinCombat(CGameRoleObj& stRoleObj);

private:

    CCombatFramework();

    //��ʼ��PVEս��ս����Ϣ
    int InitPveBattlefield(CGameRoleObj& stRoleObj, int iLevelID);

    //��ʼ��PVPս��ս����Ϣ
    int InitPVPBattlefield(CGameRoleObj& stActiveRoleObj, CGameRoleObj& stPassiveRoleObj);

    //����ս�����󣬷���CBattlefieldObj��index
    CBattlefieldObj* CreateBattlefieldObj(int& iBattlefieldObjIndex);

    //����ս����ʼ����Ϣ
    int SendBattlefieldInitNotify(CGameRoleObj& stRoleObj);

    //���ʹ���ɹ��ķ���
    int SendSuccessResponse();

private:

    unsigned int m_uiUin;

    static GameProtocolMsg m_stGameMsg;
};

#endif

