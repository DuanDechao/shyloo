#include "Harbor.h"
#include "slxml_reader.h"
#include "slargs.h"

sl::api::ITcpSession* NodeSessionTcpServer::mallocTcpSession(sl::api::IKernel* pKernel){
	return NodeSession::create(_harbor);
}

sl::api::ITcpSession* NodeSessionIpcServer::mallocTcpSession(sl::api::IKernel* pKernel){
	return NodeSession::create(_harbor);
}

class NodeCBMessageHandler : public INodeMessageHandler{
public:
	NodeCBMessageHandler(const NodeCB cb) : _cb(cb){}
	virtual ~NodeCBMessageHandler() {}

	virtual void DealNodeMessage(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId, const char* pContext, const int32 size){
		_cb(pKernel, nodeType, nodeId, sl::OBStream(pContext, size));
	}
private:
	NodeCB		_cb;
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

	const sl::xml::ISLXmlNode& harbor_conf = server_conf.root()["harbor"][0];
	_sendSize = harbor_conf.getAttributeInt32("send");
	_recvSize = harbor_conf.getAttributeInt32("recv");
	_useIpc = harbor_conf.getAttributeBoolean("useIpc");
	const sl::xml::ISLXmlNode& nodes = harbor_conf["node"];
	for (int32 i = 0; i < nodes.count(); i++){
		int32 type = nodes[i].getAttributeInt32("type");
		_nodeNames[type] = nodes[i].getAttributeString("name");
	}

	if (harbor_conf.subNodeExist("pipe")){
		const sl::xml::ISLXmlNode& pipes = harbor_conf["pipe"];
		for (int32 i = 0; i < pipes.count(); i++){
			int64 type1 = pipes[i].getAttributeInt64("node1");
			int64 type2 = pipes[i].getAttributeInt64("node2");

			_nodeSize[(type1 | (type2 << 32))] = pipes[i].getAttributeInt32("size");
		}
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

	_pTcpServer = NEW NodeSessionTcpServer(this);
	if (!_pTcpServer){
		SLASSERT(false, "wtf");
		return false;
	}

	_pIpcServer = NEW NodeSessionIpcServer(this);
	if (!_pIpcServer){
		SLASSERT(false, "wtf");
		return false;
	}

	START_TIMER(this, 0, 1, 500);
	return true; 
}

bool Harbor::destory(sl::api::IKernel * pKernel){
	if (_pTcpServer)
		DEL _pTcpServer;
	_pTcpServer = nullptr;

	if (_pIpcServer)
		DEL _pIpcServer;
	_pIpcServer = nullptr;

	DEL this;

	return true;
}

void Harbor::onNodeOpen(sl::api::IKernel* pKernel, int32 nodeType, int32 nodeId, const char* ip, int32 nodePort, NodeSession* session){
	int64 idx = (int64)_nodeType | (((int64)nodeType) << 32);
	auto itor = _nodeSize.find(idx);
	if (itor != _nodeSize.end()){
		session->adjustSendBuffSize(itor->second);
		TRACE_LOG("node [%s:%d] adjust send buff %d", _nodeNames[nodeType].c_str(), nodeId, itor->second);
	}

	idx = (int64)nodeType | (((int64)_nodeType) << 32);
	itor = _nodeSize.find(idx);
	if (itor != _nodeSize.end()){
		session->adjustRecvBuffSize(itor->second);
		TRACE_LOG("node [%s:%d] adjust recv buff %d", _nodeNames[nodeType].c_str(), nodeId, itor->second);
	}

	_allNode[nodeType].insert(std::make_pair(nodeId, session));
	for (auto& listener : _listenerPool){
		listener->onOpen(pKernel, nodeType, nodeId, ip, nodePort);
	}
	TRACE_LOG("node[%s:%d] from[%s:%d] opened", _nodeNames[nodeType].c_str(), _nodeId, ip, nodePort);
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

void Harbor::connect(const char* ip, const int32 port, const int32 nodeType, const int32 nodeId, bool ipcTransfor){
	if (_useIpc && ipcTransfor){
		sl::api::IKernel* pKernel = _pKernel;
		NodeSession* pSession = (NodeSession *)_pIpcServer->mallocTcpSession(_pKernel);
		SLASSERT(pSession, "wtf");
		pSession->setConnect(ip, port);
		pSession->setNodeInfo(nodeType, nodeId);

		int64 localId = (int64)_nodeId | ((int64)_nodeType << 32);
		int64 remoteId = (int64)nodeId | ((int64)nodeType << 32);
		if (!_pKernel->addIPCClient(pSession, localId, remoteId, _sendSize, _recvSize)){
			START_TIMER(pSession, 0, TIMER_BEAT_FOREVER, RECONNECT_INTERVAL);
			ERROR_LOG("connect [%s:%d] failed!", ip, port);
		}
		else{
			TRACE_LOG("connect [%s:%d] success!", ip, port);
		}
	}
	else{
		sl::api::IKernel * pKernel = _pKernel;
		NodeSession* pSession = (NodeSession *)_pTcpServer->mallocTcpSession(_pKernel);
		SLASSERT(pSession, "wtf");
		pSession->setConnect(ip, port);
		pSession->setNodeInfo(nodeType, nodeId);

		if (!_pKernel->startTcpClient(pSession, ip, port, _sendSize, _recvSize)){
			START_TIMER(pSession, 0, TIMER_BEAT_FOREVER, RECONNECT_INTERVAL);
			ERROR_LOG("connect [%s:%d] failed!", ip, port);
		}
		else{
			TRACE_LOG("connect [%s:%d] success!", ip, port);
		}
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

	if (pKernel->startTcpServer(_pTcpServer, "0.0.0.0", _port, _sendSize, _recvSize)){
		TRACE_LOG("start tcp server[%s:%d] success", "0.0.0.0", _port);
	}
	else{
		TRACE_LOG("start tcp server[%s:%d] failed", "0.0.0.0", _port);
	}

	if (_useIpc){
		int64 serverIpcId = (int64)_nodeId | ((int64)_nodeType << 32);
		if (pKernel->addIPCServer(_pIpcServer, serverIpcId)){
			TRACE_LOG("start ipc server[%s:%d] success", "0.0.0.0", _port);
		}
		else{
			TRACE_LOG("start ipc server[%s:%d] failed", "0.0.0.0", _port);
		}
	}
}

void Harbor::send(int32 nodeType, int32 nodeId, int32 messageId, const OArgs& args){
	auto itor = _allNode.find(nodeType);
	if (itor == _allNode.end()){
		ECHO_ERROR("send data but can't find node[%s]", getNodeName(nodeType));
		SLASSERT(false, "wtf");
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