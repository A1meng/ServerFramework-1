#ifndef __COMBAT_HANDLER_HPP__
#define __COMBAT_HANDLER_HPP__

#include "SessionDefine.hpp"
#include "Handler.hpp"
#include "RepThingsManager.hpp"

class CGameRoleObj;

class CCombatHandler : public IHandler
{
protected:
	 static GameProtocolMsg ms_stZoneMsg;
public:
	virtual ~CCombatHandler();

public:
	virtual int OnClientMsg();

private:

    //����ʼս��������
    int OnRequestBeginCombat();

    //���������ƶ�������
    int OnRequestMovePosition();

    //���������ж�������
    int OnRequestCombatAction();

    //���»��AI������
    int OnRequestUpdatePartnerAI();

    //����PVPս��������
    int OnRequestStartPVPCombat();

    //����PVPս��������
    int OnRequestAcceptPVPCombat();
    
    //����ս�����͵�����
    int OnRequestSetCombatForm();

private:

    // ����ʧ�ܻظ�
    int SendFailedResponse(unsigned uiMsgID, const unsigned int uiResultID, const TNetHead_V2& rstNetHead);
};

#endif
