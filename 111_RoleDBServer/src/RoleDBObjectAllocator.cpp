#include "RoleSeqRecordObj.hpp"
#include "LRUHashCache.hpp"
#include "RoleDBObjectAllocator.hpp"
#include "StringUtility.hpp"
#include "ConfigHelper.hpp"

CServerObjectAllocator::CServerObjectAllocator()
{
    m_iTotalSize = 0;
}

int CServerObjectAllocator::Initialize(bool bResumeMode)
{
    // ������Ҫ����Ķ�����һ�鹲���ڴ���
    size_t iShmSize = CaculateTotalSize();
    int iRet = m_stShm.CreateShmSegmentByKey(GenerateServerShmKey(GAME_SERVER_ROLEDB,1), iShmSize);
    if (iRet < 0)
    {
        return -1;
    }
    
    // �ӹ����ڴ��з�����ֶ��󻺴���
    //todo jasonxiong2 ��߲���Ҫ���仺��
    //CLRUHashCache<CRoleSeqRecordObj>::AllocateFromShm(m_stShm);

    return 0;
}

size_t CServerObjectAllocator::CaculateTotalSize()
{
    if (m_iTotalSize > 0)
    {
        return m_iTotalSize;
    }

    // ��ɫ��ż�¼���󻺴���
    //todo jasonxiong2 ��߲���Ҫ���仺��
    //size_t iRoleSeqRecordCacheSize = CLRUHashCache<CRoleSeqRecordObj>::CaculateSize(
    //                                  MAX_ROLE_SEQ_RECORD_OBJ_NUMBER);
    //m_iTotalSize += iRoleSeqRecordCacheSize;

    return m_iTotalSize;
}
