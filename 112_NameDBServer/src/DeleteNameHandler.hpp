#ifndef __DELETE_NAME_HANDLER_HPP__
#define __DELETE_NAME_HANDLER_HPP__

#include "DBClientWrapper.hpp"
#include "Handler.hpp"
#include "NameDBPublic.hpp"

class CNameDBHandlerSet;

// MSGID_DELETENAME_REQUEST ��Ϣ������
class CDeleteNameHandler : public CHandler
{
private:
    DBClientWrapper* m_pDatabase;   // �������ݿ��ָ��
    GameProtocolMsg* m_pstRequestMsg; // ���������Ϣ

	int m_iThreadIdx;

    //���ɵ�SQL���
    static char m_szQueryString[GameConfig::NAME_TABLE_SPLIT_FACTOR][256];

public:
	void SetThreadIdx(const int iThreadIdx){m_iThreadIdx = iThreadIdx;}

private:
    // ����Ķ���ֻ����CNameDBHandlerSet���д���
    friend class CNameDBHandlerSet;
    CDeleteNameHandler(DBClientWrapper* pDatabase);

public:
    virtual void OnClientMsg(GameProtocolMsg* pstRequestMsg, SHandleResult* pstHandleResult);

private:

    //ɾ��Name��Ϣ
    int DeleteName(const std::string& strName, int iType);

    void FillFailedResponse(const unsigned int uiResultID, GameProtocolMsg* pstResponseMsg);
    void FillSuccessfulResponse(GameProtocolMsg* pstResponseMsg);
};

#endif
