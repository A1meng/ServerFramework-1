#ifndef __CS_PROTOCOL_ENGINE_HPP__
#define __CS_PROTOCOL_ENGINE_HPP__

#include "SingletonTemplate.hpp"
#include "ProtocolEngine.hpp"

using namespace ServerLib;

// LotusServer�������ĺ�̨Server֮���Э�����������TNetHead_V2�ṹ�Ľ���
class C2SProtocolEngine : public IProtocolEngine
{
private:
    // ����Ķ���ֻ����CSingleton<C2SProtocolEngine>���д���
    friend class CSingleton<C2SProtocolEngine>;
    C2SProtocolEngine() {}

public:
    virtual int Initialize();
    virtual int Decode(unsigned char* pszCodeBuf, const int iCodeLen,
                       TNetHead_V2* pstNetHead, GameProtocolMsg* pstMsg);
    virtual int Encode(TNetHead_V2* pstNetHead, GameProtocolMsg* pstMsg,
                       unsigned char* pszCodeBuf, int iBufLen, int& iCodeLen);
};

#endif // __CS_PROTOCOL_ENGINE_HPP__

