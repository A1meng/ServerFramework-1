#ifndef __LOGOUT_HANDLER__HPP__
#define __LOGOUT_HANDLER__HPP__


#include "Handler.hpp"
#include "ZoneErrorNumDef.hpp"

class CGameRoleObj;
class CGameSessionObj;

class CLogoutHandler : public IHandler
{
protected:
	static GameProtocolMsg m_stZoneMsg;
public:
    virtual ~CLogoutHandler();

public:
    virtual int OnClientMsg();

private:
    virtual int SecurityCheck();

public:
    // ����������
    static int LogoutRole(CGameRoleObj* pRoleObj, int iReason);

    // ����������
    // 1. ׼���׶�
    static int LogoutPrepare(CGameRoleObj* pRoleObj);

    // 2. ͬ������

    // 3. ɾ������, �Ͽ�����
    static int LogoutAction(CGameRoleObj* pRoleObj);

    // ֪ͨworld����
    static int NotifyLogoutToWorld(const RoleID& stRoleID);

private:
    int OnRequestLogout();

    static void NotifyRoleLogout(CGameRoleObj* pstRoleObj);

    static int SendFailedResponse(const unsigned int uiResultID, const TNetHead_V2& rstNetHead);

    static int SendSuccessfulResponse(CGameRoleObj* pRoleObj);
};
#endif
