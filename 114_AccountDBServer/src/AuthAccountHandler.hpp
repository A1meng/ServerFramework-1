#ifndef __AUTH_ACCOUNT_HANDLER_HPP__
#define __AUTH_ACCOUNT_HANDLER_HPP__

#include "DBClientWrapper.hpp"
#include "Handler.hpp"
#include "AccountDBPublic.hpp"

//��֤ƽ̨�ʺŵ����������

class CAccountDBHandlerSet;

// MSGID_ACCOUNTDB_FETCHROLE_REQUEST��Ϣ������
class CAuthAccountHandler : public CHandler
{
private:
    DBClientWrapper* m_pDatabase;   // �������ݿ��ָ��
    GameProtocolMsg* m_pstRequestMsg; // ���������Ϣ

	//�����߳�idx
	int m_iThreadIdx;
public:
	void SetThreadIdx(const int iThreadIdx){m_iThreadIdx = iThreadIdx;}

private:
    // ����Ķ���ֻ����CAccountDBHandlerSet���д���
    friend class CAccountDBHandlerSet;
    CAuthAccountHandler(DBClientWrapper* pDatabase);

public:
    virtual void OnClientMsg(GameProtocolMsg* pstRequestMsg, SHandleResult* pstHandleResult);

private:
    void OnAuthAccountRequest(SHandleResult* pstHandleResult);

    //У����֤�˻�������
    int CheckAccountPasswd(const std::string& strAccount, int iLoginType, const std::string& strPasswd, RegAuth_AuthAccount_Response& stResp);

private:
    void FillFailedResponse(const unsigned int uiResultID, GameProtocolMsg* pstResponseMsg);
    void FillSuccessfulResponse(GameProtocolMsg* pstResponseMsg);
};

#endif // __ROLE_DB_FETCH_ROLE_HANDLER_HPP__
