#include "PacketSender.h"
#include "NodeProtocol.h"
#include "NodeDefine.h"
#include "IHarbor.h"

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

	char* buf[sizeof(client::Transfor) + sizeof(client::Header)];
	client::Transfor& info = *(client::Transfor*)buf;
	info.delay = delay;
	info.actorId = id;

	client::Header& header = *(client::Header*)(buf + sizeof(client::Transfor));
	header.messageId = messageId;
	header.size = sizeof(client::Header) + size;
	_harbor->send(NodeType::GATE, gate, buf, sizeof(buf));
	_harbor->send(NodeType::GATE, gate, context, size);
}