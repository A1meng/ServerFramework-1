
#ifndef __RTT_CALCULATOR_HPP__
#define __RTT_CALCULATOR_HPP__

// ʹ��[Jacobson 1988]�㷨ͳ������RTT

#include "GameConfigDefine.hpp"
#include "GameProtocol.hpp"
#include "TimeValue.hpp"

using namespace ServerLib;

typedef struct tagRoundTripTime
{
    double m_srtt;   // ƽ��RTT
    double m_drtt;   // ����RTT
    double m_rtt;    // ������RTT

    int Initialize()
    {
        m_srtt = 0.0;
        m_drtt = 0.75;
        m_rtt = 0.0;

        return 0;
    }
}TRoundTripTime;

class CRttUtility
{
public:
    // ��32λ��ת��Ϊ64λ��
    static uint64_t Convert32to64(unsigned int high, unsigned int low);

    // ��ȡ��ǰ�ĺ���ʱ��
    static uint64_t GetNowMilliSeconds();

    // ��ȡʱ��ĺ���ֵ
    static uint64_t GetMilliSeconds(CTimeValue& rstTimeValue);

    // ���¿ͻ���Rtt
    static int UpdateRTT(const GameCSMsgHead& rstMsgHead);
};

#endif
