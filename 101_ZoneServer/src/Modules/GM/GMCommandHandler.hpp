#ifndef __GM_COMMAND_HANDLER_HPP__
#define __GM_COMMAND_HANDLER_HPP__

#include <string>

#include "Handler.hpp"
#include "GMBaseCommand.hpp"
#include "GameProtocol.hpp"
#include "SessionDefine.hpp"
#include "ZoneErrorNumDef.hpp"

class CGameRoleObj;

class CGMCommandHandler: public IHandler
{
public:
    virtual ~CGMCommandHandler(void);
    CGMCommandHandler();
public:
    int OnClientMsg();

private:
    int ParseParameter();
    int OnRequestGM();
    int RunCommand();

    //����Ƿ���GM�û�
    int CheckIsGMUser();

private:
    int SendFailedResponse(int iResultID, const TNetHead_V2& rstNetHead);
    int SendSuccessfulResponse();

protected:
	static GameProtocolMsg m_stGameMsg;

private:

    //����������GM�����еĲ���
    std::vector<std::string> m_vParams;
};

#endif
