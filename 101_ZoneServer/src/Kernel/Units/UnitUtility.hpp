#ifndef __UNIT_UNTILITY_HPP__
#define __UNIT_UNTILITY_HPP__

#include "GameProtocol.hpp"
#include "LogAdapter.hpp"
#include "ZoneMsgHelper.hpp"

using namespace ServerLib;

class CUnitUtility
{
public:
    // ��UnitID��ȡ��λ����, ��λ����, �Լ���������
	static TUNITINFO* GetUnitInfo(const int iUnitID);

    static CGameRoleObj* GetRoleObj(const int iUnitID);

    // ��ȡ��ɫ��λ
    static CGameRoleObj* GetRoleByUin(const int uiUin);
    static CGameRoleObj* GetRoleByID(const RoleID& rstRoleID);

	static bool IsEqualRole(const RoleID& rstRoleID1, const RoleID& rstRoleID2);

public:
    // ��λ״̬����

    // ���õ�λ״̬
    static void SetUnitStatus(TUNITINFO* pstUnit, EUnitStatus enStatus);

    // �����λ״̬
    static void ClearUnitStatus(TUNITINFO* pstUnit, EUnitStatus enStatus);

    // �жϵ�λ״̬
    static inline bool IsUnitStatusSet(TUNITINFO* pstUnit, EUnitStatus enStatus)
    {
        ASSERT_AND_LOG_RTN_BOOL(pstUnit);

        return pstUnit->uiUnitStatus & enStatus;
    }

    // ����״̬
    static inline bool IsUnitOnline(TUNITINFO* pstUnit)
    {
        return IsUnitStatusSet(pstUnit, EGUS_ONLINE);
    }

    // ����ɾ��״̬
    static inline void SetDeletedStatus(TUNITINFO* pstUnit)
    {
        ASSERT_AND_LOG_RTN_VOID(pstUnit);

        SetUnitStatus(pstUnit, EGUS_DELETE);
    }

public:
    // ������ͷ�UnitID
    static int AllocateUnitID();
    static void FreeUnitID(const int iUnitID);

    // ��GameUnitID�а󶨵�λ�ͳ���
    static int BindUnitToObject(int iUnitID, int iObjectIdx, unsigned char ucObjectType);

    // ����һ����λ����
    static CObj* CreateUnit(unsigned char ucUnitType, unsigned int uiKey = 0);

	// ɾ��һ����λ, �ͷ�UnitID, ���ٶ���ID. 
	// ����ɾ����һ���ǳ�Σ�յ���Ϊ, ���Ҫ�Ƚ���λ��ΪEUS_DELETED״̬, ��AppTick��ɾ��
    static int DeleteUnit(TUNITINFO* pUnitInfo);

public:
    // Ϊ�˾�ϸ����ÿ�ֵ�λ�ı�������, ��ϵͳ��λ�ֱ����

    // ��ȡ��һ����λ����
    static int IterateRoleIdx();
    static int IterateEggIdx();

private:
    static int m_iUpdateRoleIdx;
};

void create_dump(void);
#endif


