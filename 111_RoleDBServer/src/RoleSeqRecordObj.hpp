#ifndef __ROLE_SEQ_RECORD_OBJ_HPP__
#define __ROLE_SEQ_RECORD_OBJ_HPP__

#include "ObjAllocator.hpp"
#include "RoleDBPublic.hpp"

using namespace ServerLib;

class CRoleSeqRecordObj : public CObj
{
private:
    //static const int MAX_ROLE_NUMBER_PER_USER = 100; // һ���û����Դ����Ľ�ɫ����ŷ�Χ��Ϊ[0,99]
    static const int BITS_PER_BYTE = 8;
    static const unsigned char OR_MASK[BITS_PER_BYTE];
    static const unsigned char AND_MASK[BITS_PER_BYTE];

    typedef enum
    {
        BS_UNUSED = 0,
        BS_USED = 1
    } EBitState;

private:
    unsigned int m_uiUin;
    // ����uin��Ӧ��seq��ʹ���������i��bit��Ӧseq i��Ϊ1��ʾ��seq�Ѿ�ʹ�ã�Ϊ0��ʾδʹ��
    unsigned char m_ucaRoleSeqBits[1 / BITS_PER_BYTE + 1];

public:
    CRoleSeqRecordObj();
    virtual ~CRoleSeqRecordObj();

public:
    virtual int Initialize();
    DECLARE_DYN

private:
    // ��������ucaRoleSeqBits��indexΪnSeq��bit��ֵ����ЧֵΪ0��1,
    // Ϊ0��ʾ���seqû�б�ʹ�ã�Ϊ1��ʾ���seq�Ѿ���ʹ�ã�������ΧΪ[0,99]
    unsigned char GetBitValue(short nSeq);

public:
    void SetUin(unsigned int uiUin);
    unsigned int GetUin();

    void SetBitValue(short nSeq);   // ��������ucaRoleSeqBits��indexΪnSeq��bit��ֵΪ1
    void ResetBitValue(short nSeq); // ��������ucaRoleSeqBits��indexΪnSeq��bit��ֵΪ0
    short GenerateSeq(); // ����ĳ���㷨����һ����[0,99]��Χ�ڵ�δʹ�õ�seq������-1��ʾʧ��
};

#endif // __ROLE_SEQ_RECORD_OBJ_HPP__

