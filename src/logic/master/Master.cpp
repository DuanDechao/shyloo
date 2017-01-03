#include "Master.h"
#include "NodeProtocol.h"
#include "slargs.h"

IHarbor* Master::s_harbor = nullptr;
std::unordered_map<int64, Master::Node> Master::s_nodes;

bool Master::initialize(sl::api::IKernel * pKernel){
	return true;
}
bool Master::launched(sl::api::IKernel * pKernel){
	s_harbor = (IHarbor*)pKernel->findModule("harbor");
	SLASSERT(s_harbor, "not find module harbor");
	s_harbor->addNodeListener(this);
	return true;
}
bool Master::destory(sl::api::IKernel * pKernel){
	DEL this;
	return true;
}

void Master::onOpen(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId, const char* ip, const int32 port){
	int64 node = (((int64)nodeType) << 32) | nodeId;
	auto iter = s_nodes.find(node);
	if (iter != s_nodes.end()){
		SLASSERT(false, "wtf");
		return;
	}

	s_nodes[node].nodeType = nodeType;
	s_nodes[node].nodeId = nodeId;
	SafeSprintf(s_nodes[node].ip, sizeof(s_nodes[node].ip), "%s", ip);
	s_nodes[node].port = port;

	auto itor = s_nodes.begin();
	for (; itor != s_nodes.end(); ++itor){
		if (itor->first != node){
			sendNodeComing(itor->second.nodeType, itor->second.nodeId, nodeType, nodeId, ip, port);
			sendNodeComing(nodeType, nodeId, itor->second.nodeType, itor->second.nodeId, itor->second.ip, itor->second.port);
		}
	}
}

void Master::sendNodeComing(const int32 nodeType, const int32 nodeId, const int32 newNodeType, const int32 newNodeId, const char* ip, const int32 port){
	IArgs<4, 512> args;
	args << newNodeType;
	args << newNodeId;
	args << ip;
	args << port;
	args.fix();
	s_harbor->send(nodeType, nodeId, NodeProtocol::MASTER_MSG_NEW_NODE_COMING, args.out());
}