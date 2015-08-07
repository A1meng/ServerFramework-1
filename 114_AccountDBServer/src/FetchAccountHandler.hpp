#ifndef __FETCH_ACCOUNT_HANDLER_HPP__
#define __FETCH_ACCOUNT_HANDLER_HPP__

#include "DBClientWrapper.hpp"
#include "Handler.hpp"
#include "AccountDBPublic.hpp"

class CAccountDBHandlerSet;

//MSGID_ACCOUNTDB_FETCH_REQUEST ��Ϣ������
class CFetchAccountHandler : public CHandler
{
private:
    DBClientWrapper* m_pDatabase;
    GameProtocolMsg* m_pstRequestMsg; // ���������Ϣ

	int m_iThreadIdx;

    //���ɵ�SQL���
    static char m_szQueryString[GameConfig::ACCOUNT_TABLE_SPLIT_FACTOR][512];

public:
	void SetThreadIdx(const int iThreadIdx){m_iThreadIdx = iThreadIdx;}

private:
    // ����Ķ���ֻ����CAccountDBHandlerSet���д���
    friend class CAccountDBHandlerSet;
    CFetchAccountHandler(DBClientWrapper* pDatabase);

public:
    virtual void OnClientMsg(GameProtocolMsg* pstRequestMsg, SHandleResult* pstHandleResult);

private:

    //��ȡ�����ʺ���ϸ��Ϣ
    int FetchAccountInfo(const AccountID& stAccountID, const std::string& strPassword, AccountDB_FetchAccount_Response& rstResp);

    void FillFailedResponse(const unsigned int uiResultID, GameProtocolMsg& stResponseMsg);
    void FillSuccessfulResponse(GameProtocolMsg& pstResponseMsg);
};

#endif
