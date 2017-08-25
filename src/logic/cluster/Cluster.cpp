#include "Cluster.h"
#include "NodeProtocol.h"
#include "NodeDefine.h"
#include "slxml_reader.h"

bool Cluster::initialize(sl::api::IKernel * pKernel){
	return true;
}

bool Cluster::launched(sl::api::IKernel * pKernel){
	FIND_MODULE(_harbor, Harbor);
	if (_harbor->getNodeType() != NodeType::MASTER){
		RGS_NODE_ARGS_HANDLER(_harbor, NodeProtocol::MASTER_MSG_NEW_NODE, Cluster::newNodeComing);

		sl::XmlReader server_conf;
		if (!server_conf.loadXml(pKernel->getCoreFile())){
			SLASSERT(false, "can't load core file %s", pKernel->getCoreFile());
			return false;
		}
		_masterIp = server_conf.root()["master"][0].getAttributeString("ip");
		_masterPort = server_conf.root()["master"][0].getAttributeInt32("port");
		START_TIMER(this, 0, 1, 1000);
	}
	return true;
}

bool Cluster::destory(sl::api::IKernel * pKernel){
	DEL this;
	return true;
}

void Cluster::newNodeComing(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId, const OArgs& args){
	const int32 newNodeType = args.getInt32(0);
	const int32 newNodeId = args.getInt32(1);
	const char* ip = args.getString(2);
	const int32 port = args.getInt32(3);
	
	int64 nodeIdx = ((int64)newNodeType << 32) | (int64)newNodeId;
	if (_openNodes.find(nodeIdx) != _openNodes.end())
		return;

	if (_harbor->getNodeType() < newNodeType)
		return;

	if (_harbor->getNodeType() == newNodeType && _harbor->getNodeId() >= newNodeId)
		return;

	_openNodes.insert(nodeIdx);
	_harbor->connect(ip, port, newNodeType, newNodeId, isSameDeivce(pKernel->getLocalIp(), ip));
}


void Cluster::onTime(sl::api::IKernel* pKernel, int64 timetick){
	_harbor->connect(_masterIp.c_str(), _masterPort, -2, 1);
}

bool Cluster::isSameDeivce(const char* localIp, const char* remoteIp){
	if (strcmp(remoteIp, "127.0.0.1") || strcmp(localIp, remoteIp) == 0)
		return true;

	return false;
}