#include "Harbor.h"

sl::api::ITcpSession* NodeSessionServer::mallocTcpSession(sl::api::IKernel* pKernel){
	return CREATE_POOL_OBJECT(NodeSession, m_pHarbor);
}

bool Harbor::initialize(sl::api::IKernel * pKernel){
	return true;
}

bool Harbor::launched(sl::api::IKernel * pKernel){
	START_TIMER(this, 0, 1, 500);
	return true; 
}

bool Harbor::destory(sl::api::IKernel * pKernel){
	return true;
}

void Harbor::onNodeOpen(sl::api::IKernel* pKernel, int32 nodeType, int32 nodeId, const char* ip, int32 nodePort, NodeSession* session){

	m_allNode[nodeType].insert(std::make_pair(nodeId, session));

	for (auto& listener : m_listenerPool){
		listener->onOpen(pKernel, nodeType, nodeId, ip, nodePort);
	}
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
	if (itor != m_allCBPool.end()){
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
	}
}

void Harbor::rgsNodeMessageHandler(int32 messageId, node_cb handler){
	m_allCBPool[messageId].push_back(NEW NodeCBMessageHandler(handler));
}

void Harbor::startListening(sl::api::IKernel* pKernel){
	if (m_port)
		pKernel->startTcpServer(m_pServer, "0.0.0.0", m_port, m_sendSize, m_recvSize);
}

void Harbor::onTime(sl::api::IKernel* pKernel, int64 timetick){
	startListening(pKernel);
}