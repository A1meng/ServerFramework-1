
#include "GameProtocol.hpp"
#include "LogAdapter.hpp"
#include "BattlefieldObj.hpp"
#include "CombatUnitObj.hpp"
#include "CombatUtility.hpp"

#include "AStarPathUtility.hpp"

using namespace ServerLib;
using namespace GameConfig;

// Ѱ·��ʱ·��������
static int g_iTempPathNumber;
static const int MAX_TEMP_PATH_NUMBER = 1024;

// ·����
TSceneBlock CAStarPathUtility::m_astSceneBlock[MAX_PATH_NODE];

// A* ��С��
CPathMinHeap CAStarPathUtility::m_stPathMinHeap;

//��ǰʹ�õ�ͼ���
int CAStarPathUtility::m_iMapWidth = 0;

//��ǰʹ�õ�ͼ�߶�
int CAStarPathUtility::m_iMapHeight = 0;

int CAStarPathUtility::m_iUnitSize = 0;

#define BLOCK_X(pBlock) ( ((pBlock) - &m_astSceneBlock[0]) % (m_iMapWidth) )
#define BLOCK_Y(pBlock) ( ((pBlock) - &m_astSceneBlock[0]) / (m_iMapWidth) )

//A* �㷨Ѱ·
bool CAStarPathUtility::FindAStarPath(CBattlefieldObj& stBattlefieldObj, const CScenePathManager& stPathManager, const TUNITPOSITION& stStartPos, 
                                      const TUNITPOSITION& stEndPos, int iSize, std::vector<TUNITPOSITION>& vPath)
{

    m_iMapWidth = stPathManager.GetBattlefieldWidth();
    m_iMapHeight = stPathManager.GetBattlefieldHeight();
    m_iUnitSize = iSize;

    if (stStartPos.iPosX < 0 || stStartPos.iPosX >= m_iMapWidth
        || stStartPos.iPosY < 0 || stStartPos.iPosY >= m_iMapHeight
        || stEndPos.iPosX < 0 || stEndPos.iPosX  >= m_iMapWidth
        || stEndPos.iPosY  < 0 || stEndPos.iPosY  >= m_iMapHeight)
    {
        return false;
    }

    //���ֱ�Ӹ�������������յ������
    TSceneBlock *pstStartBlock = &m_astSceneBlock[stStartPos.iPosY*m_iMapWidth+stStartPos.iPosX];
    TSceneBlock *pstEndBlock = &m_astSceneBlock[stEndPos.iPosY*m_iMapWidth+stEndPos.iPosX];

    m_stPathMinHeap.Initialize();

    //��ʼ��Bock�����еĿ�
    for(int y=0; y<m_iMapHeight; ++y)
    {
        for(int x=0; x<m_iMapWidth; ++x)
        {
            m_astSceneBlock[(y)*m_iMapWidth+x].iSceneBlock = (y)*m_iMapWidth+x;
        }
    }

    // ���δ���ǰ�ڵ����Χ4���ڵ�, ֱ���������ڵ�
    TSceneBlock *pstCenterBlock = pstStartBlock;
    while (1)
    {
        if (m_stPathMinHeap.IsHeapFull())
        {
            break;
        }

        // ����ǰ�ڵ�������б�
        pstCenterBlock->stAStar.bClosed = true;

        // �����յ������Ŀ�
        if (pstCenterBlock->iSceneBlock == pstEndBlock->iSceneBlock)
        {
            break;
        }

        int iCenterX = BLOCK_X(pstCenterBlock);
        int iCenterY = BLOCK_Y(pstCenterBlock);

        if (iCenterX - 1 >= 0)
        {
            int iRet = AStarCountNode(stBattlefieldObj, stPathManager, iCenterX-1, iCenterY, stEndPos.iPosX, stEndPos.iPosY, pstCenterBlock);
            if (iRet < 0)
            {
                break;
            }
        }

        if (iCenterX + 1 < m_iMapWidth)
        {
            int iRet = AStarCountNode(stBattlefieldObj, stPathManager, iCenterX+1, iCenterY, stEndPos.iPosX, stEndPos.iPosY, pstCenterBlock);
            if (iRet < 0)
            {
                break;
            }
        }

        if (iCenterY - 1 >= 0)
        {
            int iRet = AStarCountNode(stBattlefieldObj, stPathManager, iCenterX, iCenterY-1, stEndPos.iPosX, stEndPos.iPosY, pstCenterBlock);
            if (iRet < 0)
            {
                break;
            }
        }

        if (iCenterY + 1 < m_iMapHeight)
        {
            int iRet = AStarCountNode(stBattlefieldObj, stPathManager, iCenterX, iCenterY+1, stEndPos.iPosX, stEndPos.iPosY, pstCenterBlock);
            if (iRet < 0)
            {
                break;
            }
        }

        // ȡ�����б���·����С����Ϊ��ǰ�ڵ�
        pstCenterBlock = m_stPathMinHeap.PopHeap();
        if (pstCenterBlock == NULL)
        {
            pstStartBlock->stAStar.bClosed = false;
            break;
        }
    }

    pstStartBlock->stAStar.bClosed = false;

    // A* �㷨����, ����·���Ż�

    // ȥ���յ��
    if (pstCenterBlock == pstEndBlock)
    {
        pstCenterBlock = pstCenterBlock->stAStar.pstCenterNode;
    }

    // ͳ��·�������
    g_iTempPathNumber = 0;
    TSceneBlock *pstPathBlock = pstCenterBlock;
    while (pstPathBlock && pstPathBlock != pstStartBlock)
    {
        g_iTempPathNumber++;
        pstPathBlock = pstPathBlock->stAStar.pstCenterNode;
    }

    if (g_iTempPathNumber >= MAX_TEMP_PATH_NUMBER - 1)
    {
        return false;
    }

    //��װ���յ�·��
    vPath.clear();

    // ��·���㷴��������
    pstPathBlock = pstCenterBlock;
    for (int i = g_iTempPathNumber - 1; i >= 0; i--)
    {
        vPath.insert(vPath.begin(), TUNITPOSITION(BLOCK_X(pstPathBlock),BLOCK_Y(pstPathBlock)));

        pstPathBlock = pstPathBlock->stAStar.pstCenterNode;
    }

    // ����յ�
    vPath.push_back(stEndPos);

    return true;
}

// A* ����ڵ�Ȩֵ
int CAStarPathUtility::AStarCountNode(CBattlefieldObj& stBattlefieldObj, const CScenePathManager& stPathManager, int iX, int iY, int iEndX, int iEndY, TSceneBlock *pstCenterBlock)
{
    TSceneBlock *pstNeighborBlock = &m_astSceneBlock[(iY) * m_iMapWidth + iX];

    //��ȡ������Ҫ�ж��赲�ĵ�
    std::vector<TUNITPOSITION> vCheckPos;
    for(int x=0; x<=m_iUnitSize; ++x)
    {
        for(int y=0; y<=m_iUnitSize; ++y)
        {
            vCheckPos.push_back(TUNITPOSITION(iX+x, iY+y));
        }
    }

    //���ж��赲���������赲
    for(unsigned i=0; i<vCheckPos.size(); ++i)
    {
        // �˿鲻�����ߣ�����
        if(!stPathManager.BattlefieldPosCanWalk(vCheckPos[i]))
        {
            return 0;
        }
    }

    //�������Ŀ��㣬���жϼ��λ���Ƿ�����
    if(iX!=iEndX || iY!=iEndY)
    {
        for(unsigned i=0; i<vCheckPos.size(); ++i)
        {
            CCombatUnitObj* pstCombatUnitObj = stBattlefieldObj.GetCombatUnitByPos(vCheckPos[i]);
            if(pstCombatUnitObj && pstCombatUnitObj->GetCombatUnitID()!=stBattlefieldObj.GetActionUnitID())
            {
                return 0;
            }
        }
    }

    TAStarNode &stAStar = pstNeighborBlock->stAStar;

    // �Ѿ��������б�����
    if (stAStar.bClosed)
    {
        return 0;
    }

    //ʡ��G������������ٶ�, ���ò�������·��, �������������·��Ѱ·������
    int iValueG = pstCenterBlock->stAStar.iValueG + 1;

    // �ڿ����б��У���·�����������ȣ�����
    if (stAStar.bOpened && stAStar.iValueG < iValueG)
    {
        return 0;
    }

    // ���µ�ǰ�ڵ�GHFֵ����ָ���µ����Ľڵ�
    stAStar.iValueG = iValueG;
    // stAStar.iValueH = ABS(iX, iEndX) + ABS(iY, iEndY);
    stAStar.iValueF = stAStar.iValueG + ABS(iX, iEndX) + ABS(iY, iEndY);

    stAStar.pstCenterNode = pstCenterBlock;

    // ���뿪���б�
    if (!stAStar.bOpened)
    {
        bool bPushed = m_stPathMinHeap.PushHeap(pstNeighborBlock);
        if (!bPushed)
        {
            return -1;
        }

        stAStar.bOpened = true;
    }

    return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
CPathMinHeap::CPathMinHeap()
{
    m_iOpenNodes = 0;
    m_iCloseNodes = 0;

    memset(m_astOpenNode, 0, sizeof(m_astOpenNode));
    memset(m_astCloseNode, 0, sizeof(m_astCloseNode));
}

// �ж��Ƿ񻺳�����
bool CPathMinHeap::IsHeapFull()
{
    return (m_iCloseNodes >= MAX_PATH_NODE) || (m_iOpenNodes >= MAX_PATH_NODE);
}

void CPathMinHeap::Initialize()
{
    for (int i = 0; i < m_iOpenNodes; i++)
    {
        memset(&(m_astOpenNode[i]->stAStar), 0, sizeof(TAStarNode));
    }

    for (int i = 0; i < m_iCloseNodes; i++)
    {
        memset(&(m_astCloseNode[i]->stAStar), 0, sizeof(TAStarNode));
    }

    m_iOpenNodes = 0;
    m_iCloseNodes = 0;
}

TSceneBlock *CPathMinHeap::PopHeap()
{
    if (m_iOpenNodes <= 0)
    {
        return NULL;
    }

    // ������Сֵ
    TSceneBlock *pstMinBlock = m_astOpenNode[0];

    // �Ƚ������ӽڵ㣬��С������Ϊ���ڵ�
    int iParent = 0;
    int iLeftChild, iRightChild;
    for (iLeftChild = 2 * iParent + 1, iRightChild = iLeftChild + 1;
        iRightChild < m_iOpenNodes;
        iLeftChild = 2 * iParent + 1, iRightChild = iLeftChild + 1)
    {
        if (m_astOpenNode[iLeftChild]->stAStar.iValueF < m_astOpenNode[iRightChild]->stAStar.iValueF)
        {
            m_astOpenNode[iParent] = m_astOpenNode[iLeftChild];
            iParent = iLeftChild;
        }
        else
        {
            m_astOpenNode[iParent] = m_astOpenNode[iRightChild];
            iParent = iRightChild;
        }
    }

    // �����һ���ڵ����ڿճ����Ľڵ���, ��ֹ����ն�
    if (iParent != m_iOpenNodes - 1)
    {
        bool bPushed = InsertHeap(m_astOpenNode[--m_iOpenNodes], iParent);
        if (!bPushed)
        {
            return NULL;
        }
    }

    m_iOpenNodes--;

    if (m_iCloseNodes < MAX_PATH_NODE)
    {
        m_astCloseNode[m_iCloseNodes++] = pstMinBlock;
    }
    else
    {
        return NULL;
    }

    return pstMinBlock;
}

bool CPathMinHeap::PushHeap(TSceneBlock *pstSceneBlock)
{
    if (m_iOpenNodes >= MAX_PATH_NODE)
    {
        return false;
    }

    return InsertHeap(pstSceneBlock, m_iOpenNodes);
}

bool CPathMinHeap::InsertHeap(TSceneBlock *pstSceneBlock, int iPosition)
{
    if (iPosition >= MAX_PATH_NODE)
    {
        return false;
    }

    m_astOpenNode[iPosition] = pstSceneBlock;

    // ���κ͸��ڵ�Ƚϣ�����ȸ��ڵ�С��������
    int iChild, iParent;
    for (iChild = iPosition, iParent = (iChild - 1) / 2;
        iChild > 0;
        iChild = iParent, iParent = (iChild - 1) / 2)
    {
        if (m_astOpenNode[iChild]->stAStar.iValueF < m_astOpenNode[iParent]->stAStar.iValueF)
        {
            TSceneBlock *tmp = m_astOpenNode[iParent];
            m_astOpenNode[iParent] = m_astOpenNode[iChild];
            m_astOpenNode[iChild] = tmp;
        }
        else
        {
            break;
        }
    }

    m_iOpenNodes++;

    return true;
}
