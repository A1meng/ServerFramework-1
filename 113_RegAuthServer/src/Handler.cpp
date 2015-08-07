#include <string.h>
#include <arpa/inet.h>

#include "Handler.hpp"
#include "RegAuthSingleton.hpp"

int  IHandler::EncodeAndSendCode(IProtocolEngine* pProtocolEngine,
                         TNetHead_V2* pstNetHead, GameProtocolMsg* pstMsg ,EGameServerID enMsgPeer)
{
//  ASSERT_AND_LOG_RTN_INT(pProtocolEngine);
//  ASSERT_AND_LOG_RTN_INT(pstNetHead);
//  ASSERT_AND_LOG_RTN_INT(pstMsg);

    unsigned char szCodeBuf[MAX_CODE_LEN];
    int iBufLen = sizeof(szCodeBuf);
    int iCodeLen;

    // ��ʼ��code buffer��code length
    memset(szCodeBuf, 0, sizeof(szCodeBuf));
    iCodeLen = 0;

    int iInstanceID = 0;
    if (GAME_SERVER_LOTUSREGAUTH == enMsgPeer)
    {
        TNetHead_V2 stTmpNetHead = *pstNetHead;
        //socket id ��ת��
        unsigned  int uiSessionID = ntohl(pstNetHead->m_uiSocketFD);
        iInstanceID = uiSessionID / MAX_FD_NUMBER;
        stTmpNetHead.m_uiSocketFD = htonl(uiSessionID % MAX_FD_NUMBER);
        pProtocolEngine->Encode(&stTmpNetHead, pstMsg, (unsigned char*)szCodeBuf, iBufLen, iCodeLen);

    }
    else
    {
        // ���뱾������Ϊ��������
        pProtocolEngine->Encode(pstNetHead, pstMsg, (unsigned char*)szCodeBuf, iBufLen, iCodeLen);
    }

    // ������������
    int iRes = RegAuthMsgTransceiver->SendOneMsg((char*)szCodeBuf, iCodeLen, enMsgPeer, iInstanceID);

    if (iRes < 0)
    {
        return -1;
    }

    return 0;
}

int IHandler::EncodeAndSendCodeBySession(IProtocolEngine* pProtocolEngine,
                      unsigned uiSessionFd, GameProtocolMsg* pstMsg, EGameServerID enMsgPeer)
{
    //���һ�ȡSession
    CSessionObj* pSession = SessionManager->GetSession(uiSessionFd);
    ASSERT_AND_LOG_RTN_INT(pSession);

    //���һ�ȡNetHead
    TNetHead_V2* pstNetHead = pSession->GetNetHead();
    ASSERT_AND_LOG_RTN_INT(pstNetHead);

    return EncodeAndSendCode(pProtocolEngine, pstNetHead, pstMsg, enMsgPeer);
}

void IHandler::GenerateMsgHead(GameProtocolMsg* pstRegAuthMsg,
                               unsigned int uiSessionFd, unsigned int uiMsgID, unsigned int uiAccountHash)
{
    ASSERT_AND_LOG_RTN_VOID(pstRegAuthMsg);

    pstRegAuthMsg->Clear();

    GameCSMsgHead* pstMsgHead = pstRegAuthMsg->mutable_m_stmsghead();
    pstMsgHead->set_m_uisessionfd(uiSessionFd);
    pstMsgHead->set_m_uimsgid((ProtocolMsgID)uiMsgID);
    pstMsgHead->set_m_uin(uiAccountHash);   //NOTE�� ��RegAuth Э��ͷ�У�����uin�ֶ�ΪAccountID��Hashֵ

    return;
}
