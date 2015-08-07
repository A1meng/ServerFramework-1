#ifndef __NAMEDB_INFO_CONFIG_HPP__
#define __NAMEDB_INFO_CONFIG_HPP__

//��ȡNAMEDB����Ϣ�ļ�������XML���ļ���ʽ

#include "GameConfigDefine.hpp"

struct ONENAMEDBINFO
{
    int iDBMajorVersion;    //ʹ�õ����ݿ�����汾��
    int iDBMinVersion;      //ʹ�õ����ݿ���Ӱ汾��
    char szDBEngine[64];    //ʹ�õ�DB���������
    char szDBHost[64];      //�������ݿ��HOST
    char szUserName[64];    //�������ݿ���û���
    char szUserPasswd[64];  //�������ݿ������
    char szDBName[64];      //���ӵ����ݿ��DB����

    ONENAMEDBINFO()
    {
        memset(this, 0, sizeof(*this));
    };
};

class CNameDBInfoConfig
{
public:
    CNameDBInfoConfig();
    ~CNameDBInfoConfig();

    int LoadNameDBInfoConfig(const char* pszFilePath);

    const ONENAMEDBINFO* GetOneNameDBInfoByIndex(int iDBIndex);

public:
    static const int TOTAL_NAMEDB_NUMBER = GameConfig::NAME_TABLE_SPLIT_FACTOR;

private:
    
    int m_iNameDBNumber;

    ONENAMEDBINFO m_astNameDBInfo[TOTAL_NAMEDB_NUMBER];
};

#endif
