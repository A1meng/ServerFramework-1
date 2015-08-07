#ifndef __ADD_ACCOUNT_HANDLER_HPP__
#define __ADD_ACCOUNT_HANDLER_HPP__

#include "DBClientWrapper.hpp"
#include "Handler.hpp"
#include "AccountDBPublic.hpp"

//�����ʺ��������Ĵ���

class CAccountDBHandlerSet;

// MSGID_ACCOUNTDB_ADDACCOUNT_REQUEST ��Ϣ������
class CAddAccountHandler : public CHandler
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
    CAddAccountHandler(DBClientWrapper* pDatabase);

public:
    virtual void OnClientMsg(GameProtocolMsg* pstRequestMsg, SHandleResult* pstHandleResult);

private:

    //���б�Ҫ�Ĳ������
    int CheckParams();

    void OnAddAccountRequest(SHandleResult* pstHandleResult);

    //����ʺ��Ƿ����
    int CheckAccountExist(const AccountID& stAccountID, bool& bIsExist);

    //��ȡ��δ��ʹ�õ�UIN
    int GetAvaliableUin(unsigned int& uin);

    //�����µļ�¼
    int AddNewRecord(const AccountID& stAccountID, unsigned int uin, int iWorldID, const std::string& strPassword);

private:
    void FillFailedResponse(const unsigned int uiResultID, GameProtocolMsg* pstResponseMsg);
    void FillSuccessfulResponse(GameProtocolMsg& stResponseMsg);
};

#endif // __ADD_ACCOUNT_HANDLER_HPP__
