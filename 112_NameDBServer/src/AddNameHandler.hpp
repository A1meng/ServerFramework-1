#ifndef __ADD_NAME_HANDLER_HPP__
#define __ADD_NAME_HANDLER_HPP__

#include "DBClientWrapper.hpp"
#include "Handler.hpp"
#include "NameDBPublic.hpp"

//����������ֵ�����

class CNameDBHandlerSet;

// MSGID_ADDNEWNAME_REQUEST ��Ϣ������
class CAddNameHandler : public CHandler
{
private:
    DBClientWrapper* m_pDatabase;   // �������ݿ��ָ��
    GameProtocolMsg* m_pstRequestMsg; // ���������Ϣ

	//�����߳�idx
	int m_iThreadIdx;

    //���ɵ�SQL���
    static char m_szQueryString[GameConfig::NAME_TABLE_SPLIT_FACTOR][1024];

public:
	void SetThreadIdx(const int iThreadIdx){m_iThreadIdx = iThreadIdx;}

private:
    // ����Ķ���ֻ����CNameDBHandlerSet���д���
    friend class CNameDBHandlerSet;
    CAddNameHandler(DBClientWrapper* pDatabase);

public:
    virtual void OnClientMsg(GameProtocolMsg* pstRequestMsg, SHandleResult* pstHandleResult);

private:

    //���б�Ҫ�Ĳ������
    int CheckParams();

    void OnAddNameRequest(SHandleResult* pstHandleResult);

    int CheckNameExist(const std::string& strName, int iType, bool& bIsExist);

    //�����µļ�¼
    int AddNewRecord(const std::string& strName, int iNameType, unsigned uNameID);

private:
    void FillFailedResponse(const unsigned int uiResultID, int iNameType, unsigned uNameID, GameProtocolMsg* pstResponseMsg);
    void FillSuccessfulResponse(const std::string& strName, int iNameType, unsigned uNameID, GameProtocolMsg* pstResponseMsg);
};

#endif // __ADD_NAME_HANDLER_HPP__
