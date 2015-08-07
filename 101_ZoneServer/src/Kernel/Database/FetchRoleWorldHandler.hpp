#ifndef __FETCH_ROLE_WORLD_HANDLER_HPP__
#define __FETCH_ROLE_WORLD_HANDLER_HPP__
#include "Handler.hpp"
#include "ZoneErrorNumDef.hpp"

class CFetchRoleWorldHandler : public IHandler
{
protected:
	static GameProtocolMsg m_stZoneMsg;
public:
    virtual ~CFetchRoleWorldHandler();

public:
    virtual int OnClientMsg();

private:
    int OnFetchRole();

    ////////////////////////////////////////////////////////
    // ��¼��ɫ
    int LoginRole();

    // ��ʼ����ɫ����
    int InitRoleData();
    
    // ��¼���ʼ��
    int InitRoleAfterLogin();
		
private:

    int QueryMailNumber();
    int FetchGameFriend();
};

#endif
