
#ifndef __BATTLEFIELD_PATH_CONFIG_MANAGER_HPP__
#define __BATTLEFIELD_PATH_CONFIG_MANAGER_HPP__

#include "ConfigHelper.hpp"
#include "ScenePathManager.hpp"

//ս����ͼ���ù�����,����tmx��ʽ�ĵ�ͼ�ļ�

const int MAX_BATTLEFIELD_PATH_CONFIG_NUMBER      = 100;  //���֧��100�ŵ�ͼ�赲��Ϣ

class CBattlefieldPathConfigManager
{
public:
    CBattlefieldPathConfigManager();
    ~CBattlefieldPathConfigManager();

public:

    //����ս������
    int LoadAllBattlefield(); 

    //��ȡս�����赲��Ϣ
    static const CScenePathManager* GetBattlefieldPathConfigManager(int iMapID);

private:

    //���ص����赲��Ϣ����
    int LoadOneBattlefieldPath(const std::string& strConfigFile);

private:

    static int m_iBattlefieldPathNum;
    static CScenePathManager m_astBattlefiledPath[MAX_BATTLEFIELD_PATH_CONFIG_NUMBER];     //�赲������Ϣ
};

#endif
