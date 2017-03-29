#include "Logic.h"
#include "IHarbor.h"
#include "NodeProtocol.h"
#include "AgentProtocol.h"
#include "NodeDefine.h"
#include "IDCCenter.h"

bool Logic::initialize(sl::api::IKernel * pKernel){
	_self = this;
	return true;
}

bool Logic::launched(sl::api::IKernel * pKernel){
	FIND_MODULE(_harbor, Harbor);
	FIND_MODULE(_objectMgr, ObjectMgr);

	RGS_NODE_HANDLER(_harbor, NodeProtocol::GATE_MSG_BIND_PLAYER_REQ, Logic::onGateBindPlayerOnLogic);
	RGS_NODE_HANDLER(_harbor, NodeProtocol::GATE_MSG_UNBIND_ACCOUNT_REQ, Logic::onGateUnBindPlayerOnLogic);

	return true;
}
bool Logic::destory(sl::api::IKernel * pKernel){
	DEL this;
	return true;
}

void Logic::onGateBindPlayerOnLogic(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId, const OArgs& args){
	int64 actorId = args.getInt64(0);
	int64 accountId = args.getInt64(1);

	const IObject* player = _objectMgr->findObject(actorId);
	if (player){
		//player->setPropInt32()
	}
	
}
void Logic::onGateUnBindPlayerOnLogic(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId, const OArgs& args){

}