
#include "GameProtocol.hpp"
#include "LogAdapter.hpp"
#include "ZoneErrorNumDef.hpp"
#include "ModuleShortcut.hpp"

#include "CombatTrapObj.hpp"

using namespace ServerLib;

IMPLEMENT_DYN(CCombatTrapObj)

CCombatTrapObj::CCombatTrapObj()
{
    Initialize();
}

CCombatTrapObj::~CCombatTrapObj()
{

}

int CCombatTrapObj::Initialize()
{
    //���ص�����ID
    m_iConfigID = 0;

    //���ص�����
    m_pstConfig = NULL;

    //���صĵ�ǰѪ��
    m_iTrapHp = 0;

    //��ǰ��������Ӫ
    m_iCamp = FIGHT_CAMP_INVALID;

    //���ص�ǰ��λ��
    m_stPos.iPosX = -1; //X -1��ʾ�ǷǷ���λ��

    //���ص�����
    m_iDirection = COMBAT_DIRECTION_INVALID;

    return T_SERVER_SUCESS;
}

//��ʼ������
int CCombatTrapObj::InitTrap(int iConfigID, int iCamp, const TUNITPOSITION& stPos, int iDirection)
{
    m_iConfigID = iConfigID;

    //��������
    m_pstConfig = TrapCfgMgr().GetConfig(iConfigID);
    if(!m_pstConfig)
    {
        LOGERROR("Failed to get combat trap config, invalid trap id %d\n", m_iConfigID);
        return T_ZONE_SYSTEM_INVALID_CFG;
    }

    m_iCamp = iCamp;

    //���Ѫ�� <0 ���ܱ�����
    m_iTrapHp = m_pstConfig->iTrapHP;

    //����λ��
    m_stPos.iPosX = stPos.iPosX;
    m_stPos.iPosY = stPos.iPosY;

    m_iDirection = iDirection;

    return T_SERVER_SUCESS;
}

//��ȡ���ص�ǰ��λ��
const TUNITPOSITION& CCombatTrapObj::GetPosition()
{
    return m_stPos;
}

//�޸Ļ��ص�Ѫ��
void CCombatTrapObj::AddTrapHP(int iAddNum)
{
    m_iTrapHp += iAddNum;
    if(m_iTrapHp < 0)
    {
        m_iTrapHp = 0;
    }

    return;
}

int CCombatTrapObj::GetTrapHP()
{
    return m_iTrapHp;
}

//�жϻ����Ƿ�����
bool CCombatTrapObj::IsTrapDead()
{
    return (m_iTrapHp == 0);
}

//�жϻ����Ƿ�ɱ�����
bool CCombatTrapObj::IsTrapCanAttacked()
{
    return (m_iTrapHp >= 0);
}

//��ȡ�û��ص�����
const STrapConfig* CCombatTrapObj::GetConfig()
{
    return m_pstConfig;
}

//��ȡ���ص�����ID
int CCombatTrapObj::GetTrapConfigID()
{
    return m_iConfigID;
}

//��ȡ���ص���Ӫ
int CCombatTrapObj::GetTrapCamp()
{
    return m_iCamp;
}

