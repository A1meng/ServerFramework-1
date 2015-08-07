#ifndef __GM_REPOSITY_HANDLER_HPP__
#define __GM_REPOSITY_HANDLER_HPP__

#include "GMBaseCommand.hpp"

class CGameRoleObj;
class CGMReposityHandler : public IGMBaseCommand
{
public:
    CGMReposityHandler();
    ~CGMReposityHandler() { };

public:
    //ִ����ӦGM����Ĺ���
    virtual int Run(CGameRoleObj *pRoleObj, int iCommandType, std::vector<std::string>& vParams);

protected:
    //���в������
    virtual int SecurityCheck(int iCommandType, std::vector<std::string>& vParams);

private:
    CGameRoleObj* m_pRoleObj;
};

#endif
