#ifndef __GM_COMMAND_MANAGER_HPP__
#define __GM_COMMAND_MANAGER_HPP__

#include <string>

#include "GMBaseCommand.hpp"

#include "GMReposityHandler.hpp"
#include "GMFightUnitHandler.hpp"
#include "GMCombatHandler.hpp"

//���֧��100��GM Command
#define MAX_GM_COMMAND_HANDLER_NUM  500

class CGMCommandManager
{
public:

    static CGMCommandManager* Instance();

    //��ʼ��GM���������
    int Init();

    //����GM������Handler
    IGMBaseCommand* GetCommandHandler(const std::string& strCommand);

    int GetHandlerType(const std::string& strCommand);

    ~CGMCommandManager();

private:
    CGMCommandManager();

    void RegisterGMCommand(std::string strCommand, IGMBaseCommand* pstCommandHandler);

private:
    static CGMCommandManager* m_pInstance;

    IGMBaseCommand* m_astGMCommandHandler[MAX_GM_COMMAND_HANDLER_NUM];

    //������ص�GM����
    CGMReposityHandler m_stReposityHandler;

    //ս����λ��ص�GM����
    CGMFightUnitHandler m_stFightUnitHandler;

    //���ս�����
    CGMCombatHandler m_stCombatHandler;
};

#endif
