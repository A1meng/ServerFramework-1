#ifndef __FETCH_FRIEND_INFO_HANDLER_HPP__
#define __FETCH_FRIEND_INFO_HANDLER_HPP__

#include "DBClientWrapper.hpp"
#include "Handler.hpp"
#include "RoleDBPublic.hpp"

class CRoleDBHandlerSet;

class CFetchFriendInfoHandler : public CHandler
{
private:
    DBClientWrapper* m_pDatabase;   // �������ݿ��ָ��
    GameProtocolMsg* m_pstRequestMsg; // ���������Ϣ

	int m_iThreadIdx;
public:
	void SetThreadIdx(const int iThreadIdx){m_iThreadIdx = iThreadIdx;}

private:
    // ����Ķ���ֻ����CRoleDBHandlerSet���д���
    friend class CRoleDBHandlerSet;

    //ÿ�������̰߳�һ��mysql�ͻ�������
    CFetchFriendInfoHandler(DBClientWrapper* pDatabase);

public:
    virtual void OnClientMsg(GameProtocolMsg* pstRequestMsg, SHandleResult* pstHandleResult);

private:
    int QueryFriendInfo(unsigned int uin, Zone_GetFriendInfo_Response* pstResponse);
    void FillFailedResponse(const unsigned int uiResultID, GameProtocolMsg* pstResponseMsg);
    void FillSuccessfulResponse(GameProtocolMsg* pstResponseMsg);
};

#endif // __FETCH_FRIEND_INFO_HANDLER_HPP__
