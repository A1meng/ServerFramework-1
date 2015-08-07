#ifndef __NAME_DB_HANDLER_SET_HPP__
#define __NAME_DB_HANDLER_SET_HPP__

#include "HandlerSet.hpp"
#include "AddNameHandler.hpp"
#include "DeleteNameHandler.hpp"

// NameDBӦ���е���Ϣ�����߹�����
class CNameDBHandlerSet : public CHandlerSet
{
private:
    // �ü��Ϲ����������Ϣ������
    CAddNameHandler m_stAddNameHandler;
    CDeleteNameHandler m_stDeleteNameHandler;

private:
    int RegisterAllHandlers();
    int OpenNameDB();

private:
    // ��Ϣ�����ߴ�����Ϣʱ��Ҫ�������ݿ�
    DBClientWrapper m_oDBClient;

	int m_iThreadIdx;

public:
    CNameDBHandlerSet();
    ~CNameDBHandlerSet();

    // ��ʼ���ü����е���Ϣ�����ߺ����ݿ����ò��������ݿ�
    int Initialize(const int iThreadIdx);
};

#endif // __NAME_DB_HANDLER_SET_HPP__
