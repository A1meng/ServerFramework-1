#ifndef __PROTOCOL_ENGINE_HPP__
#define __PROTOCOL_ENGINE_HPP__

#include "Public.hpp"

typedef enum
{
    PT_CS = 0, // ��Ҫ����NetHead
    PT_SS,     // ����Ҫ����NetHead
    PT_MAX
} EProtocolType;

// Э�����������
class IProtocolEngine
{
protected:
    EProtocolType m_eType;

public:
    EProtocolType GetProtocolType() const
    {
        return m_eType;
    }

protected:
    IProtocolEngine()
    {
        m_eType = PT_MAX;
    }

public:
    virtual ~IProtocolEngine() {}

public:
    // ��س�ʼ��
    virtual int Initialize() = 0;

    // ��code����ΪTNetHead_V2�ṹ��GameProtocolMsg�ṹ��ע�����û��TNetHead_V2�ṹ������NULL����
    virtual int Decode(unsigned char* pszCodeBuf, const int iCodeLen,
                       TNetHead_V2* pstNetHead, GameProtocolMsg* pstMsg) = 0;

    // ��TNetHead_V2�ṹ��GameProtocolMsg�ṹ����Ϊcode��ע�����û��TNetHead_V2�ṹ������NULL����
    virtual int Encode(TNetHead_V2* pstNetHead, GameProtocolMsg* pstMsg,
                       unsigned char* pszCodeBuf, int iBufLen, int& iCodeLen) = 0;
};

#endif // __PROTOCOL_ENGINE_HPP__

