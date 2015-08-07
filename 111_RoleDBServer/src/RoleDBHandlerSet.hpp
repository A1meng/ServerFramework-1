#ifndef __ROLE_DB_HANDLER_SET_HPP__
#define __ROLE_DB_HANDLER_SET_HPP__

#include "HandlerSet.hpp"
#include "RoleDBFetchRoleHandler.hpp"
#include "RoleDBUpdateRoleHandler.hpp"
#include "RoleDBCreateRoleHandler.hpp"
#include "RoleDBListRoleHandler.hpp"

// RoleDBӦ���е���Ϣ�����߹�����
class CRoleDBHandlerSet : public CHandlerSet
{
private:
    // �ü��Ϲ����������Ϣ������
    CRoleDBFetchRoleHandler m_FetchRoleHandler;
    CRoleDBUpdateRoleHandler m_UpdateRoleHandler;
    CRoleDBCreateRoleHandler m_CreateRoleHandler;
    CRoleDBListRoleHandler m_ListRoleHandler;

private:
    int RegisterAllHandlers();
    int OpenRoleDB();

private:
    // ��Ϣ�����ߴ�����Ϣʱ��Ҫ�������ݿ�
    DBClientWrapper m_oDBClient;

	int m_iThreadIdx;

public:
    CRoleDBHandlerSet();
    ~CRoleDBHandlerSet();

    // ��ʼ���ü����е���Ϣ�����ߺ����ݿ����ò��������ݿ�
    int Initialize(const int iThreadIdx);
};

#endif // __ROLE_DB_HANDLER_SET_HPP__
