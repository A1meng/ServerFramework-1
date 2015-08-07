
#include "GameRole.hpp"
#include "ZoneErrorNumDef.hpp"
#include "LogAdapter.hpp"
#include "ZoneMsgHelper.hpp"
#include "ModuleHelper.hpp"

#include "ChatUtility.hpp"

using namespace ServerLib;

int CChatUtility::SendChatMsg(CGameRoleObj& stRoleObj, int iChannel, const char* pMsg)
{
    if(!pMsg)
    {
        return T_ZONE_SYSTEM_PARA_ERR;
    }

    static GameProtocolMsg stChatNotify;
    CZoneMsgHelper::GenerateMsgHead(stChatNotify, MSGID_WORLD_CHAT_NOTIFY);

    World_Chat_Notify* pstNotify = stChatNotify.mutable_m_stmsgbody()->mutable_m_stworld_chat_notify();

    switch(iChannel)
    {
        case CHAT_CHANNEL_WORLD:
            {
                pstNotify->set_ichannel((ChatChannelType)iChannel);
                pstNotify->mutable_stroleid()->CopyFrom(stRoleObj.GetRoleID());
                //todo jasonxiong2 ������û������
                //pstNotify->set_sznickname(stRoleObj.GetNickName());
                pstNotify->set_izoneid(CModuleHelper::GetZoneID());
                pstNotify->set_szmessage(pMsg);

                //�ȹ㲥�����ߵ����
                CZoneMsgHelper::SendZoneMsgToZoneAll(stChatNotify);

                //ת�������������
                CZoneMsgHelper::SendZoneMsgToWorld(stChatNotify);
            }
            break;

        case CHAT_CHANNEL_SYSTEM:
            {
                //ϵͳ��Ϣ
                pstNotify->set_ichannel((ChatChannelType)iChannel);
                pstNotify->set_szmessage(pMsg);

                //���͸��ͻ���
                CZoneMsgHelper::SendZoneMsgToRole(stChatNotify, &stRoleObj);
            }
            break;

        case CHAT_CHANNEL_PRIVATE:
            {
                //���˽�ģ���ʱ������
                ;
            }
            break;

        default:
            {
                LOGERROR("Failed to do chat, invalid channel %d, uin %u\n", iChannel, stRoleObj.GetUin());
                return T_ZONE_SYSTEM_PARA_ERR;
            }
            break;
    }

    return T_SERVER_SUCESS;
}

