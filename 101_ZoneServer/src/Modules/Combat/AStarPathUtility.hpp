#ifndef __ASTAR_PATH_UTILITY_HPP__
#define __ASTAR_PATH_UTILITY_HPP__

#include <vector>

#include "GameConfigDefine.hpp"
#include "ScenePathManager.hpp"

// A* �㷨����
struct tagSceneBlock;
#pragma pack(1)
typedef struct tagAStarNode
{
    unsigned short iValueG;
    unsigned short iValueF;   // FGHֵ

	char bClosed:1;  // �Ƿ��ڷ������
	char bOpened:1;  // �Ƿ��ڿ�������

    tagSceneBlock *pstCenterNode; // ���Ľڵ�

}TAStarNode;

// ��ͼ��
typedef struct tagSceneBlock
{
    unsigned short iSceneBlock;      // ��ͼ��ı��

    TAStarNode stAStar;   
}TSceneBlock;

#pragma pack()

// ·������С�ѣ��Ż�A*�㷨�Ŀ����б�

#define MAX_PATH_NODE ((int)(MAX_MAP_BLOCK_WIDTH_NUM * MAX_MAP_BLOCK_HEIGHT_NUM))

class CPathMinHeap
{
public:
    CPathMinHeap();

    void Initialize();

    // ����·����С�ĵ�
    TSceneBlock *PopHeap();

    // ѹ��һ��·����
    bool PushHeap(TSceneBlock *pstSceneBlock);

	// �ж��Ƿ񻺳�����
	bool IsHeapFull();

private:
    bool InsertHeap(TSceneBlock *pstSceneBlock, int iPosition);

private:
    int m_iOpenNodes;
    int m_iCloseNodes;
    TSceneBlock *m_astOpenNode[MAX_PATH_NODE];
    TSceneBlock *m_astCloseNode[MAX_PATH_NODE];
};

//A*Ѱ·������
class CBattlefieldObj;
class CAStarPathUtility
{
public:
    
    //A* �㷨Ѱ·
    static bool FindAStarPath(CBattlefieldObj& stBattlefieldObj, const CScenePathManager& stPathManager, const TUNITPOSITION& stStartPos, 
                              const TUNITPOSITION& stEndPos, int iSize, std::vector<TUNITPOSITION>& vPath);

private:

    // A* ����ڵ�Ȩֵ
    static int AStarCountNode(CBattlefieldObj& stBattlefieldObj, const CScenePathManager& stPathManager, int iX, int iY, int iEndX, int iEndY, TSceneBlock *pstCenterBlock);

private:

    // ·����
    static TSceneBlock m_astSceneBlock[MAX_PATH_NODE];

    // A* ��С��
    static CPathMinHeap m_stPathMinHeap;

    //��ǰʹ�õ�ͼ���
    static int m_iMapWidth;

    //��ǰʹ�õ�ͼ�߶�
    static int m_iMapHeight;

    //Ѱ·��ս����λ�Ĵ�С
    static int m_iUnitSize;
};

#endif
