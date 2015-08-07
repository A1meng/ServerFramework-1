
#include "CommonDef.hpp"
#include "LogAdapter.hpp"
#include "ErrorNumDef.hpp"

#include "ScenePathManager.hpp"

using namespace ServerLib;
using namespace pugi;

CScenePathManager::CScenePathManager()
{
    m_iMapID = 0;

    m_iMapBlockWidthNum = 0;
    m_iMapBlockHeigthNum = 0;
}

CScenePathManager::~CScenePathManager()
{

}

//��ʼ��ս����ͼ�赲��Ϣ,��XML��ʽ���ļ�
int CScenePathManager::Initialize(int iMapID, int iBlockWidthNum, int iBlockHeightNum, pugi::xml_node& stBlockLayer)
{
    m_iMapID = iMapID;

    //���õ�ͼ��Block������С
    m_iMapBlockWidthNum = iBlockWidthNum;
    m_iMapBlockHeigthNum = iBlockHeightNum;

    //��ȡ��ͼ���赲��Ϣ
    //1��ʾ���赲����������
    m_aBlockInfo.reset();
    xml_node stOneBlock = stBlockLayer.child("data").child("tile");
    int iIndex = 0;
    for(; stOneBlock; stOneBlock=stOneBlock.next_sibling())
    {
        if(stOneBlock.attribute("gid").as_int())
        {
            //���赲
            m_aBlockInfo.set(iIndex);
        }

        ++iIndex;
    }

    return T_SERVER_SUCESS;
}

//��ȡ��ͼ��ŵ�ID
int CScenePathManager::GetMapID()
{
    return m_iMapID;
}

//��ȡս���Ŀ��
int CScenePathManager::GetBattlefieldWidth() const
{
    return m_iMapBlockWidthNum;
}

//��ȡս���ĸ߶�
int CScenePathManager::GetBattlefieldHeight() const
{
    return m_iMapBlockHeigthNum;
}

//�Ƿ�·��������������
bool CScenePathManager::CanContinueWalk(const TUNITPOSITION& stStartPos, const TUNITPATH& stPath) const
{
    if(stPath.iPosNum == 0)
    {
        return true;
    }

    //�ȼ����㵽��һ����
    if(!CanContinueWalk(stStartPos,stPath.astPos[0]))
    {
        return false;
    }

    for(int i=0; i<stPath.iPosNum-1; ++i)
    {
        if(!CanContinueWalk(stPath.astPos[i], stPath.astPos[i+1]))
        {
            return false;
        }
    }

    return true;
}

//����֮���Ƿ������������
bool CScenePathManager::CanContinueWalk(const TUNITPOSITION& stStartPos, const TUNITPOSITION& stEndPos) const
{
    //�ȼ�������Ƿ�����
    if(ABS(stStartPos.iPosX,stEndPos.iPosX) == 0)
    {
        if(ABS(stStartPos.iPosY,stEndPos.iPosY) > 1)
        {
            return false;
        }
    }
    else if(ABS(stStartPos.iPosX,stEndPos.iPosX) == 1)
    {
        if(ABS(stStartPos.iPosY,stEndPos.iPosY) != 0)
        {
            return false;
        }
    }
    else
    {
        return false;
    }

    if(!BattlefieldPosCanWalk(stStartPos) || !BattlefieldPosCanWalk(stEndPos))
    {
        return false;
    }
        
    return true;
}

//ս���ϵ�ĳ���Ƿ��������
bool CScenePathManager::BattlefieldPosCanWalk(const TUNITPOSITION& stPos) const
{
    //���λ���Ƿ�Ϸ�
    if(stPos.iPosX < 0 || stPos.iPosX>=m_iMapBlockWidthNum
       || stPos.iPosY < 0 || stPos.iPosY>=m_iMapBlockHeigthNum)
    {
        return false;
    }

    if(m_aBlockInfo.test(stPos.iPosY*m_iMapBlockWidthNum+stPos.iPosX))
    {
        //���赲
        return false;
    }

    return true;
}

