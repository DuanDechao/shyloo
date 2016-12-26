#include "Harbor.h"

sl::api::ITcpSession* NodeSessionServer::mallocTcpSession(sl::api::IKernel* pKernel){
	return CREATE_POOL_OBJECT(NodeSession, m_pHarbor);
}

void Harbor::onNodeOpen(sl::api::IKernel* pKernel, int32 nodeType, int32 nodeId, const char* ip, int32 nodePort, NodeSession* session){

	/*auto itor = m_allNode.find(nodeType);
	if (itor != m_allNode.end()){

	}*/

	m_allNode[nodeType].insert(std::make_pair(nodeId, session));
}

void Harbor::onNodeMessage(sl::api::IKernel* pKernel, int32 nodeType, int32 nodeId, const char* pszBuf, const int32 size){
	SLASSERT(size >= sizeof(int32), "invalid node message context!");
	int32 messageId = *(int32*)pszBuf;
	auto itor = m_allCBPool.find(messageId);
	if (itor != m_allCBPool.end()){
		SLASSERT(false, "have no messageId %d", messageId);
		return;
	}
	for (auto* handler : m_allCBPool[messageId]){
		handler->DealNodeMessage(pKernel, nodeType, nodeId, pszBuf + sizeof(int32), size - sizeof(int32));
	}
}

void Harbor::rsgNodeMessageHandler(int32 messageId, node_cb handler){
	m_allCBPool[messageId].push_back(NEW NodeCBMessageHandler(handler));
}