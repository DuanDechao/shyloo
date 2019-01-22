#include "Node.h"
#include "NodeProtocol.h"
#include "NodeDefine.h"
#include "IResMgr.h"
#include "IHarbor.h"
bool Node::initialize(sl::api::IKernel * pKernel){
	_self = this;
	return true;
}

bool Node::launched(sl::api::IKernel * pKernel){
	if (SLMODULE(Harbor)->getNodeType() != NodeType::MASTER){
		RGS_NODE_HANDLER(SLMODULE(Harbor), NodeProtocol::MASTER_MSG_NEW_NODE, Node::newNodeComing);

		_masterIp = SLMODULE(ResMgr)->getResValueString("master/ip");
		_masterPort = SLMODULE(ResMgr)->getResValueInt32("master/port");
		SLMODULE(Harbor)->connect(_masterIp.c_str(), _masterPort, -2, 1);

		SLMODULE(Harbor)->addNodeListener(this);
	}
	return true;
}

bool Node::destory(sl::api::IKernel * pKernel){
	DEL this;
	return true;
}

void Node::newNodeComing(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId, const sl::OBStream& args){
	if(SLMODULE(Harbor)->getNodeType() == NodeType::SLAVE)
		return;

	int32 newNodeType = 0;
	int32 newNodeId = 0;
	const char* ip = nullptr;
	int32 port = 0;
	args >> newNodeType >> newNodeId >> ip >> port;

	//同类型的节点不需要互连
	if(SLMODULE(Harbor)->getNodeType() == newNodeType)
		return;

	int64 nodeIdx = ((int64)newNodeType << 32) | (int64)newNodeId;
	if (_openNodes.find(nodeIdx) != _openNodes.end())
		return;

	if (SLMODULE(Harbor)->getNodeType() < newNodeType)
		return;

	if(newNodeType == NodeType::SLAVE && !isSameMachine(pKernel->getInternetIp(), ip)){
		return;
	}

	SLMODULE(Harbor)->connect(ip, port, newNodeType, newNodeId, isSameMachine(pKernel->getLocalIp(), ip));
}

void Node::onOpen(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId, const char* ip, const int32 port){
	if(nodeType == NodeType::SLAVE){
		registerToSlave(nodeId);
	}
	
	int64 nodeIdx = ((int64)nodeType << 32) | (int64)nodeId;
	_openNodes.insert(nodeIdx);
}

void Node::onClose(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId){
	int64 nodeIdx = ((int64)nodeType << 32) | (int64)nodeId;
	_openNodes.erase(nodeIdx);
}

void Node::registerToSlave(int32 slaveNodeId){
	pid_t pid = getpid();
	sl::BStream<128> args;
	args << (int64)pid;
	SLMODULE(Harbor)->send(NodeType::SLAVE, slaveNodeId, NodeProtocol::CLUSTER_MSG_REGISTER_NODE_TO_SLAVE, args.out());
}

bool Node::isSameMachine(const char* localIp, const char* remoteIp){
	if (strcmp(remoteIp, "127.0.0.1") == 0 || strcmp(localIp, remoteIp) == 0)
		return true;

	return false;
}
