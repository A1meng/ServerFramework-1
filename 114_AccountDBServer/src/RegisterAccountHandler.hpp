#ifndef __REGISTER_ACCOUNT_HANDLER_HPP__
#define __REGISTER_ACCOUNT_HANDLER_HPP__

#include "DBClientWrapper.hpp"
#include "Handler.hpp"
#include "AccountDBPublic.hpp"

class CAccountDBHandlerSet;

// MSGID_ACCOUNTDB_CREATEROLE_REQUEST��Ϣ������
class CRegisterAccountHandler : public CHandler
{
private:
    DBClientWrapper* m_pDatabase;
    GameProtocolMsg* m_pstRequestMsg; // ���������Ϣ

	int m_iThreadIdx;
public:
	void SetThreadIdx(const int iThreadIdx){m_iThreadIdx = iThreadIdx;}

private:
    // ����Ķ���ֻ����CAccountDBHandlerSet���д���
    friend class CAccountDBHandlerSet;
    CRegisterAccountHandler(DBClientWrapper* pDatabase);

public:
    virtual void OnClientMsg(GameProtocolMsg* pstRequestMsg, SHandleResult* pstHandleResult);

private:

    //�ȼ����ͬ���ʺ��Ƿ��Ѿ�����
    int CheckAccountIsExist(bool& bIsAccountExist);

    //��ȡ��ǰ���ÿ�ע���Uin
    int GetAccountUin(unsigned& uiUin);

    //�����ݿ��в���һ����ע���ʺŵļ�¼
    int InsertNewAccountRecord(const RegAuth_RegAccount_Request& rstReq, unsigned int uiUin);

    void FillFailedResponse(const unsigned int uiResultID, GameProtocolMsg* pstResponseMsg);
    void FillSuccessfulResponse(unsigned int uiUin, GameProtocolMsg* pstResponseMsg);
};

#endif // __ROLE_DB_CREATE_ROLE_HANDLER_HPP__
