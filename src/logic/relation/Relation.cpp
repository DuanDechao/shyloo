#include "Relation.h"
#include "IHarbor.h"
#include "IObjectLocator.h"
#include "IPacketSender.h"
#include "NodeDefine.h"
#include "GameDefine.h"

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





