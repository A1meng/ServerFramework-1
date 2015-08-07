#ifndef __KYLIN_OBJECT_ALLOCATOR_HPP__
#define __KYLIN_OBJECT_ALLOCATOR_HPP__

#include "SharedMemory.hpp"
#include "SingletonTemplate.hpp"

using namespace ServerLib;

class CAccountObjectAllocator
{
private:
    CSharedMemory m_stShm;
    size_t m_iTotalSize; // ��application�и������͵Ķ��󻺴����Ĵ�С֮��

private:
    // ����Ķ���ֻ����CSingleton<CAccountObjectAllocator>���д���
    friend class CSingleton<CAccountObjectAllocator>;
    CAccountObjectAllocator();

public:
    int Initialize(bool bResume);

private:

    // ����cacheʱ��Ҫ��������������������Ӧ�޸�
    size_t CaculateTotalSize();
};

#endif // __KYLIN_OBJECT_ALLOCATOR_HPP__
