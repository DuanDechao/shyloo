#include "Harbor.h"
#include "slxml_reader.h"


sl::api::ITcpSession* NodeSessionServer::mallocTcpSession(sl::api::IKernel* pKernel){
	return CREATE_POOL_OBJECT(NodeSession, m_pHarbor);
}

bool Harbor::initialize(sl::api::IKernel * pKernel){
	m_pKernel = pKernel;
	return true;
}

bool Harbor::launched(sl::api::IKernel * pKernel){
	XmlReader server_conf;
	if (!server_conf.loadXml(pKernel->getCoreFile())){
		SLASSERT(false, "can not load core file %s", pKernel->getCoreFile());
		return false;
	}
	m_sendSize = server_conf.root()["harbor"][0].getAttributeInt32("send");
	m_recvSize = server_conf.root()["harbor"][0].getAttributeInt32("recv");
	const sl::xml::ISLXmlNode& nodes = server_conf.root()["harbor"][0]["node"];
	for (int32 i = 0; i < nodes.count(); i++){
		int32 type = nodes[i].getAttributeInt32("type");
		m_nodeNames[type] = nodes[i].getAttributeString("name");
	}

	XmlReader conf;
	if (!conf.loadXml(pKernel->getConfigFile())){
		SLASSERT(false, "can not load config file %s", pKernel->getConfigFile());
		return false;
	}
	m_nodeType = conf.root()["harbor"][0].getAttributeInt32("type");
	m_nodeId = sl::CStringUtils::StringAsInt32(pKernel->getCmdArg("node_id"));
	
	if (pKernel->getCmdArg("harbor"))
		m_port = sl::CStringUtils::StringAsInt32(pKernel->getCmdArg("harbor"));
	else
		m_port = 0;

	m_pServer = NEW NodeSessionServer(this);
	if (!m_pServer){
		SLASSERT(false, "wtf");
		return false;
	}

	START_TIMER(this, 0, 1, 500);
	return true; 
}

bool Harbor::destory(sl::api::IKernel * pKernel){
	if (m_pServer)
		DEL m_pServer;
	m_pServer = nullptr;

	DEL this;

	return true;
}

void Harbor::onNodeOpen(sl::api::IKernel* pKernel, int32 nodeType, int32 nodeId, const char* ip, int32 nodePort, NodeSession* session){

	m_allNode[nodeType].insert(std::make_pair(nodeId, session));
	for (auto& listener : m_listenerPool){
		listener->onOpen(pKernel, nodeType, nodeId, ip, nodePort);
	}
	ECHO_TRACE("node[%s:%d] from[%s:%d] opened", m_nodeNames[nodeType].c_str(), m_nodeId, ip, nodePort);
}

void Harbor::onNodeClose(sl::api::IKernel* pKernel, int32 nodeType, int32 nodeId){
	m_allNode[nodeType].erase(nodeId);
	
	for (auto& listener : m_listenerPool){
		listener->onClose(pKernel, nodeType, nodeId);
	}
}

void Harbor::onNodeMessage(sl::api::IKernel* pKernel, int32 nodeType, int32 nodeId, const char* pszBuf, const int32 size){
	SLASSERT(size >= sizeof(int32), "invalid node message context!");
	int32 messageId = *(int32*)pszBuf;
	auto itor = m_allCBPool.find(messageId);
	if (itor == m_allCBPool.end()){
		SLASSERT(false, "have no messageId %d", messageId);
		return;
	}
	for (auto& handler : m_allCBPool[messageId]){
		handler->DealNodeMessage(pKernel, nodeType, nodeId, pszBuf + sizeof(int32), size - sizeof(int32));
	}
}

void Harbor::addNodeListener(INodeListener* pNodeListener){
	m_listenerPool.push_back(pNodeListener);
}

void Harbor::connect(const char* ip, const int32 port){
	sl::api::IKernel * pKernel = m_pKernel;
	NodeSession* pSession = (NodeSession *)m_pServer->mallocTcpSession(m_pKernel);
	SLASSERT(pSession, "wtf");
	pSession->setConnect(ip, port);
	if (!m_pKernel->startTcpClient(pSession, ip, port, m_sendSize, m_recvSize)){
		START_TIMER(pSession, 0, TIMER_BEAT_FOREVER, RECONNECT_INTERVAL);
		ECHO_TRACE("connect [%s:%d] failed!", ip, port);
	}
	else{
		ECHO_TRACE("connect [%s:%d] success!", ip, port);
	}
}

void Harbor::rgsNodeMessageHandler(int32 messageId, node_args_cb handler){
	m_allCBPool[messageId].push_back(NEW NodeArgsCBMessageHandler(handler));
}

void Harbor::startListening(sl::api::IKernel* pKernel){
	if (!m_port)
		return;

	if (pKernel->startTcpServer(m_pServer, "0.0.0.0", m_port, m_sendSize, m_recvSize)){
		ECHO_TRACE("start server[%s:%d] success", "0.0.0.0", m_port);
	}
	else{
		ECHO_TRACE("start server[%s:%d] failed", "0.0.0.0", m_port);
	}
}

void Harbor::send(int32 nodeType, int32 nodeId, int32 messageId, const OArgs& args){
	auto itor = m_allNode.find(nodeType);
	if (itor == m_allNode.end()){
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

void Harbor::onTime(sl::api::IKernel* pKernel, int64 timetick){
	startListening(pKernel);
}