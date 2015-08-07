
#include "GameProtocol.hpp"
#include "ZoneObjectHelper.hpp"
#include "ModuleHelper.hpp"
#include "GameUnitID.hpp"
#include "ModuleHelper.hpp"
#include "GameRole.hpp"
#include "TimeUtility.hpp"

#include "UnitUtility.hpp"

int CUnitUtility::m_iUpdateRoleIdx = -1;

void create_dump(void)
{
//#ifdef _DEBUG_ || _DEBUG || DEBUG_ || DEBUG
    static int dump_times = 0;
    if (dump_times >= 1)
    {
        return;
    }
    ++dump_times;
    int * invalid = NULL;
    if(!fork()) {
            // Crash the app in your favorite way here
        abort();  //�� kill(getpid(), SIGABRT);Ӧ��һ����
        *invalid = 42;    //Ӧ�ò��ᵽ�������˰ɡ�
    }
//#endif
}

TUNITINFO* CUnitUtility::GetUnitInfo(const int iUnitID)
{
    CGameUnitID* pGameUnitID = GameType<CGameUnitID>::Get(iUnitID);
    if (!pGameUnitID)
    {
        return NULL;
    }

    switch(pGameUnitID->GetBindObjectType())
    {
    case EUT_ROLE:
        if (!GetRoleObj(iUnitID))
        {
            return NULL;
        }
        break;

    default:
        break;
    }

    return pGameUnitID->GetBindUnitInfo();
}

CGameRoleObj* CUnitUtility::GetRoleObj(const int iUnitID)
{
    CGameUnitID* pGameUnitID = GameType<CGameUnitID>::Get(iUnitID);
    if (!pGameUnitID)
    {
        return NULL;
    }

    int iObjectIdx = pGameUnitID->GetBindObjectIdx();

    if (pGameUnitID->GetBindObjectType() != EUT_ROLE)
    {
        return NULL;
    }

    CGameRoleObj* pRoleObj = GameTypeK32<CGameRoleObj>::GetByIdx(iObjectIdx);

    return pRoleObj;
}

bool CUnitUtility::IsEqualRole(const RoleID& rstRoleID1, const RoleID& rstRoleID2)
{
    return (rstRoleID1.uin()==rstRoleID2.uin() && rstRoleID1.uiseq()==rstRoleID2.uiseq());
}

// ��ȡ��һ����ɫ����
int CUnitUtility::IterateRoleIdx()
{
    if (m_iUpdateRoleIdx < 0)
    {
        // ��λ��ʼ��λ
        m_iUpdateRoleIdx = GameTypeK32<CGameRoleObj>::GetUsedHead();
    }
    else
    {
        // ��λ��һ����λ
        m_iUpdateRoleIdx = GameTypeK32<CGameRoleObj>::GetNextIdx(m_iUpdateRoleIdx);
    }

    return m_iUpdateRoleIdx;
}

int CUnitUtility::AllocateUnitID()
{
    int iUnitID = GameType<CGameUnitID>::Create();

    // ��ʹ��0��UnitID
    if (iUnitID == 0)
    {
        iUnitID = AllocateUnitID();
    }

    if (iUnitID < 0)
    {
        TRACESVR("Too many Units in Zone!!\n");
    }

    TRACESVR("Allocate UnitID = %d\n", iUnitID);

    return iUnitID;
}

void CUnitUtility::FreeUnitID(const int iUnitID)
{
    CGameUnitID* pGameUnitID = GameType<CGameUnitID>::Get(iUnitID);
    if (!pGameUnitID)
    {
        return;
    }

    switch (pGameUnitID->GetBindObjectType())
    {
    case EUT_ROLE:
        {
            if (iUnitID == m_iUpdateRoleIdx)
            {
                IterateRoleIdx();
            }
            break;
        }
    default:
        {
            break;
        }
    }

    GameType<CGameUnitID>::Del(iUnitID);

    TRACESVR("Free UnitID = %d\n", iUnitID);
}

// ��Unit��Object
int CUnitUtility::BindUnitToObject(int iUnitID, int iObjectIdx, unsigned char ucObjectType)
{
    CGameUnitID* pGameUnitID = GameType<CGameUnitID>::Get(iUnitID);
    ASSERT_AND_LOG_RTN_INT(pGameUnitID);

    pGameUnitID->BindObject(iObjectIdx, ucObjectType);

    return 0;
}

// ����һ����λ
CObj* CUnitUtility::CreateUnit(unsigned char ucUnitType, unsigned int uiKey)
{
    // ������λID
    int iUnitID = CUnitUtility::AllocateUnitID();
    if (iUnitID <= 0)
    {
        TRACESVR("Cannot create unit: type = %d, Alloc unitid fail!\n", ucUnitType);
        return NULL;
    }

    // �����ʹ�����λ
    CObj* pObj = NULL;
    switch (ucUnitType)
    {
    case EUT_ROLE:
        {
            pObj = (CObj*)GameTypeK32<CGameRoleObj>::CreateByKey(uiKey);
            CGameRoleObj* pRoleObj = (CGameRoleObj*)pObj;
            if (pRoleObj)
            {
                pRoleObj->GetRoleInfo().stUnitInfo.iUnitID = iUnitID;
                pRoleObj->GetRoleInfo().stUnitInfo.ucUnitType = EUT_ROLE;
                pRoleObj->SetUin(uiKey);
            }

            break;
        }
    default:
        {
            break;
        }
    }

    if (pObj)
    {
        CUnitUtility::BindUnitToObject(iUnitID, pObj->GetObjectID(), ucUnitType);
    }
    else
    {
        CUnitUtility::FreeUnitID(iUnitID);
        TRACESVR("Cannot create unit: type = %d, Alloc obj fail!\n", ucUnitType);
    }

    return pObj;
}

// ɾ��һ����λ, �ͷ�UnitID, ���ٶ���ID.
// ����ɾ����һ���ǳ�Σ�յ���Ϊ, ���Ҫ�Ƚ���λ��ΪEUS_DELETED״̬, ��AppTick��ɾ��
int CUnitUtility::DeleteUnit(TUNITINFO* pUnitInfo)
{
    int iUnitID = pUnitInfo->iUnitID;
    switch (pUnitInfo->ucUnitType)
    {
    case EUT_ROLE:
        {
            CGameRoleObj* pRoleObj = CUnitUtility::GetRoleObj(iUnitID);
            ASSERT_AND_LOG_RTN_INT(pRoleObj);

            GameTypeK32<CGameRoleObj>::DeleteByKey(pRoleObj->GetRoleID().uin());
            break;
        }

    default:
        {
            break;
        }
    }


    // ɾ��UnitID
    CUnitUtility::FreeUnitID(iUnitID);

    return 0;
}

// ��ȡ��ɫ��λ
CGameRoleObj* CUnitUtility::GetRoleByUin(const int uiUin)
{
    return GameTypeK32<CGameRoleObj>::GetByKey(uiUin);
}

CGameRoleObj* CUnitUtility::GetRoleByID(const RoleID& rstRoleID)
{
    CGameRoleObj* pRoleObj = GetRoleByUin(rstRoleID.uin());
    if (!pRoleObj || pRoleObj->GetRoleID().uiseq() != rstRoleID.uiseq())
    {
        return NULL;
    }

    return pRoleObj;
}

// ���õ�λ״̬
void CUnitUtility::SetUnitStatus(TUNITINFO* pstUnit, EUnitStatus enStatus)
{
    ASSERT_AND_LOG_RTN_VOID(pstUnit);

    pstUnit->uiUnitStatus |= enStatus;
}

// �����λ״̬
void CUnitUtility::ClearUnitStatus(TUNITINFO* pstUnit, EUnitStatus enStatus)
{
    ASSERT_AND_LOG_RTN_VOID(pstUnit);

    pstUnit->uiUnitStatus &= ~(enStatus);
}

