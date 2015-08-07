#ifndef __HANDLER_SET_HPP__
#define __HANDLER_SET_HPP__

#include <string.h>
#include "AccountPublic.hpp"
#include "Handler.hpp"

typedef enum tagenAccountMsgType
{
    EKMT_CLIENT = 1,    // �ͻ�����Ϣ
    EKMT_SERVER = 2,    // ��������Ϣ
}EAccountMsgType;

typedef struct tagAccountMsgHandler
{
    IHandler* m_pHandler;       // ��Ϣ������
    EAccountMsgType m_enMsgType;   // ��Ϣ����
}TAccountMsgHandler;

// ��Ϣ�����߹�����
class CHandlerSet
{
protected:
    // �������ʾ����Ϣ�����߼���
    TAccountMsgHandler m_apHandler[MAX_ACCOUNT_HANDLER_NUMBER];

protected:
    CHandlerSet();

public:
    virtual ~CHandlerSet() {}

public:
    // ��ʼ����Ϣ�����߼����еĸ�����Ϣ�����ߣ�����ֵΪ0��ʾ�ɹ���������ʾʧ��
    virtual int Initialize();

    // ������Ϣid���ظ���Ϣ�Ĵ�����
    IHandler* GetHandler(const unsigned int uiMsgID, EAccountMsgType enMsgType = EKMT_SERVER);

protected:
    // ������Ϣidע�����Ĵ����ߣ�����ֵΪ0��ʾ�ɹ���������ʾʧ��
    int RegisterHandler(const unsigned int uiMsgID, IHandler* pHandler, EAccountMsgType enMsgType = EKMT_SERVER);
};

#endif // __HANDLER_SET_HPP__

