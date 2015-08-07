#include <arpa/inet.h>

#include "GameProtocol.hpp"
#include "LogAdapter.hpp"
#include "SessionManager.hpp"

int CSessionManager::Initialize(bool bResume)
{
    if (bResume)
    {
        return 0;
    }

    for (int i = 0; i < (int)MAX_ROLE_OBJECT_NUMBER_IN_WORLD; i++)
    {
        m_astSessionList[0][i].SetActive(false);
        m_astSessionList[1][i].SetActive(false);
    }

    m_unSeed = 0;
    m_uiActiveNumber = 0;
    return 0;
}

CSessionObj* CSessionManager::CreateSession(TNetHead_V2& stNetHead)
{
    unsigned int uiSessionFD = ntohl(stNetHead.m_uiSocketFD);
    // TRACESVR("session fd: %u\n", uiSessionFD);

    CSessionObj* pSessionObj = GetSessionByFd(uiSessionFD);
    if (pSessionObj == NULL)
    {
        return NULL;
    }

    if (pSessionObj->IsActive())
    {
        return pSessionObj;
    }

    pSessionObj->Create(stNetHead, GenerateValue());
    m_uiActiveNumber++;

    return pSessionObj;
}

int CSessionManager::DeleteSession(const unsigned int uiSessionFD)
{
    LOGDETAIL("delete session, sockfd: %u\n", uiSessionFD);

    CSessionObj* pSessionObj = GetSessionByFd(uiSessionFD);
    if (pSessionObj == NULL)
    {
        return 0;
    }

    pSessionObj->SetActive(false);
    m_uiActiveNumber--;

    return 0;
}

unsigned short CSessionManager::GenerateValue()
{
    unsigned short unValue = ++m_unSeed;
    if (0xffff == unValue) // ���������ֵʱ����Ϊ0�����¼���
    {
        m_unSeed = 0;
    }

    return unValue;
}

int CSessionManager::CheckSession(TNetHead_V2& stNetHead)
{
    //// ����fd��session�����Ƿ���ڣ����������ܾ�����
    unsigned int uiSessionFD = ntohl(stNetHead.m_uiSocketFD);

    // ���session�����еĽ������Ƿ�ﵽ���ޣ��ﵽ������ܾ�����
    if (GetSessionByFd(uiSessionFD) == NULL)
    {
        return T_ACCOUNT_SERVER_BUSY;
    }

    return T_SERVER_SUCESS;
}

CSessionObj* CSessionManager::GetSessionByFd(unsigned int uiSessionFD)
{
    if (uiSessionFD >= (unsigned int)MAX_FD_NUMBER * 2)
    {
        return NULL;
    }

    int iCodeQueueNum = uiSessionFD / MAX_FD_NUMBER;
    int iFdIdx = uiSessionFD % MAX_ROLE_OBJECT_NUMBER_IN_WORLD;

    if (iCodeQueueNum >= MAX_CODEQUEUE_NUM)
    {
        return NULL;
    }
    if (iFdIdx >= (int)MAX_ROLE_OBJECT_NUMBER_IN_WORLD)
    {
        return NULL;
    }

    return &(m_astSessionList[iCodeQueueNum][iFdIdx]);

}

CSessionObj* CSessionManager::GetSession(const unsigned int uiSessionFD, const unsigned short unValue)
{
    CSessionObj* pSessionObj = GetSessionByFd(uiSessionFD);
    if (pSessionObj->IsActive())
    {
        return pSessionObj; // Ϊͬһ��session
    }

    return NULL; // session�����ڻ���valueֵ��ͬ
}

void CSessionManager::ClearCache(time_t& stTime)
{
    return;
}
