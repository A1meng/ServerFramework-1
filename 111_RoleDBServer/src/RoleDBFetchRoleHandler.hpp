#ifndef __ROLE_DB_FETCH_ROLE_HANDLER_HPP__
#define __ROLE_DB_FETCH_ROLE_HANDLER_HPP__

#include "DBClientWrapper.hpp"
#include "Handler.hpp"
#include "RoleDBPublic.hpp"
#include "SeqConverter.hpp"

class CRoleDBHandlerSet;

// MSGID_ROLEDB_FETCHROLE_REQUEST��Ϣ������
class CRoleDBFetchRoleHandler : public CHandler
{
private:
    DBClientWrapper* m_pDatabase;   // �������ݿ��ָ��
    GameProtocolMsg* m_pstRequestMsg; // ���������Ϣ

	//�����߳�idx
	int m_iThreadIdx;

    //SQL����ַ���������
    static char m_szQueryString[GameConfig::ROLE_TABLE_SPLIT_FACTOR][512];

public:
	void SetThreadIdx(const int iThreadIdx){m_iThreadIdx = iThreadIdx;}

private:
    // ����Ķ���ֻ����CRoleDBHandlerSet���д���
    friend class CRoleDBHandlerSet;
    CRoleDBFetchRoleHandler(DBClientWrapper* pDatabase);

public:
    virtual void OnClientMsg(GameProtocolMsg* pstRequestMsg, SHandleResult* pstHandleResult);

private:
    void OnFetchRoleRequest(SHandleResult* pstHandleResult);

private:
    int QueryRole(const RoleID& stRoleID, GameUserInfo& rstDBUserInfo);
    void FillFailedResponse(const unsigned int uiResultID, GameProtocolMsg* pstResponseMsg);
    void FillSuccessfulResponse(GameProtocolMsg* pstResponseMsg);
};

#endif // __ROLE_DB_FETCH_ROLE_HANDLER_HPP__
