#ifndef __FETCHROLE_WORLD_HANDLER_HPP__
#define __FETCHROLE_WORLD_HANDLER_HPP__

#include "Handler.hpp"
#include "WorldGlobalDefine.hpp"
#include "WorldObjectHelperW_K64.hpp"
#include "WorldRoleStatus.hpp"

class CFetchRoleWorldHandler : public IHandler
{
public:
	virtual ~CFetchRoleWorldHandler();

public:
	virtual int OnClientMsg(const void* pMsg);

public:
    // ���ؽ�ɫ��ѯ����
    int SendFetchRoleResponse(const RoleID& stRoleID, int iReqID);

    // ����ʧ�ܲ�ѯ
    int SendFailedFetchRoleResponse(const RoleID& stRoleID, int iReqID);

private:
    // ��ѯ���������
	int OnRequestFetchRoleWorld();

    //��ѯ���󷵻ص�World�Ĵ������
    int OnResponseFetchRoleWorld();

private:
	GameProtocolMsg* m_pMsg;

	GameProtocolMsg m_stWorldMsg;
};

#endif


