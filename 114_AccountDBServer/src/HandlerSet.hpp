#ifndef __HANDLER_SET_HPP__
#define __HANDLER_SET_HPP__

#include <string.h>
#include "Handler.hpp"

using namespace GameConfig;

// ��Ϣ�����߹�����
class CHandlerSet
{
protected:
    // �������ʾ����Ϣ�����߼���
    CHandler* m_apHandler[MAX_HANDLER_NUMBER];

protected:
    CHandlerSet();

public:
    virtual ~CHandlerSet() {};

public:
    // ��ʼ����Ϣ�����߼����еĸ�����Ϣ�����ߣ�����ֵΪ0��ʾ�ɹ���������ʾʧ��
    virtual int Initialize();

    // ������Ϣid���ظ���Ϣ�Ĵ�����
    CHandler* GetHandler(const unsigned int uiMsgID);

protected:
    // ������Ϣidע�����Ĵ����ߣ�����ֵΪ0��ʾ�ɹ���������ʾʧ��
    int RegisterHandler(const unsigned int uiMsgID, CHandler* pHandler);
};

#endif // __HANDLER_SET_HPP__

