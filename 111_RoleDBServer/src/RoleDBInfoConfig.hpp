#ifndef __ROLEDB_INFO_CONFIG_HPP__
#define __ROLEDB_INFO_CONFIG_HPP__

//��ȡROLEDB����Ϣ�ļ�������XML���ļ���ʽ

#include "GameConfigDefine.hpp"

struct ONEROLEDBINFO
{
    int iDBMajorVersion;    //ʹ�õ����ݿ�����汾��
    int iDBMinVersion;      //ʹ�õ����ݿ���Ӱ汾��
    char szDBEngine[64];    //ʹ�õ�DB���������
    char szDBHost[64];      //�������ݿ��HOST
    char szUserName[64];    //�������ݿ���û���
    char szUserPasswd[64];  //�������ݿ������
    char szDBName[64];      //���ӵ����ݿ��DB����

    ONEROLEDBINFO()
    {
        memset(this, 0, sizeof(*this));
    };
};

class CRoleDBInfoConfig
{
public:
    CRoleDBInfoConfig();
    ~CRoleDBInfoConfig();

    int LoadRoleDBInfoConfig(const char* pszFilePath);

    const ONEROLEDBINFO* GetOneRoleDBInfoByIndex(int iDBIndex);

public:
    static const int TOTAL_ROLEDB_NUMBER = GameConfig::ROLE_TABLE_SPLIT_FACTOR;

private:
    
    int m_iRoleDBNumber;

    ONEROLEDBINFO m_astRoleDBInfo[TOTAL_ROLEDB_NUMBER];
};

#endif
