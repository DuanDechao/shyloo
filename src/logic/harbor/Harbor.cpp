#include "Harbor.h"
#include "slxml_reader.h"
#include "slargs.h"
#include "IDebugHelper.h"
std::unordered_map<std::string, int32> Harbor::s_nodeTypes;
std::unordered_map<int32, std::string> Harbor::s_nodeNames;
sl::api::ITcpSession* NodeSessionTcpServer::mallocTcpSession(sl::api::IKernel* pKernel){
	return NodeSession::create(_harbor);
}

void NodeSessionTcpServer::setListenPort(uint16 port){
	_harbor->setPort(port);
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
	_useIpc = false;
	_nodeId = 0;
	_port = 0;

	if(!initNodeTypes(pKernel)){
		SLASSERT(false, "wtf");
		return false;
	}

	const char* serverName = pKernel->getCmdArg("name");
	_nodeType = s_nodeTypes[serverName];

	if (pKernel->getCmdArg("harbor"))
		_port = sl::CStringUtils::StringAsInt32(pKernel->getCmdArg("harbor"));

	if(pKernel->getCmdArg("node_id"))
		_nodeId = sl::CStringUtils::StringAsInt32(pKernel->getCmdArg("node_id"));

    return true;
}

bool Harbor::launched(sl::api::IKernel * pKernel){
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

	startListening(pKernel);
	return true; 
}

bool Harbor::destory(sl::api::IKernel * pKernel){
	if (_pTcpServer)
		DEL _pTcpServer;
	_pTcpServer = nullptr;

	if (_pIpcServer)
		DEL _pIpcServer;
	_pIpcServer = nullptr;

	for(auto cbItor : _allCBPool){
		for(auto* cb : cbItor.second){
			DEL cb;
		}
	}

	DEL this;

	return true;
}

bool Harbor::initNodeTypes(sl::api::IKernel* pKernel){
	s_nodeTypes["gate"] = NodeType::GATE;
	s_nodeTypes["master"] = NodeType::MASTER;
	s_nodeTypes["slave"] = NodeType::SLAVE;
	s_nodeTypes["game"] = NodeType::LOGIC;
	s_nodeTypes["scene"] = NodeType::SCENE;
	s_nodeTypes["dbmgr"] = NodeType::DATABASE;
	s_nodeTypes["logger"] = NodeType::LOGGER;
	s_nodeNames[NodeType::GATE] = "gate";
	s_nodeNames[NodeType::MASTER] = "master";
	s_nodeNames[NodeType::SLAVE] = "slave";
	s_nodeNames[NodeType::LOGIC] = "game";
	s_nodeNames[NodeType::SCENE] = "scene";
	s_nodeNames[NodeType::DATABASE] = "dbmgr";
	s_nodeNames[NodeType::LOGGER] = "logger";
	return true;
}

bool Harbor::isNodeConnected(const int32 nodeType, const int32 nodeId){
	auto itor = _connectedNodes.find(nodeType);
	if(itor == _connectedNodes.end())
		return false;

	return itor->second & (uint64)(1 << nodeId);
}

int32 Harbor::allocNodeIdByType(const int32 nodeType){
	int32 nodeId = 0;
	auto itor = _lastAllocNodeId.find(nodeType);
	if(itor != _lastAllocNodeId.end())
		_lastAllocNodeId[nodeType]++;
	else
		_lastAllocNodeId[nodeType] = 1;

	auto nodeItor = _allNode.find(nodeType);
	if(nodeItor != _allNode.end()){
		while(nodeItor->second.find(_lastAllocNodeId[nodeType]) != nodeItor->second.end()){
			_lastAllocNodeId[nodeType]++;
		}
	}
	return _lastAllocNodeId[nodeType];
}

void Harbor::onNodeOpen(sl::api::IKernel* pKernel, int32 nodeType, int32 nodeId, const char* ip, int32 nodePort, NodeSession* session){
	_allNode[nodeType].insert(std::make_pair(nodeId, session));

	if(_connectedNodes.find(nodeType) == _connectedNodes.end())
		_connectedNodes[nodeType] = 0;

	_connectedNodes[nodeType] |= (uint64)(1 << nodeId);
	
	for (auto& listener : _listenerPool){
		listener->onOpen(pKernel, nodeType, nodeId, ip, nodePort);
	}
	INFO_LOG("node[%s:%d] from[%s:%d] opened\n", s_nodeNames[nodeType].c_str(), nodeId, ip, nodePort);
}

void Harbor::onNodeClose(sl::api::IKernel* pKernel, int32 nodeType, int32 nodeId){
	_allNode[nodeType].erase(nodeId);
	_connectedNodes[nodeType] &= ~((uint64)(1 << nodeId));
	
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

void Harbor::connect(const char* ip, const int32 port, const int32 nodeType, const int32 nodeId, bool isIpcTransfor){
	if (_useIpc && isIpcTransfor){
		sl::api::IKernel* pKernel = _pKernel;
		NodeSession* pSession = (NodeSession *)_pIpcServer->mallocTcpSession(_pKernel);
		SLASSERT(pSession, "wtf");
		pSession->setConnect(ip, port);
		pSession->setNodeInfo(nodeType, nodeId);

		int64 localId = (int64)_nodeId | ((int64)_nodeType << 32);
		int64 remoteId = (int64)nodeId | ((int64)nodeType << 32);
		if (!_pKernel->addIPCClient(pSession, localId, remoteId)){
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

		if (!_pKernel->startTcpClient(pSession, ip, port)){
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
	if (pKernel->startTcpServer(_pTcpServer, "0.0.0.0", _port)){
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

void Harbor::send(int32 nodeType, int32 nodeId, int32 messageId, const sl::OBStream& args){
	auto itor = _allNode.find(nodeType);
	if (itor == _allNode.end()){
		ERROR_LOG("send data but can't find node[%s:%d]", getNodeName(nodeType), nodeId);
		return;
	}

	auto itor1 = itor->second.find(nodeId);
	if (itor1 == itor->second.end()){
		ERROR_LOG("send data but can't find node[%s:%d]", getNodeName(nodeType), nodeId);
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

void Harbor::broadcast(int32 nodeType, int32 messageId, const sl::OBStream& args){
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

void Harbor::broadcast(int32 messageId, const OArgs& args, std::unordered_map<int32, std::set<int32>>& exclude){
	for (auto nodeType : _allNode){
		auto excludeNodes = exclude.find(nodeType.first);
		bool isNodeTypeExclude = (excludeNodes != exclude.end());	
		for (auto node : nodeType.second){
			if(isNodeTypeExclude && excludeNodes->second.find(node.first) != excludeNodes->second.end()){
				continue;
			}
			node.second->prepareSendNodeMessage(messageId, args.getSize());
			node.second->send(args.getContext(), args.getSize());
		}
	}
}

void Harbor::broadcast(int32 messageId, const sl::OBStream& args, std::unordered_map<int32, std::set<int32>>& exclude){
	for (auto nodeType : _allNode){
		auto excludeNodes = exclude.find(nodeType.first);
		bool isNodeTypeExclude = (excludeNodes != exclude.end());	
		for (auto node : nodeType.second){
			if(isNodeTypeExclude && excludeNodes->second.find(node.first) != excludeNodes->second.end()){
				continue;
			}
			node.second->prepareSendNodeMessage(messageId, args.getSize());
			node.second->send(args.getContext(), args.getSize());
		}
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

void Harbor::broadcast(int32 messageId, const sl::OBStream& args){
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
