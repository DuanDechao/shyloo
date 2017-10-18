#include "Relation.h"
#include "IHarbor.h"
#include "IObjectLocator.h"
#include "IPacketSender.h"
#include "NodeDefine.h"
#include "GameDefine.h"
#include "slstring_utils.h"
#include "ChatProtocol.h"
#include "NodeProtocol.h"
#include "EventID.h"
#include "IEventEngine.h"

bool Relation::initialize(sl::api::IKernel * pKernel){
	_kernel = pKernel;
	_self = this;
	return true;
}

bool Relation::launched(sl::api::IKernel * pKernel){
	FIND_MODULE(_harbor, Harbor);
	if (_harbor->getNodeType() != NodeType::RELATION)
		return true;

	FIND_MODULE(_objectLocator, ObjectLocator);
	FIND_MODULE(_packetSender, PacketSender);
	FIND_MODULE(_eventEngine, EventEngine);

	_chatBalanceIp = pKernel->getCmdArg("chat_balance_ip");
	_chatBalancePort = sl::CStringUtils::StringAsInt32(pKernel->getCmdArg("chat_balance_port"));
	if (!pKernel->startTcpClient(this, _chatBalanceIp.c_str(), _chatBalancePort, 64 * 1024, 64 * 1024)){
		ECHO_ERROR("connect chat balancer %s:%d error", _chatBalanceIp.c_str(), _chatBalancePort);
		return false;
	}

	_channelIp = "";
	_channelPort = 0;

	_status = STATUS_NOT_GET_CHANNEL;

	return true;
}

bool Relation::destory(sl::api::IKernel * pKernel){
	DEL this;
	return true;
}

void Relation::sendToClient(int64 actorId, int32 messageId, const sl::OBStream& args){
	int32 gate = _objectLocator->findObjectGate(actorId);
	if (gate == game::NODE_INVALID_ID)
		return;

	_packetSender->send(gate, actorId, messageId, args);
}

int32 Relation::onRecv(sl::api::IKernel* pKernel, const char* pContext, int dwLen){
	if (dwLen < 8)
		return 0;

	int32 msgID = *(int32*)pContext;
	int32 msgSize = *(int32*)((char*)pContext + sizeof(int32));
	if (msgSize >= dwLen){
		switch (msgID){
		case ChatProtocolID::BALANCE_MSG_CHANNEL_ADDRESS_INFO_SYNC:{
			const ChatBalancer::addrinfo* pInfo = (ChatBalancer::addrinfo*)((const char*)pContext + sizeof(int32)* 2);
			if (pInfo->errCode){
				SLASSERT(false, "wtf");
				return msgSize;
			}

			_channelIp = pInfo->ip;
			_channelPort = pInfo->port;

			ECHO_TRACE("Get Channel info %s %d", _channelIp.c_str(), _channelPort);
			_status = STATUS_HAS_GET_CHANNEL;

			IArgs<2, 512> args;
			args << _channelIp.getString() << _channelPort;
			args.fix();
			_harbor->broadcast(NodeProtocol::RELATION_MSG_SYNC_CHAT_CHANNEL_ADDRESS, args.out());

			logic_event::ChatAddress evt;
			evt.ip = _channelIp.getString();
			evt.port = _channelPort;
			_eventEngine->execEvent(logic_event::EVENT_RELATION_SYNC_CHAT_CHANNEL_ADDRESS, &evt, sizeof(evt));
		}
		default:
			SLASSERT(false, "wtf");
			break;
		}

		return msgSize;
	}

	return 0;
}

void Relation::onOpen(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId, const char* ip, const int32 port){

}

void Relation::onClose(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId){

}





