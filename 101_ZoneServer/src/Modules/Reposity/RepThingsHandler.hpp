#ifndef __REP_THINGS_HANDLER_HPP__
#define __REP_THINGS_HANDLER_HPP__

#include "SessionDefine.hpp"
#include "Handler.hpp"
#include "RepThingsManager.hpp"

class CGameRoleObj;

class CRepThingsHandler : public IHandler
{
protected:
	 static GameProtocolMsg ms_stZoneMsg;
public:
	virtual ~CRepThingsHandler();

public:
	virtual int OnClientMsg();

private:

    //��ҶԱ�����Ʒ�Ĳ���
    int OnRequestOperaRepItem();

    //�����ȡ��������ϸ��Ϣ
    int OnRequestGetRepInfo();

    //��Ҵ���ս����λװ���Ĳ���
    int OnRequestWearEquipment();

    //��ҳ��۱�����Ʒ
    int OnRequestSellItem();

private:

    // ����ʧ�ܻظ�
     static int SendFailedResponse(unsigned uiMsgID, const unsigned int uiResultID, const TNetHead_V2& rstNetHead);
};

#endif
