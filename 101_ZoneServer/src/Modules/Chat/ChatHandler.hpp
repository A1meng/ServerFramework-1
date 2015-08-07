
#ifndef __CHAT_HANDLER_HPP__
#define __CHAT_HANDLER_HPP__

#include "Handler.hpp"
#include "GameProtocol.hpp"
#include "SessionDefine.hpp"
#include "ZoneErrorNumDef.hpp"

class CGameRoleObj;

class CChatHandler: public IHandler
{
public:
    virtual ~CChatHandler(void);
    CChatHandler();
public:
    int OnClientMsg();

private:

    //������Ϣ�Ĵ���
    int OnRequestChat();

    //����������Ϣ������
    int OnNotifyChat();

private:
    int SendFailedResponse(int iMsgID, int iResultID, const TNetHead_V2& rstNetHead);

    //���ͳɹ��Ļظ�
    int SendSuccessfulResponse();

protected:
	static GameProtocolMsg m_stGameMsg;
};

#endif
