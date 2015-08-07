#include <assert.h>
#include <arpa/inet.h>

#include "GameProtocol.hpp"
#include "LogAdapter.hpp"
#include "SessionObj.hpp"
#include "RegAuthSingleton.hpp"
#include "ClientClosedHandler.hpp"

CClientClosedHandler::CClientClosedHandler()
{
}

void CClientClosedHandler::OnClientMsg(TNetHead_V2* pstNetHead,
                                       GameProtocolMsg* pstMsg, SHandleResult* pstResult)
{
    // ��ʹ��Result
    ASSERT_AND_LOG_RTN_VOID(pstNetHead);
    ASSERT_AND_LOG_RTN_VOID(pstMsg);

    // ���cache�е�session���
    unsigned int uiSessionFD = ntohl(pstNetHead->m_uiSocketFD);
    TRACESVR("Handling ClientClosedRequest from lotus server, sockfd: %u\n", uiSessionFD);
    SessionManager->DeleteSession(uiSessionFD);
}
