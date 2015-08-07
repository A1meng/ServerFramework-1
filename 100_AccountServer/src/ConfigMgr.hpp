#ifndef __CONFIG_MGR_H__
#define __CONFIG_MGR_H__

//#include "ConfigDef.hpp"
//#include "WhiteListConfig.hpp"
#include "UinPrivManager.hpp"

class CConfigMgr
{
private:
    //TZoneConfList m_stZoneConfList;
    CUinPrivManager m_stUinPrivManager;
    static int m_iWhiteListEnabled;
    static int m_iCreateRoleEnabled;

public:
    int LoadAllConf();

    TUinPrivInfo* GetUinPrivInfo(unsigned int uiUin);

public:

    //��������
    static int IsWhiteListEnabled();

    //�Ƿ���������ɫ
    static bool IsCreateRoleEnabled();
};

#endif // __CONFIG_MGR_H__
