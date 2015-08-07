#include <string.h>
#include "RoleSeqRecordObj.hpp"

IMPLEMENT_DYN(CRoleSeqRecordObj)

const unsigned char CRoleSeqRecordObj::OR_MASK[BITS_PER_BYTE] =
{
    0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01
};

const unsigned char CRoleSeqRecordObj::AND_MASK[BITS_PER_BYTE] =
{
    0x7F, 0xBF, 0xDF, 0xEF, 0xF7, 0xFB, 0xFD, 0xFE
};

CRoleSeqRecordObj::CRoleSeqRecordObj()
{
    m_uiUin = 0;
    memset(m_ucaRoleSeqBits, 0, sizeof(m_ucaRoleSeqBits));
}

CRoleSeqRecordObj::~CRoleSeqRecordObj()
{

}

void CRoleSeqRecordObj::SetUin(unsigned int uiUin)
{
    m_uiUin = uiUin;
}

unsigned int CRoleSeqRecordObj::GetUin()
{
    return m_uiUin;
}

int CRoleSeqRecordObj::Initialize()
{
    return 0;
}

// �ڲ�����
unsigned char CRoleSeqRecordObj::GetBitValue(short nSeq)
{
    int iByteIndex = nSeq / BITS_PER_BYTE; // �ҵ�����byte���±�
    int iBitIndex = nSeq % BITS_PER_BYTE;  // �ҵ�byte�е�bit�±�

    unsigned char ucTmp = m_ucaRoleSeqBits[iByteIndex] >> (BITS_PER_BYTE - 1 - iBitIndex);
    ucTmp &= 0x1;

    return ucTmp;
}

void CRoleSeqRecordObj::SetBitValue(short nSeq)
{
    int iByteIndex = nSeq / BITS_PER_BYTE; // �ҵ�����byte���±�
    int iBitIndex = nSeq % BITS_PER_BYTE;  // �ҵ�byte�е�bit�±�

    m_ucaRoleSeqBits[iByteIndex] |= OR_MASK[iBitIndex];
}

void CRoleSeqRecordObj::ResetBitValue(short nSeq)
{
    int iByteIndex = nSeq / BITS_PER_BYTE; // �ҵ�����byte���±�
    int iBitIndex = nSeq % BITS_PER_BYTE;  // �ҵ�byte�е�bit�±�

    m_ucaRoleSeqBits[iByteIndex] &= AND_MASK[iBitIndex];
}

// �����㷨������seq�ķ�Χ��[0,99]��100�������Ҽ��Ѿ����ڵ�����seqΪA���Ǳ������ɵ�seqΪB��
// ���AС��99����B����A+1�����A����99����B����[0,98]��Χ�ڵ���С��δʹ�õ��Ǹ�ֵ��
short CRoleSeqRecordObj::GenerateSeq()
{
    short i = 0;
    short nSeq = -1;

    // ���ж�max seq�Ƿ��Ѿ�ʹ��
    short nMaxSeqIndex = 0;
    unsigned char ucMaxSeqValue = GetBitValue(nMaxSeqIndex);

    if (BS_UNUSED == ucMaxSeqValue) // max seqδʹ��
    {
        // �ҵ���ʹ�õ�����seq
        for (i = nMaxSeqIndex - 1; i >= 0; i--)
        {
            if (BS_USED == GetBitValue(i))
            {
                nSeq = i + 1;
                break;
            }
        }

        // ���[0,99]��δʹ�ã���i����-1
    }
    else // max seq��ʹ��
    {
        // �ҵ�δʹ�õ���С��seq
        for (i = 0; i < nMaxSeqIndex; i++)
        {
            if (BS_UNUSED == GetBitValue(i))
            {
                nSeq = i;
                break;
            }
        }

        // ���δ�ҵ����õ�seq����i����nMaxSeqIndex��nSeq��Ȼ����-1
    }

    // [0,99]��δʹ��
    if (-1 == i)
    {
        nSeq = 0;
    }

    return nSeq;
}
