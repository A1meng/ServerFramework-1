#ifndef __UPDATE_ACCOUNT_HANDLER_HPP__
#define __UPDATE_ACCOUNT_HANDLER_HPP__

#include "DBClientWrapper.hpp"
#include "Handler.hpp"
#include "AccountDBPublic.hpp"

class CAccountDBHandlerSet;

//MSGID_ACCOUNTDB_UPDATE_REQUEST ��Ϣ������
class CUpdateAccountHandler : public CHandler
{
private:
    DBClientWrapper* m_pDatabase;   // �������ݿ��ָ��
    GameProtocolMsg* m_pstRequestMsg; // ���������Ϣ

	//�����߳�idx
	int m_iThreadIdx;

    //���ɵ�SQL���
    static char m_szQueryString[GameConfig::ACCOUNT_TABLE_SPLIT_FACTOR][1024];

public:
	void SetThreadIdx(const int iThreadIdx){m_iThreadIdx = iThreadIdx;}

private:
    // ����Ķ���ֻ����CAccountDBHandlerSet���д���
    friend class CAccountDBHandlerSet;
    CUpdateAccountHandler(DBClientWrapper* pDatabase);

public:
    virtual void OnClientMsg(GameProtocolMsg* pstRequestMsg, SHandleResult* pstHandleResult);

private:
    void OnUpdateAccountRequest(SHandleResult* pstHandleResult);

private:

    //����ʺ��������Ч��
    int UpdateSecurityCheck(const AccountID& stAccountID, const std::string& strPassword);

    //�����ʺ���Ϣ
    int UpdateAccountInfo(const AccountID& stAccountID);

    void FillFailedResponse(const unsigned int uiResultID, GameProtocolMsg* pstResponseMsg);
    void FillSuccessfulResponse(GameProtocolMsg* pstResponseMsg);
};

#endif
