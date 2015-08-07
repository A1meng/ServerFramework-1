
#ifndef __UNIT_BUFFER_OBJ_HPP__
#define __UNIT_BUFFER_OBJ_HPP__

#include "GameProtocol.hpp"
#include "GameConfigDefine.hpp"
#include "ZoneObjectAllocator.hpp"

//ս����λ��Buff����
class CUnitBufferObj : public CObj
{
public:
    CUnitBufferObj();
    virtual ~CUnitBufferObj();
    virtual int Initialize();

    DECLARE_DYN

public:

    //��ʼ��һ��Buff
    int InitUnitBuff(int iBuffID, int iCastUnitID, int iRemainRounds);
    
    //BuffID
    int GetUnitBuffID();
    void SetUnitBuffID(int iBuffID);

    //Buffʣ��غ���
    int GetRemainRound();
    void SetRemainRound(int iRemainRound);

    //Buff������ʵ��Ч��
    int GetBuffEffect();
    void SetBuffEffect(int iRealEffect);

    //��Ӹ�Buff��ս����λID
    int GetCastUnitID();
    void GetCastUnitID(int iCastUnitID);

///////////////////////////////////////////////////////////////////////
private:
    
    //������Buff��ID
    int m_iBuffID;

    //Buffʣ��Ļغ���
    int m_iRemainRounds;

    //Buff������ʵ��Ч��
    int m_iRealEffect;

    //ʩ�Ӹ�Buff��ս����λID
    int m_iCastUnitID;
};

#endif
