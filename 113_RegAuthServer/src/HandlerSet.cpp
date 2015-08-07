#include "HandlerSet.hpp"

CHandlerSet::CHandlerSet()
{

}

IHandler* CHandlerSet::GetHandler(const unsigned int uiMsgID, ERegAuthMsgType enMsgType)
{
    if(uiMsgID >= MAX_REGAUTH_HANDLER_NUMBER)
    {
        TRACESVR("NULL handler, msg id: %d\n", uiMsgID);
        return NULL;
    }

    // �ͻ�����Ϣֻ�ܵ��ÿͻ���Handler, ��������Ϣ����ֱ�ӵ��ÿͻ���Handler
    if(enMsgType == m_apHandler[uiMsgID].m_enMsgType || EKMT_SERVER == enMsgType)
    {
        return m_apHandler[uiMsgID].m_pHandler;
    }
    else
    {
        return NULL;
    }
}

int CHandlerSet::RegisterHandler(const unsigned int uiMsgID, IHandler* pHandler, ERegAuthMsgType enMsgType)
{
    ASSERT_AND_LOG_RTN_INT(pHandler);

    if(uiMsgID >= MAX_REGAUTH_HANDLER_NUMBER)
    {
        return -1;
    }

    // ��ֹ�ظ�ע��
    if (m_apHandler[uiMsgID].m_pHandler)
    {
        TRACESVR("Duplicated Handler Registered: uiMsgID = %d, Handler = %p\n", uiMsgID, pHandler);
        abort();
    }

    m_apHandler[uiMsgID].m_pHandler = (IHandler*)pHandler;
    m_apHandler[uiMsgID].m_enMsgType = enMsgType;

    return 0;
}

int CHandlerSet::Initialize()
{
    memset(m_apHandler, 0, sizeof(m_apHandler));
    return 0;
}

