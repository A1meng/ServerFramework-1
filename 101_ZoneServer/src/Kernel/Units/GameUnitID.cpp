
#include "GameProtocol.hpp"
#include "ZoneObjectHelper.hpp"
#include "GameRole.hpp"

#include "GameUnitID.hpp"

IMPLEMENT_DYN(CGameUnitID)


int CGameUnitID::Initialize()
{
    m_iBindObjectIdx    = -1;
    m_ucBindObjectType  = -1;

    m_pstUnitInfo  = NULL;

    m_stListNode.Initialize();
    m_stListNode2.Initialize();

    return 0;
}

int CGameUnitID::Resume()
{
    // 0 ����Ч��UnitID
    if (GetObjectID() == 0)
    {
        return 0;
    }

    BindUnitInfo();

    return 0;
}

// �󶨶���Idx
void CGameUnitID::BindObject(int iObjectIdx, unsigned char ucObjectType)
{
    m_iBindObjectIdx = iObjectIdx;
    m_ucBindObjectType = ucObjectType;

    // ���ö�������
    BindUnitInfo();
}

// ��ȡ�󶨶��������
TUNITINFO* CGameUnitID::GetBindUnitInfo()
{
    return m_pstUnitInfo;
}

// ��ȡ�󶨶����Idx
int CGameUnitID::GetBindObjectIdx()
{
    return m_iBindObjectIdx;
}

// ��ȡ�󶨶��������
int CGameUnitID::GetBindObjectType()
{
    return m_ucBindObjectType;
}

void CGameUnitID::BindUnitInfo()
{
    switch (m_ucBindObjectType)
    {
    case EUT_ROLE:
        {
            CGameRoleObj* pRoleObj = GameTypeK32<CGameRoleObj>::GetByIdx(m_iBindObjectIdx);
            ASSERT_AND_LOG_RTN_VOID(pRoleObj);

            m_pstUnitInfo = &(pRoleObj->GetRoleInfo().stUnitInfo);
            break;
        }

    default:
        {
            ASSERT_AND_LOG_RTN_VOID(0);
        }
    }

    ASSERT_AND_LOG_RTN_VOID(m_pstUnitInfo->iUnitID == GetObjectID());
    ASSERT_AND_LOG_RTN_VOID(m_pstUnitInfo->ucUnitType == m_ucBindObjectType);

	TRACESVR("BindUnit: ID = %d, Idx = %d, Type = %d\n", 
		m_pstUnitInfo->iUnitID, m_iBindObjectIdx, m_pstUnitInfo->ucUnitType);
}

// ���úͻ�ȡͬһ�����ڵĵ�λ����
TObjIdxListNode& CGameUnitID::GetListNode()
{
    return m_stListNode;
}

// ���úͻ�ȡͬһ��������ͬ���͵�λ������
TObjIdxListNode& CGameUnitID::GetListNode2()
{
    return m_stListNode2;
}

TObjIdxListNode* GetGameUnitIDListNode(int iIdx)
{
    CGameUnitID* pGameUnitID = GameType<CGameUnitID>::Get(iIdx);
    if (!pGameUnitID)
    {
        return NULL;
    }

    return &(pGameUnitID->GetListNode());
}

TObjIdxListNode* GetGameUnitIDListNode2(int iIdx)
{
    CGameUnitID* pGameUnitID = GameType<CGameUnitID>::Get(iIdx);
    if (!pGameUnitID)
    {
        return NULL;
    }

    return &(pGameUnitID->GetListNode2());
}
