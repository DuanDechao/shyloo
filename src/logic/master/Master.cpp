#include "Master.h"
#include "NodeProtocol.h"
#include "slargs.h"
#include "slxml_reader.h"

bool Master::initialize(sl::api::IKernel * pKernel){
	return true;
}

bool Master::launched(sl::api::IKernel * pKernel){
	FIND_MODULE(_harbor, Harbor);

	_harbor->addNodeListener(this);

	sl::XmlReader server_conf;
	if (!server_conf.loadXml(pKernel->getCoreFile())){
		SLASSERT(false, "can't open xml file %s", pKernel->getCoreFile());
		return false;
	}
	const sl::xml::ISLXmlNode& nodes = server_conf.root()["harbor"][0]["node"];
	for (int32 i = 0; i < nodes.count(); i++){
		if (nodes[i].hasAttribute("hidden") && nodes[i].getAttributeBoolean("hidden"))
			continue;
		_exposes.insert(nodes[i].getAttributeInt32("type"));
	}
	return true;
}

bool Master::destory(sl::api::IKernel * pKernel){
	DEL this;
	return true;
}

void Master::onOpen(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId, const char* ip, const int32 port){
	if (_exposes.find(nodeType) == _exposes.end())
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

}

void Master::sendNewNode(const int32 nodeType, const int32 nodeId, const int32 newNodeType, const int32 newNodeId, const char* ip, const int32 port){
	IArgs<4, 512> args;
	args << newNodeType;
	args << newNodeId;
	args << ip;
	args << port;
	args.fix();
	_harbor->send(nodeType, nodeId, NodeProtocol::MASTER_MSG_NEW_NODE, args.out());
}