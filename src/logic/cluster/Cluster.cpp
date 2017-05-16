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
		const char* ip = server_conf.root()["master"][0].getAttributeString("ip");
		const int32 port = server_conf.root()["master"][0].getAttributeInt32("port");
		_harbor->connect(ip, port);
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
	_harbor->connect(ip, port);
}