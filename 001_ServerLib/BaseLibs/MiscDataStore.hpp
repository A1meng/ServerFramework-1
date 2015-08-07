#ifndef __MISCDATA_STORE_HPP__
#define __MISCDATA_STORE_HPP__

#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>

#include "CommonDef.hpp"

using namespace ServerLib;

//������ļ��������˼򵥷�װ
//��Ϊÿ�ζ���ȫ��д�ļ���ֻ�ʺ����ݱȽ�С���ļ�,��Լ4kһ��
//4k write һ�δ�Լ��4us
//��Ĵ洢���Ǵ����ݿ��
class CMiscDataStore
{
public:
    CMiscDataStore() {}
    virtual ~CMiscDataStore();

//	static bool IsExsist(const char *szFileName, int iWorldID, int iZoneID);
//	static int *GetFileFP(const char *szFileName, int iWorldID, int iZoneID, FILE *&fp);
//
    // ��ʼ���ļ�, WorldID��ZoneID��Ϊ�˱����ļ��ظ�
    virtual int Init(const char *szFileName, int iWorldID, int iZoneID);

    //д�ļ��� һ�㲻�����أ�
    //����д����ʱ��������WritePrivateData
    virtual int WriteData();
private:
    //���ļ��� ��Initʱ����
    virtual int ReadData() = 0;

    //д�ļ��� д������Ҫ�洢������
    virtual int WritePrivateData() = 0;

private:
    //�õ��ļ���Ӧ����ʵ�ļ�·��
    static int GetRealFilePath(char (&szFilePath)[MAX_FILENAME_LENGTH], const char* pFileName, int iWorldID, int iZoneID);

public:
    //�ļ��Ƿ����
    static bool Exists(const char* pFileName, int iWorldID, int iZoneID);

    //��ȡ�ļ�����, ������ʵ��ȡ���ֽ���
    static int ReadFile(const char* pFileName, int iWorldID, int iZoneID, char* pData, int iDataLen);

    //д�ļ�����, ������ʵд����ֽ���
    static int WriteFile(const char* pFileName, int iWorldID, int iZoneID, const char* pData, int iDataLen);

protected:
    //���ڴ򿪵��ļ�ָ��
    FILE* m_fp;
    //�ļ�����
    char m_szDataStoreFileName[MAX_FILENAME_LENGTH];
};

#endif
