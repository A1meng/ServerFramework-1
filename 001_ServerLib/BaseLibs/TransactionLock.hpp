// ********************************************************************
// ��    �ߣ� amacroli(idehong@gmail.com)
// �������ڣ� 2011��7��8��
// ��    ���� �������Ե���
// Ӧ    �ã�
// ********************************************************************

#ifndef __TRANSACTIONLOCK_HPP__
#define __TRANSACTIONLOCK_HPP__

template<typename T>
class CTransactionLock
{
public:
    CTransactionLock(void)
    {
        UnLock();
    }
    ~CTransactionLock(void)
    {
        UnLock();
    }

public:
    const T* GetLockInfo()
    {
        return &m_stLockInfo;
    }

    bool IsLock()
    {
        return m_bLock;
    }

    void Lock(const T* pLockInfo)
    {
        m_stLockInfo = *pLockInfo;
        m_bLock = true;
    }

    void UnLock()
    {
        memset(&m_stLockInfo, 0, sizeof(m_stLockInfo));
        m_bLock = false;
    }

private:
    bool m_bLock;
    T m_stLockInfo;
};
#endif // __TRANSACTIONLOCK_HPP__
