
#ifndef __GAME_UNIT_ID_HPP__
#define __GAME_UNIT_ID_HPP__

// ������λID��. ��������λ����������֮�����ϵ

#include "GameProtocol.hpp"
#include "ObjIdxList.hpp"
#include "ObjAllocator.hpp"

using namespace ServerLib;

class CGameUnitID : public CObj
{
public:
    CGameUnitID() {};
    virtual ~CGameUnitID() {};
    virtual int Initialize();
    virtual int Resume();

public:
    DECLARE_DYN

public:
    // �󶨶���Idx
    void BindObject(int iObjectIdx, unsigned char ucObjectType);

public:
    // ��ȡ�󶨶����Idx
    int GetBindObjectIdx();

    // ��ȡ�󶨶��������
    int GetBindObjectType();

public:
    // ��ȡ�󶨶��������
    TUNITINFO* GetBindUnitInfo();

public:
    // ���úͻ�ȡͬһ�����ڵĵ�λ����
    TObjIdxListNode& GetListNode();

    // ���úͻ�ȡͬһ��������ͬ���͵�λ������
    TObjIdxListNode& GetListNode2();

private:
    // ���ö��������
    void BindUnitInfo();

private: 
    int m_iBindObjectIdx;
    unsigned char m_ucBindObjectType;
    
    // ͬһ������, ���е�λ������
    TObjIdxListNode m_stListNode;

    // ͬһ������, ��ͬ���͵ĵ�λ����
    TObjIdxListNode m_stListNode2;

private:
    TUNITINFO* m_pstUnitInfo;
};

extern TObjIdxListNode* GetGameUnitIDListNode(int iIdx);
extern TObjIdxListNode* GetGameUnitIDListNode2(int iIdx);

#endif
