#include "Harbor.h"

void Harbor::onNodeOpen(sl::api::IKernel* pKernel, int32 nodeType, int32 nodeId, const char* ip, int32 nodePort, NodeSession* session){

	/*auto itor = m_allNode.find(nodeType);
	if (itor != m_allNode.end()){

	}*/

	m_allNode[nodeType].insert(std::make_pair(nodeId, session));
}

void Harbor::onNodeMessage(sl::api::IKernel* pKernel, int32 nodeType, int32 nodeId, const char* pszBuf, const int32 size){
	auto itor = m_allCB.find()
}