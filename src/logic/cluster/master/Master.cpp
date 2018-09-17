#include "Master.h"
#include "NodeProtocol.h"
#include "slargs.h"
#include "IHarbor.h"
#include "NodeDefine.h"
#include "IResMgr.h"
#include "IAgent.h"

bool Master::initialize(sl::api::IKernel * pKernel){
	if(SLMODULE(Harbor)->getNodeType() != NodeType::MASTER)
		return true;
	
	int32 port = SLMODULE(ResMgr)->getResValueInt32("master/port");
	SLMODULE(Harbor)->setPort(port);
	SLMODULE(Harbor)->setLocalNodeId(1);

	int32 consolePort = SLMODULE(ResMgr)->getResValueInt32("master/consolePort");
	SLMODULE(Agent)->setPort(consolePort);

	return true;
}

bool Master::launched(sl::api::IKernel * pKernel){
	if(SLMODULE(Harbor)->getNodeType() != NodeType::MASTER)
		return true;
	
	SLMODULE(Harbor)->addNodeListener(this);
	return true;
}

bool Master::destory(sl::api::IKernel * pKernel){
	DEL this;
	return true;
}


void Master::onOpen(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId, const char* ip, const int32 port){
	if(nodeType == NodeType::SLAVE)
		return;

	int64 node = (((int64)nodeType) << 32) | nodeId;
	auto iter = _nodes.find(node);
	if (iter != _nodes.end()){
		SLASSERT(false, "wtf");
		return;
	}
	Node& nodeInfo = _nodes[node];
	nodeInfo.nodeType = nodeType;
	nodeInfo.nodeId = nodeId;
	SafeSprintf(nodeInfo.ip, sizeof(nodeInfo.ip), "%s", ip);
	nodeInfo.port = port;
	
	auto itor = _nodes.begin();
	for (; itor != _nodes.end(); ++itor){
		if (itor->first != node){
			sendNewNode(itor->second.nodeType, itor->second.nodeId, nodeType, nodeId, ip, port);
			sendNewNode(nodeType, nodeId, itor->second.nodeType, itor->second.nodeId, itor->second.ip, itor->second.port);
		}
	}
}

void Master::onClose(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId){
	if(nodeType == NodeType::SLAVE)
		return;

	int64 node = (((int64)nodeType) << 32) | nodeId;
	auto iter = _nodes.find(node);
	if (iter == _nodes.end()){
		SLASSERT(false, "wtf");
		return;
	}
	_nodes.erase(iter);
}

void Master::sendNewNode(const int32 nodeType, const int32 nodeId, const int32 newNodeType, const int32 newNodeId, const char* ip, const int32 port){
	sl::BStream<512> args;
	args << newNodeType;
	args << newNodeId;
	args << ip;
	args << port;
	SLMODULE(Harbor)->send(nodeType, nodeId, NodeProtocol::MASTER_MSG_NEW_NODE, args.out());
}
