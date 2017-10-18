#include "Chat.h"
#include "IEventEngine.h"
#include "IHarbor.h"
#include "NodeDefine.h"
#include "EventID.h"
#include "NodeProtocol.h"
#include "IPlayerMgr.h"
#include "Attr.h"
#include "ChatProtocol.h"
#include "IDCCenter.h"
#include "Protocol.pb.h"

#define APPID 1
bool Chat::initialize(sl::api::IKernel * pKernel){
	_kernel = pKernel;
	_self = this;
	_channelIP = "";
	_channelPort = 0;
	_bConnected = false;
	return true;
}

bool Chat::launched(sl::api::IKernel * pKernel){
	FIND_MODULE(_harbor, Harbor);
	FIND_MODULE(_eventEngine, EventEngine);
	if (_harbor->getNodeType() != NodeType::RELATION){
		RGS_NODE_ARGS_HANDLER(_harbor, NodeProtocol::RELATION_MSG_SYNC_CHAT_CHANNEL_ADDRESS, Chat::onRelationChatChannelSync);
	}
	else{
		RGS_EVENT_HANDLER(_eventEngine, logic_event::EVENT_RELATION_SYNC_CHAT_CHANNEL_ADDRESS, Chat::onRelationChannelSync);
	}

	if (_harbor->getNodeType() == NodeType::LOGIC){
		FIND_MODULE(_playerMgr, PlayerMgr);

		RGS_EVENT_HANDLER(_eventEngine, logic_event::EVENT_LOGIC_PLAYER_ONLINE, Chat::onPlayerOnline);

	}
	return true;
}

bool Chat::destory(sl::api::IKernel * pKernel){
	return true;
}

void Chat::onPlayerOnline(sl::api::IKernel* pKernel, const void* context, const int32 size){
	SLASSERT(size == sizeof(logic_event::Biology), "invailed event");
	IObject* object = ((logic_event::Biology*)context)->object;
	SLASSERT(object && object->getPropInt8(attr_def::type) == protocol::ObjectType::OBJECT_TYPE_PLAYER, "wtfd");

	if (_bConnected){
		int32 messageId = ChatProtocolID::CHAT_API_RGS_LISTENER;
		int32 len = sizeof(ChatPublic::TicketID) + sizeof(int32)* 2;
		ChatPublic::TicketID info(APPID, object->getID());

		_self->send(&messageId, sizeof(int32));
		_self->send(&len, sizeof(int32));
		_self->send(&info, sizeof(info));
	}
}

void Chat::onRelationChannelSync(sl::api::IKernel* pKernel, const void* context, const int32 size){
	SLASSERT(size == sizeof(logic_event::ChatAddress), "invailed event");
	const char* ip = ((logic_event::ChatAddress*)context)->ip;
	const int32 port = ((logic_event::ChatAddress*)context)->port;

	syncChatChannel(pKernel, ip, port);
}

void Chat::onRelationChatChannelSync(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId, const OArgs& args){
	const char* ip = args.getString(0);
	int32 port = args.getInt32(1);

	syncChatChannel(pKernel, ip, port);
}

void Chat::syncChatChannel(sl::api::IKernel* pKernel, const char* ip, const int32 port){
	if (_channelIP == ip && _channelPort == port && _bConnected)
		return;

	_channelIP = ip;
	_channelPort = port;

	if (_bConnected)
		_self->close();
	else{
		ECHO_TRACE("get channel %s:%d", _channelIP.getString(), _channelPort);
		if (!pKernel->startTcpClient(_self, _channelIP.getString(), _channelPort, 10 * 1024 * 1024, 10 * 1024 * 1024)){
			ECHO_TRACE("connnect channel %s:%d failed, reconnect....", _channelIP.getString(), _channelPort);
			START_TIMER(_self, 1000, 1, 1000);
		}
	}
}

void Chat::onTime(sl::api::IKernel* pKernel, int64 timetick){
	if (!pKernel->startTcpClient(_self, _channelIP.getString(), _channelPort, 10 * 1024 * 1024, 10 * 1024 * 1024)){
		ECHO_TRACE("connect channel %s:%d failed, reconnect...", _channelIP.getString(), _channelPort);
		pKernel->killTimer(_self);
		START_TIMER(_self, 1000, 1, 1000);
	}
}

void Chat::onDisconnect(sl::api::IKernel* pKernel){
	ECHO_TRACE("channel %s:%d has broken, reconnect....", _channelIP.getString(), _channelPort);
	START_TIMER(_self, 1000, 1, 1000);
	_bConnected = false;
}

void Chat::onConnected(sl::api::IKernel* pKernel){
	_bConnected = true;

	if (_harbor->getNodeType() == NodeType::LOGIC){
		Chat* tSelf = _self;
		_playerMgr->foreach(OCTempProp::CHAT_TOKEN_TIMER, [&tSelf](sl::api::IKernel* pKernel, IObject* object, int64 tick){
			int32 messageId = ChatProtocolID::CHAT_API_RGS_LISTENER;
			int32 len = sizeof(ChatPublic::TicketID) + sizeof(int32)* 2;
			ChatPublic::TicketID info(APPID, object->getID());

			tSelf->send(&messageId, sizeof(int32));
			tSelf->send(&len, sizeof(int32));
			tSelf->send(&info, sizeof(info));
		});
	}
}
