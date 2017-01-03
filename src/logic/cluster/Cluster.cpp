#include "Cluster.h"
#include "NodeProtocol.h"
#include "NodeDefine.h"
IHarbor* Cluster::s_harbor = nullptr;

bool Cluster::initialize(sl::api::IKernel * pKernel){
	return true;
}

bool Cluster::launched(sl::api::IKernel * pKernel){
	s_harbor = (IHarbor*)pKernel->findModule("harbor");
	SLASSERT(s_harbor, "not find module harbor");
	if (s_harbor->getNodeType() != NodeType::MASTER){
		s_harbor->rgsNodeMessageHandler(NodeProtocol::MASTER_MSG_NEW_NODE_COMING, Cluster::newNodeComing);
		s_harbor->connect("127.0.0.1", 7200);
	}
	return true;
}

bool Cluster::destory(sl::api::IKernel * pKernel){
	DEL this;
	return true;
}

void Cluster::newNodeComing(sl::api::IKernel* pKernel, int32 nodeType, int32 nodeId, const OArgs& args){
	const int32 newNodeType = args.getInt32(0);
	const int32 newNodeId = args.getInt32(1);
	const char* ip = args.getString(2);
	const int32 port = args.getInt32(3);
	if (nodeType > newNodeType){
		s_harbor->connect(ip, port);
	}
	else if (nodeType == newNodeType && nodeId < newNodeId){
		s_harbor->connect(ip, port);
	}
}