#include "Harbor.h"
#include "slxml_reader.h"
#include "slargs.h"

sl::api::ITcpSession* NodeSessionServer::mallocTcpSession(sl::api::IKernel* pKernel){
	return CREATE_POOL_OBJECT(NodeSession, m_pHarbor);
}

class NodeCBMessageHandler : public INodeMessageHandler{
public:
	NodeCBMessageHandler(const NodeCB cb) : m_cb(cb){}
	virtual ~NodeCBMessageHandler() {}

	virtual void DealNodeMessage(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId, const char* pContext, const int32 size){
		m_cb(pKernel, nodeType, nodeId, sl::OBStream(pContext, size));
	}
private:
	NodeCB		m_cb;
};


class NodeArgsCBMessageHandler : public INodeMessageHandler{
public:
	NodeArgsCBMessageHandler(const NodeArgsCB cb) : _cb(cb){}
	virtual ~NodeArgsCBMessageHandler() {}

	virtual void DealNodeMessage(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId, const char* pContext, const int32 size){
		OArgs args(pContext, size);
		_cb(pKernel, nodeType, nodeId, args);
	}
private:
	NodeArgsCB		_cb;
};

bool Harbor::initialize(sl::api::IKernel * pKernel){
	_pKernel = pKernel;
	return true;
}

bool Harbor::launched(sl::api::IKernel * pKernel){
	XmlReader server_conf;
	if (!server_conf.loadXml(pKernel->getCoreFile())){
		SLASSERT(false, "can not load core file %s", pKernel->getCoreFile());
		return false;
	}
	_sendSize = server_conf.root()["harbor"][0].getAttributeInt32("send");
	_recvSize = server_conf.root()["harbor"][0].getAttributeInt32("recv");
	const sl::xml::ISLXmlNode& nodes = server_conf.root()["harbor"][0]["node"];
	for (int32 i = 0; i < nodes.count(); i++){
		int32 type = nodes[i].getAttributeInt32("type");
		_nodeNames[type] = nodes[i].getAttributeString("name");
	}

	XmlReader conf;
	if (!conf.loadXml(pKernel->getConfigFile())){
		SLASSERT(false, "can not load config file %s", pKernel->getConfigFile());
		return false;
	}
	_nodeType = conf.root()["harbor"][0].getAttributeInt32("type");
	_nodeId = sl::CStringUtils::StringAsInt32(pKernel->getCmdArg("node_id"));
	
	if (pKernel->getCmdArg("harbor"))
		_port = sl::CStringUtils::StringAsInt32(pKernel->getCmdArg("harbor"));
	else
		_port = 0;

	_pServer = NEW NodeSessionServer(this);
	if (!_pServer){
		SLASSERT(false, "wtf");
		return false;
	}

	START_TIMER(this, 0, 1, 500);
	return true; 
}

bool Harbor::destory(sl::api::IKernel * pKernel){
	if (_pServer)
		DEL _pServer;
	_pServer = nullptr;

	DEL this;

	return true;
}

void Harbor::onNodeOpen(sl::api::IKernel* pKernel, int32 nodeType, int32 nodeId, const char* ip, int32 nodePort, NodeSession* session){

	_allNode[nodeType].insert(std::make_pair(nodeId, session));
	for (auto& listener : _listenerPool){
		listener->onOpen(pKernel, nodeType, nodeId, ip, nodePort);
	}
	ECHO_TRACE("node[%s:%d] from[%s:%d] opened", _nodeNames[nodeType].c_str(), _nodeId, ip, nodePort);
}

void Harbor::onNodeClose(sl::api::IKernel* pKernel, int32 nodeType, int32 nodeId){
	_allNode[nodeType].erase(nodeId);
	
	for (auto& listener : _listenerPool){
		listener->onClose(pKernel, nodeType, nodeId);
	}
}

void Harbor::onNodeMessage(sl::api::IKernel* pKernel, int32 nodeType, int32 nodeId, const char* pszBuf, const int32 size){
	SLASSERT(size >= sizeof(int32), "invalid node message context!");
	int32 messageId = *(int32*)pszBuf;
	auto itor = _allCBPool.find(messageId);
	if (itor == _allCBPool.end()){
		//SLASSERT(false, "have no messageId %d", messageId);
		return;
	}
	for (auto& handler : _allCBPool[messageId]){
		handler->DealNodeMessage(pKernel, nodeType, nodeId, pszBuf + sizeof(int32), size - sizeof(int32));
	}
}

void Harbor::addNodeListener(INodeListener* pNodeListener){
	_listenerPool.push_back(pNodeListener);
}

void Harbor::connect(const char* ip, const int32 port){
	sl::api::IKernel * pKernel = _pKernel;
	NodeSession* pSession = (NodeSession *)_pServer->mallocTcpSession(_pKernel);
	SLASSERT(pSession, "wtf");
	pSession->setConnect(ip, port);
	if (!_pKernel->startTcpClient(pSession, ip, port, _sendSize, _recvSize)){
		START_TIMER(pSession, 0, TIMER_BEAT_FOREVER, RECONNECT_INTERVAL);
		ECHO_ERROR("connect [%s:%d] failed!", ip, port);
	}
	else{
		ECHO_TRACE("connect [%s:%d] success!", ip, port);
	}
}

void Harbor::rgsNodeArgsMessageHandler(int32 messageId, const NodeArgsCB& handler){
	_allCBPool[messageId].push_back(NEW NodeArgsCBMessageHandler(handler));
}

void Harbor::rgsNodeMessageHandler(int32 messageId, const NodeCB& handler){
	_allCBPool[messageId].push_back(NEW NodeCBMessageHandler(handler));
}

void Harbor::startListening(sl::api::IKernel* pKernel){
	if (!_port)
		return;

	if (pKernel->startTcpServer(_pServer, "0.0.0.0", _port, _sendSize, _recvSize)){
		ECHO_TRACE("start server[%s:%d] success", "0.0.0.0", _port);
	}
	else{
		ECHO_ERROR("start server[%s:%d] failed", "0.0.0.0", _port);
	}
}

void Harbor::send(int32 nodeType, int32 nodeId, int32 messageId, const OArgs& args){
	auto itor = _allNode.find(nodeType);
	if (itor == _allNode.end()){
		//SLASSERT(false, "wtf");
		return;
	}
	auto itor1 = itor->second.find(nodeId);
	if (itor1 == itor->second.end()){
		SLASSERT(false, "wtf");
		return;
	}
	itor1->second->prepareSendNodeMessage(messageId, args.getSize());
	itor1->second->send(args.getContext(), args.getSize());
}

void Harbor::prepareSend(int32 nodeType, int32 nodeId, int32 messageId, int32 size){
	auto itor = _allNode[nodeType].find(nodeId);
	if (itor != _allNode[nodeType].end()){
		itor->second->prepareSendNodeMessage(messageId, size);
	}
}

void Harbor::send(int32 nodeType, int32 nodeId, const void* pContext, const int32 size){
	auto itor = _allNode[nodeType].find(nodeId);
	if (itor != _allNode[nodeType].end()){
		itor->second->send(pContext, size);
	}
}

void Harbor::broadcast(int32 nodeType, int32 messageId, const OArgs& args){
	auto itor = _allNode.find(nodeType);
	if (itor == _allNode.end()){
		//SLASSERT(false, "wtf");
		return;
	}

	for (auto node : itor->second){
		node.second->prepareSendNodeMessage(messageId, args.getSize());
		node.second->send(args.getContext(), args.getSize());
	}
}


void Harbor::broadcast(int32 messageId, const OArgs& args){
	for (auto nodeType : _allNode){
		for (auto node : nodeType.second){
			node.second->prepareSendNodeMessage(messageId, args.getSize());
			node.second->send(args.getContext(), args.getSize());
		}
	}
}

void Harbor::prepareBroadcast(int32 nodeType, const int32 messageId, const int32 size){
	auto itor = _allNode.find(nodeType);
	if (itor == _allNode.end() || _allNode[nodeType].empty()){
		return;
	}

	for (auto node : itor->second){
		node.second->prepareSendNodeMessage(messageId, size);
	}
}
void Harbor::broadcast(int32 nodeType, const void* context, const int32 size){
	auto itor = _allNode.find(nodeType);
	if (itor == _allNode.end() || _allNode[nodeType].empty()){
		return;
	}

	for (auto node : itor->second){
		node.second->send(context, size);
	}
}

void Harbor::onTime(sl::api::IKernel* pKernel, int64 timetick){
	startListening(pKernel);
}