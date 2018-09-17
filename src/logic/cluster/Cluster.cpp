#include "Cluster.h"
#include "NodeProtocol.h"
#include "NodeDefine.h"
#include "IResMgr.h"

bool Cluster::initialize(sl::api::IKernel * pKernel){
	_clusterReady = false;
	return true;
}

bool Cluster::launched(sl::api::IKernel * pKernel){
	if (SLMODULE(Harbor)->getNodeType() != NodeType::MASTER){
		RGS_NODE_HANDLER(SLMODULE(Harbor), NodeProtocol::MASTER_MSG_NEW_NODE, Cluster::newNodeComing);

		_masterIp = SLMODULE(ResMgr)->getResValueString("master/ip");
		_masterPort = SLMODULE(ResMgr)->getResValueInt32("master/port");
		START_TIMER(this, 0, 1, 1000);
	}

	RGS_NODE_HANDLER(SLMODULE(Harbor), NodeProtocol::MASTER_MSG_SERVER_STARTED, Cluster::onClusterIsReady);
	return true;
}

bool Cluster::destory(sl::api::IKernel * pKernel){
	DEL this;
	return true;
}

void Cluster::newNodeComing(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId, const sl::OBStream& args){
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

	//if (SLMODULE(Harbor)->getNodeType() == newNodeType && SLMODULE(Harbor)->getNodeId() >= newNodeId)
	//	return;

	_openNodes.insert(nodeIdx);
	SLMODULE(Harbor)->connect(ip, port, newNodeType, newNodeId, isSameDeivce(pKernel->getLocalIp(), ip));
}

void Cluster::onClusterIsReady(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId, const sl::OBStream& args){
	_clusterReady = true;
}


void Cluster::onTime(sl::api::IKernel* pKernel, int64 timetick){
	SLMODULE(Harbor)->connect(_masterIp.c_str(), _masterPort, -2, 1);
}

bool Cluster::isSameDeivce(const char* localIp, const char* remoteIp){
	if (strcmp(remoteIp, "127.0.0.1") || strcmp(localIp, remoteIp) == 0)
		return true;

	return false;
}
