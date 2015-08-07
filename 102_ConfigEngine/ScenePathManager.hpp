#ifndef __SCENE_PATH_MANAGER_HPP__
#define __SCENE_PATH_MANAGER_HPP__

//ս��·������������Ҫ������ͼ�Ĵ�С��������赲����Ϣ

#include <bitset>

#include "pugixml.hpp"

#include "GameConfigDefine.hpp"

//������֧�ֵ������ӿ����Ŀ
const unsigned int MAX_MAP_BLOCK_WIDTH_NUM = 64;

//������֧�ֵ������Ӹ߶���Ŀ
const unsigned int MAX_MAP_BLOCK_HEIGHT_NUM = 18;

class CScenePathManager
{
public:
    CScenePathManager();

    ~CScenePathManager();

public:

    //��ʼ��ս����ͼ�赲��Ϣ,��XML��ʽ���ļ�
    int Initialize(int iMapID, int iBlockWidthNum, int iBlockHeightNum, pugi::xml_node& stBlockLayer);

    //��ȡս���Ŀ��
    int GetBattlefieldWidth() const;

    //��ȡս���ĸ߶�
    int GetBattlefieldHeight() const;

    //��ȡ��ͼ��ŵ�ID
    int GetMapID();

public:

    //�Ƿ�·��������������
    bool CanContinueWalk(const TUNITPOSITION& stStartPos, const TUNITPATH& stPath) const;

    //ս��������֮���Ƿ������������
    bool CanContinueWalk(const TUNITPOSITION& stStartPos, const TUNITPOSITION& stEndPos) const;

    //ս���ϵ�ĳ���Ƿ��������
    bool BattlefieldPosCanWalk(const TUNITPOSITION& stPos) const;

private:

    //��ͼ�赲��Ϣ�ı��ID
    int m_iMapID;

    //��ͼ�ĸ��ӿ����Ŀ
    int m_iMapBlockWidthNum;

    //��ͼ�ĸ��Ӹ߶���Ŀ
    int m_iMapBlockHeigthNum;

    //�������赲��Ϣ
    std::bitset<MAX_MAP_BLOCK_WIDTH_NUM * MAX_MAP_BLOCK_HEIGHT_NUM> m_aBlockInfo;
};

#endif
