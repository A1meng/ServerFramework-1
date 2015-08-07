

#include <string>
#include <dirent.h>
#include <fstream>
#include <iostream>

#include "pugixml.hpp"
#include "LogAdapter.hpp"
#include "ErrorNumDef.hpp"
#include "ConfigHelper.hpp"

#include "BattlefieldPathConfigManager.hpp"

using namespace pugi;

CScenePathManager CBattlefieldPathConfigManager::m_astBattlefiledPath[MAX_BATTLEFIELD_PATH_CONFIG_NUMBER];     //�赲������Ϣ

int CBattlefieldPathConfigManager::m_iBattlefieldPathNum = 0;

CBattlefieldPathConfigManager::CBattlefieldPathConfigManager()
{
    m_iBattlefieldPathNum = 0;

    memset(m_astBattlefiledPath, 0, sizeof(m_astBattlefiledPath));
}

CBattlefieldPathConfigManager::~CBattlefieldPathConfigManager()
{

}

//����ս���ر���Ϣ����
int CBattlefieldPathConfigManager::LoadAllBattlefield()
{
    m_iBattlefieldPathNum = 0;

    char szResPath[256] = {0};

    //��ȡ�ر��ļ���·��
    GetCommonResourcePath(szResPath, sizeof(szResPath), "map/");

    //������Ŀ¼�µ����������ļ�
    DIR* pstDir = opendir(szResPath);

    int iRet = T_SERVER_SUCESS;
    struct dirent* pstEntry = NULL;
    while((pstEntry=readdir(pstDir)) != NULL)
    {
        if(!(pstEntry->d_type&DT_REG))
        {
            continue;
        }
        //ֻ������ͨ�ļ�
        std::string strConfigFile(std::string(szResPath)+std::string(pstEntry->d_name));

        if(strConfigFile.find(std::string("tmx"),0) != std::string::npos)
        {
            //TMX��ʽΪxml���赲��Ϣ
            iRet = LoadOneBattlefieldPath(strConfigFile);
            if(iRet)
            {
                LOGERROR("Failed to load battlefield path file %s, ret %d\n", strConfigFile.c_str(), iRet);

                return iRet;
            }
        }
    }

    closedir(pstDir);

    LOGDEBUG("Success to load all battlefield  path files!\n");

    return T_SERVER_SUCESS;
}

//��ȡ��ͼ���赲��Ϣ
const CScenePathManager* CBattlefieldPathConfigManager::GetBattlefieldPathConfigManager(int iMapID)
{
    for(int i=0; i<m_iBattlefieldPathNum; ++i)
    {
        if(m_astBattlefiledPath[i].GetMapID() == iMapID)
        {
            return &(m_astBattlefiledPath[i]);
        }
    }

    return NULL;
}

//���ص����ر���Ϣ����
int CBattlefieldPathConfigManager::LoadOneBattlefieldPath(const std::string& strConfigFile)
{
    if(m_iBattlefieldPathNum >= MAX_BATTLEFIELD_PATH_CONFIG_NUMBER)
    {
        LOGERROR("Failed to load battlefield config, num reach max %d\n", MAX_BATTLEFIELD_PATH_CONFIG_NUMBER);
        return -1;
    }

    static xml_document stXmlDoc;
    xml_parse_result pstResult = stXmlDoc.load_file(strConfigFile.c_str());
    if(!pstResult)
    {
        LOGERROR("Failed to open map file : %s\n", strConfigFile.c_str());
        stXmlDoc.reset();
        return -2;
    }

    //��ȡ��ͼ�ļ����õ�����
    int iMapID = 0;
    xml_node stProperties = stXmlDoc.child("map").child("properties");
    for(xml_node stOneProperty=stProperties.child("property"); stOneProperty; stOneProperty=stOneProperty.next_sibling())
    {
        const char* pstrName = stOneProperty.attribute("name").as_string();
        if(!SAFE_STRCMP(pstrName,"id", 2))
        {
            //�ǵ�ͼID
            iMapID = stOneProperty.attribute("value").as_int();
        }
    }

    //��ȡ�赲�����Ϣ: map->layer->name "map_block"
    xml_node stMap = stXmlDoc.child("map");
    for(xml_node stOneLayer = stMap.child("layer"); stOneLayer; stOneLayer=stOneLayer.next_sibling())
    {
        if(SAFE_STRCMP(stOneLayer.attribute("name").as_string(),"mapblock",sizeof("mapblock")-1))
        {
            //����mapblock��
            continue;
        }

        //�ҵ��赲�㣬��ȡ�赲��Ϣ
        int iMapWidth = stOneLayer.attribute("width").as_int();
        int iMapHeight = stOneLayer.attribute("height").as_int();
        CScenePathManager& stPathManager = m_astBattlefiledPath[m_iBattlefieldPathNum];
        int iRet = stPathManager.Initialize(iMapID, iMapWidth, iMapHeight, stOneLayer);
        if(iRet)
        {
            LOGERROR("Failed to load battlefield path config, file name %s\n", strConfigFile.c_str());
            stXmlDoc.reset();
            return iRet;
        }
    
        ++m_iBattlefieldPathNum;

        break;
    }

    stXmlDoc.reset();

    LOGDEBUG("Success to load one battlefield config %s\n", strConfigFile.c_str());

    return T_SERVER_SUCESS;
}

