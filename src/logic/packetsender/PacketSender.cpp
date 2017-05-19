#include "PacketSender.h"
#include "NodeProtocol.h"
#include "NodeDefine.h"
#include "IHarbor.h"
#include "GameDefine.h"

bool PacketSender::initialize(sl::api::IKernel * pKernel){
	_self = this;
	_kernel = pKernel;
	return true;
}

bool PacketSender::launched(sl::api::IKernel * pKernel){
	FIND_MODULE(_harbor, Harbor);
	return true;
}

bool PacketSender::destory(sl::api::IKernel * pKernel){
	DEL this;
	return true;
}

void PacketSender::send(int32 gate, int64 id, int32 messageId, const void* context, const int32 size, int8 delay){
	int32 len = sizeof(client::Transfor) + sizeof(client::Header) + size;
	_harbor->prepareSend(NodeType::GATE, gate, NodeProtocol::LOGIC_MSG_TRANSFOR, len);

	char buf[sizeof(client::Transfor) + sizeof(client::Header)];
	client::Transfor& info = *(client::Transfor*)buf;
	info.delay = delay;
	info.actorId = id;

	client::Header& header = *(client::Header*)(buf + sizeof(client::Transfor));
	header.messageId = messageId;
	header.size = sizeof(client::Header) + size;
	_harbor->send(NodeType::GATE, gate, buf, sizeof(buf));
	_harbor->send(NodeType::GATE, gate, context, size);
}

void PacketSender::send(int32 gate, int64 id, int32 messageId, const sl::OBStream& args, int8 delay){
	send(gate, id, messageId, args.getContext(), args.getSize(), delay);
}

void PacketSender::broadcast(const std::unordered_map<int32, std::vector<int64>>& gateIds, int32 messageId, const void* context, const int32 size, int8 delay){
	client::Header header;
	header.messageId = messageId;
	header.size = sizeof(client::Header) + size;

	int32 sendSize = header.size;
	int32 gate = game::INVAILD_GATE_NODE_ID;
	for (auto itor = gateIds.begin(); itor != gateIds.end(); ++itor){
		sendSize += sizeof(client::Brocast) + sizeof(int64)* (int32)itor->second.size();
		gate = itor->first;
	}

	_harbor->prepareSend(NodeType::GATE, gate, NodeProtocol::LOGIC_MSG_BROCAST, sendSize);
	_harbor->send(NodeType::GATE, gate, &header, sizeof(client::Header));
	_harbor->send(NodeType::GATE, gate, context, size);

	for (auto itor = gateIds.begin(); itor != gateIds.end(); ++itor){
		client::Brocast info;
		info.delay = delay;
		info.gate = itor->first;
		info.count = (int32)itor->second.size();

		_harbor->send(NodeType::GATE, gate, &info, sizeof(client::Brocast));
		_harbor->send(NodeType::GATE, gate, itor->second.data(), sizeof(int64)* info.count);
	}
}

void PacketSender::broadcast(const std::unordered_map<int32, std::vector<int64>>& gateIds, int32 messageId, const sl::OBStream& args, int8 delay){
	broadcast(gateIds, messageId, args.getContext(), args.getSize(), delay);
}

void PacketSender::broadcast(int32 messageId, const void* context, const int32 size, int8 delay){
	int32 len = sizeof(client::Header) + size + sizeof(int8);
	_harbor->prepareBroadcast(NodeType::GATE, NodeProtocol::LOGIC_MSG_ALLSVR_BROCAST, len);
	_harbor->broadcast(NodeType::GATE, &delay, sizeof(int8));

	client::Header header;
	header.messageId = messageId;
	header.size = size + sizeof(client::Header);
	_harbor->broadcast(NodeType::GATE, &header, sizeof(client::Header));
	_harbor->broadcast(NodeType::GATE, context, size);
}

void PacketSender::broadcast(int32 messageId, const sl::OBStream& args, int8 delay){
	broadcast(messageId, args.getContext(), args.getSize(), delay);
}