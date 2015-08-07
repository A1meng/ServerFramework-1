#ifndef __HANDLER_HPP__
#define __HANDLER_HPP__

#include "Public.hpp"

// ��Ϣ�����߳�����
class CHandler
{
protected:
    CHandler() {};

public:
    virtual ~CHandler() {};

    // ��Ϣ����
    virtual void OnClientMsg(GameProtocolMsg* pstRequestMsg, SHandleResult* pstHandleResult) = 0;

protected:
    virtual void GenerateResponseMsgHead(GameProtocolMsg* pstServerMsg, 
                                         unsigned int uiSessionID, unsigned int uiMsgID, unsigned int uiUin);
};

#endif // __HANDLER_HPP__

