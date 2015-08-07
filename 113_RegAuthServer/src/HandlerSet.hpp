#ifndef __HANDLER_SET_HPP__
#define __HANDLER_SET_HPP__

#include <string.h>

#include "RegAuthPublic.hpp"
#include "Handler.hpp"

typedef enum tagenRegAuthMsgType
{
    EKMT_CLIENT = 1,    // �ͻ�����Ϣ
    EKMT_SERVER = 2,    // ��������Ϣ
}ERegAuthMsgType;

typedef struct tagRegAuthMsgHandler
{
    IHandler* m_pHandler;       // ��Ϣ������
    ERegAuthMsgType m_enMsgType;   // ��Ϣ����
}TRegAuthMsgHandler;

// ��Ϣ�����߹�����
class CHandlerSet
{
protected:
    // �������ʾ����Ϣ�����߼���
    TRegAuthMsgHandler m_apHandler[MAX_REGAUTH_HANDLER_NUMBER];

protected:
    CHandlerSet();

public:
    virtual ~CHandlerSet() {}

public:
    // ��ʼ����Ϣ�����߼����еĸ�����Ϣ�����ߣ�����ֵΪ0��ʾ�ɹ���������ʾʧ��
    virtual int Initialize();

    // ������Ϣid���ظ���Ϣ�Ĵ�����
    IHandler* GetHandler(const unsigned int uiMsgID, ERegAuthMsgType enMsgType = EKMT_SERVER);

protected:
    // ������Ϣidע�����Ĵ����ߣ�����ֵΪ0��ʾ�ɹ���������ʾʧ��
    int RegisterHandler(const unsigned int uiMsgID, IHandler* pHandler, ERegAuthMsgType enMsgType = EKMT_SERVER);
};

#endif // __HANDLER_SET_HPP__

