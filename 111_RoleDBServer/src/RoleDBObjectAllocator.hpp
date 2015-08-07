#ifndef __GAME_OBJECT_ALLOCATOR_HPP__
#define __GAME_OBJECT_ALLOCATOR_HPP__

#include "SharedMemory.hpp"
#include "SingletonTemplate.hpp"

using namespace ServerLib;

class CServerObjectAllocator
{
public:
    CServerObjectAllocator();
    int Initialize(bool bResumeMode);

private:
    // ����cacheʱ��Ҫ��������������������Ӧ�޸�
    size_t CaculateTotalSize();

private:
    CSharedMemory m_stShm;
    size_t m_iTotalSize; // ��application�и������͵Ķ��󻺴����Ĵ�С֮��

};

#endif // __GAME_OBJECT_ALLOCATOR_HPP__
