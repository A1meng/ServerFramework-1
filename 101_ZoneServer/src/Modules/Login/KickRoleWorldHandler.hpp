// ���������¼�

#ifndef __KICKROLE_WORLD_HANDLER_HPP__
#define __KICKROLE_WORLD_HANDLER_HPP__

#include "Handler.hpp"
#include "ZoneErrorNumDef.hpp"

class CGameSessionObj;

class CKickRoleWorldHandler : public IHandler
{
protected:
	static GameProtocolMsg ms_stZoneMsg;
public:
    virtual ~CKickRoleWorldHandler();

public:
    virtual int OnClientMsg();

public:
    // ���سɹ��ظ�
    static int SendSuccessfullResponse(const World_KickRole_Request& rstResp);

    // ����ʧ�ܻظ�
    static int SendFailedResponse(const World_KickRole_Request& rstResp);

private:
    // ��������World������֪ͨ, ��������
    int OnRequestKickRole();

    // ��������World�����߻ظ�, ��������
    int OnResponseKickRole();

	//�������Է�ŵ���������
	int OnForbidKickRole();

private:
    // ��World�����������
    int FetchRoleFromWorldServer(const RoleID& stRoleID, char cEnterType);    
};

#endif
