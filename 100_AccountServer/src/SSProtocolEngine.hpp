#ifndef __SS_PROTOCOL_ENGINE_HPP__
#define __SS_PROTOCOL_ENGINE_HPP__

#include "SingletonTemplate.hpp"
#include "ProtocolEngine.hpp"

using namespace ServerLib;

// ����LotusServer֮��������ĺ�̨Server֮���Э�������������TNetHead_V2�ṹ
class S2SProtocolEngine : public IProtocolEngine
{
private:
    // ����Ķ���ֻ����CSingleton<S2SProtocolEngine>���д���
    friend class CSingleton<S2SProtocolEngine>;
    S2SProtocolEngine() {}

public:
    virtual int Initialize();
    virtual int Decode(unsigned char* pszCodeBuf, const int iCodeLen,
                       TNetHead_V2* pstNetHead, GameProtocolMsg* pstMsg);
    virtual int Encode(TNetHead_V2* pstNetHead, GameProtocolMsg* pstMsg,
                       unsigned char* pszCodeBuf, int iBufLen, int& iCodeLen);
};

#endif // __SS_PROTOCOL_ENGINE_HPP__

