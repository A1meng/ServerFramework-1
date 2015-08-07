#ifndef __ROLE_NUMBER_OBJ_HPP__
#define __ROLE_NUMBER_OBJ_HPP__

#include "AccountPublic.hpp"
#include "ObjAllocator.hpp"
#include "GameProtocol.hpp"

using namespace ServerLib;

class CRoleNumberObj : public CObj
{
private:
    unsigned int m_uiUin;
    bool m_bNoneRole; // ������û������е�world�϶�û�н�ɫ����ñ�־Ϊtrue
    int m_iRoleNumber; // ���û���world�Ͻ�ɫ����
    RoleID m_astRoleID[10];   //֧�ִ��������ɫ

public:
    CRoleNumberObj();
    virtual ~CRoleNumberObj();

public:
    virtual int Initialize();

public:
    unsigned int GetUin() const;
    void SetUin(unsigned int uiUin);
    void SetNoneRoleFlag();
    void ResetNoneRoleFlag();
    unsigned char GetRoleNumberOnWorld() const;
    void AddOneRoleToWorld(const RoleID& stRoleID);
    void SubOneRoleFromWorld(const RoleID& stRoleID);

    DECLARE_DYN
};

#endif // __ROLE_NUMBER_OBJ_HPP__

