#include "SessionObj.hpp"
#include "RegAuthObjectAllocator.hpp"
#include "ConfigHelper.hpp"
#include "LogAdapter.hpp"

CRegAuthObjectAllocator::CRegAuthObjectAllocator()
{
    m_iTotalSize = 0;
}

int CRegAuthObjectAllocator::Initialize(bool bResume)
{
    int iShmSize = CaculateTotalSize();
    if(iShmSize <= 0)
    {
        return 0;
    }


    // ������Ҫ����Ķ�����һ�鹲���ڴ���
    int iRet = m_stShm.CreateShmSegmentByKey(GenerateServerShmKey(GAME_SERVER_REGAUTH,1), iShmSize);
    if (iRet < 0)
    {
        return -1;
    }

    return 0;
}

size_t CRegAuthObjectAllocator::CaculateTotalSize()
{
    if (m_iTotalSize > 0)
    {
        return m_iTotalSize;
    }

    //todo jasonxiong �������Ż�ʱ�ٿ��Ƿ���Ҫ����Ķ���
    /*
    // ������ɫ��Ϣ����������uin��Ϊhash key
    size_t iRegisterAccountRequestCacheSize =
        CFixedHashCache<CRegisterAccountRequestObj>::CaculateSize(
            MAX_REGISTER_ACCOUNT_REQUEST_NUMBER);
    m_iTotalSize += iRegisterAccountRequestCacheSize;

    // ����ʺ�->uin,passwd��ӳ��Ļ��棬����Ҫÿ����֤�������ݿ��ѯ
    size_t iAccountCacheSize =
        CLRUHashCache<CAccountUinObj>::CaculateSize(MAX_ACCOUNT_OBJ_CACHE_NUMBER);
    m_iTotalSize += iAccountCacheSize;
    */

    //LOGDEBUG("RegisterAccountReuqest cache size %zu, Account cache size %zu!\n", iRegisterAccountRequestCacheSize, iAccountCacheSize);

    return m_iTotalSize;
}
