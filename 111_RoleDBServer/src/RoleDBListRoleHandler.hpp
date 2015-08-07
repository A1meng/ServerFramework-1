#ifndef __ROLE_DB_LIST_ROLE_HANDLER_HPP__
#define __ROLE_DB_LIST_ROLE_HANDLER_HPP__

#include "DBClientWrapper.hpp"
#include "Handler.hpp"

class CRoleDBHandlerSet;

// LISTROLE_ACCOUNT_REQUEST��Ϣ������
class CRoleDBListRoleHandler : public CHandler
{
private:
    DBClientWrapper* m_pDatabase;   // �������ݿ��ָ��
    GameProtocolMsg* m_pstRequestMsg; // ���������Ϣ

	//�����߳�idx
	int m_iThreadIdx;

    //SQL����ַ���������
    static char m_szQueryString[GameConfig::ROLE_TABLE_SPLIT_FACTOR][256];

public:
	void SetThreadIdx(const int iThreadIdx){m_iThreadIdx = iThreadIdx;}

private:
    // ����Ķ���ֻ����CRoleDBHandlerSet���д���
    friend class CRoleDBHandlerSet;
    CRoleDBListRoleHandler(DBClientWrapper* pDatabase);

public:
    virtual void OnClientMsg(GameProtocolMsg* pstRequestMsg, SHandleResult* pstHandleResult);

private:
    void FillSuccessfulResponse(GameProtocolMsg* pstResponseMsg);
    void FillFailedResponse(const unsigned int uiResultID, GameProtocolMsg* pstResponseMsg);
    int QueryAndParseRole(const unsigned int uiUin, short nWorldID, short nNewWorldID,
                          Account_ListRole_Response* pstListRoleResponse, int& iErrnoNum);

	int AccountListRole(SHandleResult* pstHandleResult);
};

#endif // __ROLE_DB_LIST_ROLE_HANDLER_HPP__
