#ifndef __CONFIG_MGR_H__
#define __CONFIG_MGR_H__

//#include "ConfigDef.hpp"
//#include "WhiteListConfig.hpp"
#include "UinPrivManager.hpp"

struct WulinServerConfig
{
    int iServerID;
    char szAccountIP[32];
    char szZoneIP[32];

    WulinServerConfig()
    {
        memset(this, 0, sizeof(*this));
    };
};

class CConfigMgr
{
public:
    int LoadAllConf();

    TUinPrivInfo* GetUinPrivInfo(unsigned int uiUin);

public:

    //��������
    static int IsWhiteListEnabled();

    const WulinServerConfig* GetServerInfo(int iServerID);

private:
    //TZoneConfList m_stZoneConfList;
    CUinPrivManager m_stUinPrivManager;
    static int m_iWhiteListEnabled;

    //��Ϸ�������б�
    int m_iWulinServerNum;
    WulinServerConfig m_astServerList[32];
};

#endif // __CONFIG_MGR_H__
