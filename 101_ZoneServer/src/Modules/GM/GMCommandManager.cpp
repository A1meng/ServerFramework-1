#include <string>

#include "GameProtocol.hpp"
#include "LogAdapter.hpp"
#include "ZoneErrorNumDef.hpp"

#include "GMCommandManager.hpp"

using namespace ServerLib;

CGMCommandManager* CGMCommandManager::m_pInstance = NULL;

CGMCommandManager* CGMCommandManager::Instance()
{
    if(!m_pInstance)
    {
        m_pInstance = new CGMCommandManager;
    }

    return m_pInstance;
}

//��ʼ��GM���������
int CGMCommandManager::Init()
{
    //ע�����е�GM����Ĵ���Handler

    //ע�ᱳ����ص�Handler CGMReposityHandler
    RegisterGMCommand(std::string("AddRepItem"), &m_stReposityHandler);

    //ע��ս����λ��ص�Handler CGMFightUnitHandler
    RegisterGMCommand(std::string("AddFightUnit"), &m_stFightUnitHandler);
    RegisterGMCommand(std::string("AddUnitAttr"), &m_stFightUnitHandler);
    
    //���ս����ص�Handler
    RegisterGMCommand(std::string("AddCombatUnit"), &m_stCombatHandler);

    return 0;
}

//����GM������Handler
IGMBaseCommand* CGMCommandManager::GetCommandHandler(const std::string& strCommand)
{
    int iCommandType = GetHandlerType(strCommand);
    if(iCommandType == GM_COMMAND_TYPE_INVALID)
    {
        return NULL;
    }

    return m_astGMCommandHandler[iCommandType];
}

CGMCommandManager::CGMCommandManager()
{

}

CGMCommandManager::~CGMCommandManager()
{
    return;
}

void CGMCommandManager::RegisterGMCommand(std::string strCommand, IGMBaseCommand* pstCommandHandler)
{
    int iCommandType = GetHandlerType(strCommand);
    if(iCommandType == GM_COMMAND_TYPE_INVALID)
    {
        LOGERROR("Failed to register gm command, invalid type %s\n", strCommand.c_str());
        return;
    }

    m_astGMCommandHandler[iCommandType] = pstCommandHandler;

    return;
}

int CGMCommandManager::GetHandlerType(const std::string& strCommand)
{
    if(strCommand.compare("AddRepItem") == 0)
    {
        return GM_COMMAND_ADD_REPITEM;
    }
    else if(strCommand.compare("AddFightUnit") == 0)
    {
        return GM_COMMAND_ADD_FIGHTUNIT;
    }
    else if(strCommand.compare("AddUnitAttr") == 0)
    {
        return GM_COMMAND_ADD_UNITATTR;
    }
    else if(strCommand.compare("AddCombatUnit") == 0)
    {
        return GM_COMMAND_ADD_COMBATUNIT;
    }
    else if(strCommand.compare("AddSkill") == 0)
    {
        return GM_COMMAND_ADD_SKILL;
    }

    return GM_COMMAND_TYPE_INVALID;
}
