#ifndef __ACCOUNTDB_INFO_CONFIG_HPP__
#define __ACCOUNTDB_INFO_CONFIG_HPP__

//��ȡACCOUNTDB����Ϣ�ļ�������XML���ļ���ʽ

#include "GameConfigDefine.hpp"

struct ONEACCOUNTDBINFO
{
    int iDBMajorVersion;    //ʹ�õ����ݿ�����汾��
    int iDBMinVersion;      //ʹ�õ����ݿ���Ӱ汾��
    char szDBEngine[64];    //ʹ�õ�DB���������
    char szDBHost[64];      //�������ݿ��HOST
    char szUserName[64];    //�������ݿ���û���
    char szUserPasswd[64];  //�������ݿ������
    char szDBName[64];      //���ӵ����ݿ��DB����

    ONEACCOUNTDBINFO()
    {
        memset(this, 0, sizeof(*this));
    };
};

class CAccountDBInfoConfig
{
public:
    CAccountDBInfoConfig();
    ~CAccountDBInfoConfig();

    int LoadAccountDBInfoConfig(const char* pszFilePath);

    const ONEACCOUNTDBINFO* GetOneAccountDBInfoByIndex(int iDBIndex);

public:
    static const int TOTAL_ACCOUNTDB_NUMBER = GameConfig::ACCOUNT_TABLE_SPLIT_FACTOR;

private:
    
    int m_iAccountDBNumber;

    ONEACCOUNTDBINFO m_astAccountDBInfo[TOTAL_ACCOUNTDB_NUMBER];
};

#endif
