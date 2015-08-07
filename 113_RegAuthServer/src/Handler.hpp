#ifndef __HANDLER_HPP__
#define __HANDLER_HPP__

#include "CodeQueue.hpp"
#include "ProtocolEngine.hpp"
#include "Public.hpp"
#include "ConfigHelper.hpp"
#include "GameProtocol.hpp"

using namespace ServerLib;

// ��Ϣ�����߳�����
class IHandler
{
protected:
    IHandler() {}

public:
    virtual ~IHandler() {}

    // ��Ϣ����ӿڡ�
    // ���˸�LotusServer����֮�⣬������Server��������ҪpstNetHead����
    // �������Ϣ������ת����Ϣ����ʹ��pstHandleResult��������ΪNULL����
    virtual void OnClientMsg(TNetHead_V2* pstNetHead, GameProtocolMsg* pstMsg,
                             SHandleResult* pstResult) = 0;
protected:
    static int EncodeAndSendCode(IProtocolEngine* pProtocolEngine,
                          TNetHead_V2* pstNetHead, GameProtocolMsg* pstMsg, EGameServerID enMsgPeer);

    static int EncodeAndSendCodeBySession(IProtocolEngine* pProtocolEngine,
                          unsigned uiSessionFd, GameProtocolMsg* pstMsg, EGameServerID enMsgPeer);

    static void GenerateMsgHead(GameProtocolMsg* pstRegAuthMsg,
                                unsigned int uiSessionFd, unsigned int uiMsgID, unsigned int uiAccountHash);
};

#endif // __HANDLER_HPP__

