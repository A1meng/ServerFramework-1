#ifndef __UNIT_BUFFER_MANAGER_HPP__
#define __UNIT_BUFFER_MANAGER_HPP__

#include <vector>

#include "GameProtocol.hpp"
#include "GameConfigDefine.hpp"

using namespace GameConfig;

class CUnitBufferObj;
class CUnitBufferManager
{
public:
    CUnitBufferManager();
    ~CUnitBufferManager();

    int Initialize();

    //ӵ�е�ս����λ��UnitID
    int GetOwnerUnitID();
    void SetOwnerUnitID(int iUnitID);

    //����һ��Buff
    int AddUnitBuff(unsigned int uin, int iCrossID, int iBuffID, int iCastUnitID, Zone_CombatAddBuff_Notify& stNotify);

    //��������ЧBuff
    int DoUnitBuffEffect(unsigned int uin, int iCrossID, int iTriggerType, int iTriggerUnitID, Zone_DoBuffEffect_Notify& stNotify, int* pDamageNum = NULL);

    //��������BUFF�Ļغ���
    int DecreaseAllBuffRound(Zone_RemoveBuff_Notify& stNotify);

    //�Ƴ�BUFFЧ��������ID
    int DelUnitBuffByID(int iBuffID, RemoveBuffEffect& stRemoveBuffNotify);

public:

    //��ȡ����BUFF������ID
    void GetUnitBuffID(std::vector<int>& vBuffIDs);

    //�Ƿ���BUFFIDͬ���͵�BUFF
    bool HasBuffOfSameType(int iBuffID);

    //�Ƿ�����ͬID��BUFF
    bool HasBuffOfSameID(int iBuffID);

    //����BuffManager�����е�BuffObj
    void ClearBuffObj();

private:

    //����Buff��Ч��ʵ��Ч��
    int DoOneBuffEffect(unsigned int uin, int iCrossID, const SFightBuffConfig& stBuffConfig, BuffEffect& stEffectNotify, int iCastUnitID = -1, int iTriggerUnitID = -1, int* pDamageNum = NULL);

    //������Ե��ӵ�Buff,����true��ʾ��Buff���Ե�����ȥ
    bool ProcessOverlyingBuff(int iBuffOverlyingType, int iBuffLevel, RemoveBuffEffect& stRemoveBuffNotify);

    //��Ŀ����Чһ��BUFF��ʵ��Ч��
    int ProcessRealBuffEffect(unsigned int uin, int iCrossID, const SFightBuffConfig& stBuffConfig, int iTargetUnitID, BuffEffect& stEffectNotify, int* pDamageNum = NULL);

    //���������滻����Buff���滻ǰ��ֵ
    void ProcessReplaceAttrBuffValue(int iAttrType, int iAddNum);

    //����BuffObj��Indexɾ��Buff
    int DelUnitBuffByIndex(int& iBuffObjIndex, RemoveBuffEffect& stRemoveBuffNotify);

    //�Ƴ�BUFF��Ч��
    int RemoveBuffEffects(CUnitBufferObj& stBuffObj, const SFightBuffConfig& stFightBuffConfig, RemoveOneBuffEffect& stRemoveOneNotify);

    //����ID����BuffObj
    CUnitBufferObj* GetUnitBufferObjByID(int iBuffID);

    //����һ���µ�BufferObj
    CUnitBufferObj* CreateNewBufferObj();

private:

    //BuffManager��ӵ����UnitID
    int m_iUnitID;

    //���и��ӵ�BuffObj��index
    int m_iUnitBuffObjNum;
    int m_aiUnitBuffObjIndex[MAX_FIGHT_UNIT_BUFF_NUM];
};

#endif
