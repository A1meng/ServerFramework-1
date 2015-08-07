
#ifndef __COMBAT_TRAP_OBJ_HPP__
#define __COMBAT_TRAP_OBJ_HPP__

#include "GameProtocol.hpp"
#include "GameConfigDefine.hpp"
#include "ZoneObjectAllocator.hpp"

struct TCOMBATTRAPINFO
{
    int iConfigID;          //���ص�����ID
    int iTrapObjID;         //���ض����ObjID

    TCOMBATTRAPINFO()
    {
        memset(this, 0, sizeof(*this));
    };
};

//ս���ϻ��صĶ���TrapObj
class CCombatTrapObj : public CObj
{
public:
    CCombatTrapObj();
    virtual ~CCombatTrapObj();
    virtual int Initialize();

    DECLARE_DYN

public:

    //��ʼ������
    int InitTrap(int iConfigID, int iCamp, const TUNITPOSITION& stPos, int iDirection);

    //��ȡ���ص�ǰ��λ��
    const TUNITPOSITION& GetPosition();

    //�޸Ļ��ص�Ѫ��
    void AddTrapHP(int iAddNum);
    int GetTrapHP();

    //�жϻ����Ƿ�����
    bool IsTrapDead();

    //�жϻ����Ƿ�ɱ�����
    bool IsTrapCanAttacked();

    //��ȡ�û��ص�����
    const GameConfig::STrapConfig* GetConfig();

    //��ȡ���ص�����ID
    int GetTrapConfigID();

    //��ȡ���ص���Ӫ
    int GetTrapCamp();

///////////////////////////////////////////////////////////////////////
private:

    //���ص�����ID
    int m_iConfigID;

    //���ص�����
    const GameConfig::STrapConfig* m_pstConfig;

    //���صĵ�ǰѪ��
    int m_iTrapHp;

    //������������Ӫ
    int m_iCamp;

    //���صĵ�ǰλ��
    TUNITPOSITION m_stPos;

    //���ص�ǰ�ĳ���
    int m_iDirection;
};

#endif

