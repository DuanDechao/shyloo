#include "Master.h"
#include "NodeProtocol.h"
#include "slargs.h"
#include "slxml_reader.h"

IHarbor* Master::s_harbor = nullptr;
std::unordered_map<int64, Master::Node> Master::s_nodes;
std::unordered_set<int32> Master::s_exposes;

bool Master::initialize(sl::api::IKernel * pKernel){
	return true;
}

bool Master::launched(sl::api::IKernel * pKernel){
	s_harbor = (IHarbor*)pKernel->findModule("harbor");
	SLASSERT(s_harbor, "not find module harbor");
	s_harbor->addNodeListener(this);

	sl::XmlReader server_conf;
	if (!server_conf.loadXml(pKernel->getCoreFile())){
		SLASSERT(false, "can't open xml file %s", pKernel->getCoreFile());
		return false;
	}
	const sl::xml::ISLXmlNode& nodes = server_conf.root()["harbor"][0]["node"];
	for (int32 i = 0; i < nodes.count(); i++){
		if (nodes[i].hasAttribute("hidden") && nodes[i].getAttributeBoolean("hidden"))
			continue;
		s_exposes.insert(nodes[i].getAttributeInt32("type"));
	}
	return true;
}

bool Master::destory(sl::api::IKernel * pKernel){
	DEL this;
	return true;
}

void Master::onOpen(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId, const char* ip, const int32 port){
	if (s_exposes.find(nodeType) == s_exposes.end())
		return;
	
	int64 node = (((int64)nodeType) << 32) | nodeId;
	auto iter = s_nodes.find(node);
	if (iter != s_nodes.end()){
		SLASSERT(false, "wtf");
		return;
	}
	Node& nodeInfo = s_nodes[node];
	nodeInfo.nodeType = nodeType;
	nodeInfo.nodeId = nodeId;
	SafeSprintf(nodeInfo.ip, sizeof(nodeInfo.ip), "%s", ip);
	nodeInfo.port = port;

	auto itor = s_nodes.begin();
	for (; itor != s_nodes.end(); ++itor){
		if (itor->first != node){
			sendNewNode(itor->second.nodeType, itor->second.nodeId, nodeType, nodeId, ip, port);
			sendNewNode(nodeType, nodeId, itor->second.nodeType, itor->second.nodeId, itor->second.ip, itor->second.port);
		}
	}
}

void Master::sendNewNode(const int32 nodeType, const int32 nodeId, const int32 newNodeType, const int32 newNodeId, const char* ip, const int32 port){
	IArgs<4, 512> args;
	args << newNodeType;
	args << newNodeId;
	args << ip;
	args << port;
	args.fix();
	s_harbor->send(nodeType, nodeId, NodeProtocol::MASTER_MSG_NEW_NODE, args.out());
}