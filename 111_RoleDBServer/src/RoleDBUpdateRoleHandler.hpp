#ifndef __ROLE_DB_UPDATE_ROLE_HANDLER_HPP__
#define __ROLE_DB_UPDATE_ROLE_HANDLER_HPP__

#include "DBClientWrapper.hpp"
#include "Handler.hpp"
#include "RoleDBPublic.hpp"

class CRoleDBHandlerSet;

// MSGID_ROLEDB_UPDATEROLE_REQUEST��Ϣ������
class CRoleDBUpdateRoleHandler : public CHandler
{
private:
    DBClientWrapper* m_pDatabase;   // �������ݿ��ָ��
    GameProtocolMsg* m_pstRequestMsg; // ���������Ϣ

	//�����߳�idx
	int m_iThreadIdx;

    //SQL����ַ���������
    static char m_szQueryString[GameConfig::ROLE_TABLE_SPLIT_FACTOR][51200];

public:
	void SetThreadIdx(const int iThreadIdx){m_iThreadIdx = iThreadIdx;}

private:
    // ����Ķ���ֻ����CRoleDBHandlerSet���д���
    friend class CRoleDBHandlerSet;
    CRoleDBUpdateRoleHandler(DBClientWrapper* pDatabase);

public:
    virtual void OnClientMsg(GameProtocolMsg* pstRequestMsg, SHandleResult* pstHandleResult);

private:
    void OnUpdateRoleRequest(SHandleResult* pstHandleResult);

private:
    int UpdateRole(const World_UpdateRole_Request* pstUpdateRoleRequest);
    int GenerateQueryString(const World_UpdateRole_Request& rstUpdateReq, char* pszBuff, int iBuffLen, int& iLength);
    void FillFailedResponse(const unsigned int uiResultID, GameProtocolMsg* pstResponseMsg);
    void FillSuccessfulResponse(GameProtocolMsg* pstResponseMsg);
};

#endif // __ROLE_DB_UPDATE_ROLE_HANDLER_HPP__
