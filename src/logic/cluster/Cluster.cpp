#include "Cluster.h"
#include "NodeProtocol.h"
#include "NodeDefine.h"
#include "slxml_reader.h"

IHarbor* Cluster::s_harbor = nullptr;
std::set<int64> Cluster::s_openNodes;
bool Cluster::initialize(sl::api::IKernel * pKernel){
	return true;
}

bool Cluster::launched(sl::api::IKernel * pKernel){
	s_harbor = (IHarbor*)pKernel->findModule("harbor");
	SLASSERT(s_harbor, "not find module harbor");
	if (s_harbor->getNodeType() != NodeType::MASTER){
		s_harbor->rgsNodeMessageHandler(NodeProtocol::MASTER_MSG_NEW_NODE, Cluster::newNodeComing);

		sl::XmlReader server_conf;
		if (!server_conf.loadXml(pKernel->getCoreFile())){
			SLASSERT(false, "can't load core file %s", pKernel->getCoreFile());
			return false;
		}
		const char* ip = server_conf.root()["master"][0].getAttributeString("ip");
		const int32 port = server_conf.root()["master"][0].getAttributeInt32("port");
		s_harbor->connect(ip, port);
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
	
	int64 nodeIdx = ((int64)newNodeType << 32) | (int64)newNodeId;
	if (s_openNodes.find(nodeIdx) != s_openNodes.end())
		return;

	if (s_harbor->getNodeType() < newNodeType)
		return;

	if (s_harbor->getNodeType() == newNodeType && s_harbor->getNodeId() >= newNodeId)
		return;

	s_openNodes.insert(nodeIdx);
	s_harbor->connect(ip, port);
}