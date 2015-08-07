

#include "GameProtocol.hpp"
#include "LogAdapter.hpp"
#include "ZoneErrorNumDef.hpp"
#include "ModuleShortcut.hpp"

#include "UnitBufferObj.hpp"

using namespace ServerLib;

IMPLEMENT_DYN(CUnitBufferObj)

CUnitBufferObj::CUnitBufferObj()
{
    Initialize();
}

CUnitBufferObj::~CUnitBufferObj()
{

}

int CUnitBufferObj::Initialize()
{
    //������Buff��ID
    m_iBuffID = 0;

    //Buffʣ��Ļغ���
    m_iRemainRounds = 0;

    //Buff������ʵ��Ч��
    m_iRealEffect = 0;

    //ʩ�Ӹ�Buff��ս����λID
    m_iCastUnitID = -1;

    return T_SERVER_SUCESS;
}

//��ʼ��һ��Buff
int CUnitBufferObj::InitUnitBuff(int iBuffID, int iCastUnitID, int iRemainRounds)
{
    m_iBuffID = iBuffID;
    m_iCastUnitID = iCastUnitID;
    m_iRemainRounds = iRemainRounds;

    m_iRealEffect = 0;

    return T_SERVER_SUCESS;
}
    
//BuffID
int CUnitBufferObj::GetUnitBuffID()
{
    return m_iBuffID;
}

void CUnitBufferObj::SetUnitBuffID(int iBuffID)
{
    m_iBuffID = iBuffID;
}

//Buffʣ��غ���
int CUnitBufferObj::GetRemainRound()
{
    return m_iRemainRounds;
}

void CUnitBufferObj::SetRemainRound(int iRemainRound)
{
    m_iRemainRounds = iRemainRound;
}

//Buff������ʵ��Ч��
int CUnitBufferObj::GetBuffEffect()
{
    return m_iRealEffect;
}

void CUnitBufferObj::SetBuffEffect(int iRealEffect)
{
    m_iRealEffect = iRealEffect;
}

//��Ӹ�Buff��ս����λID
int CUnitBufferObj::GetCastUnitID()
{
    return m_iCastUnitID;
}

void CUnitBufferObj::GetCastUnitID(int iCastUnitID)
{
    m_iCastUnitID = iCastUnitID;
}

    


