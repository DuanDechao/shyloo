 #include "NodeSession.h"
#include "Harbor.h"

struct NodeHeader{
	int32 messageId;
	int32 len;
};
struct NodeReport{
	int32 nodeType;
	int32 nodeId;
	int32 port;
};
const int32 NODE_REPORT = 1;
const int32 NODE_MESSAGE = 2;

int32 NodeSession::onRecv(sl::api::IKernel* pKernel, const char* pContext, int dwLen){
	NodeHeader* header = (NodeHeader*)pContext;
	if (!m_bReady){
		if (header->messageId != NODE_REPORT)
			return 0;

		NodeReport* report =(NodeReport*)(pContext + sizeof(NodeHeader));
		m_nodeId = report->nodeId;
		m_nodeType = report->nodeType;
		
		m_pHarbor->onNodeOpen(pKernel, m_nodeType, m_nodeId, getRemoteIP(), report->port, this);
		m_bReady = true;
	}
	else{
		if (header->messageId == NODE_REPORT)
			return 0;

		m_pHarbor->onNodeMessage(pKernel, m_nodeType, m_nodeId, pContext + sizeof(NodeHeader), dwLen - sizeof(NodeHeader));
	}
	return dwLen;
}

void NodeSession::send(const void* pContext, const int32 size){
	ITcpSession::send(pContext, size);
}

void NodeSession::onConnected(sl::api::IKernel* pKernel){
	char buf[sizeof(NodeHeader)+sizeof(NodeReport)];
	NodeHeader* header = (NodeHeader*)buf;
	header->messageId = NODE_REPORT;
	header->len = sizeof(NodeHeader)+sizeof(NodeReport);
	NodeReport* report = (NodeReport*)(buf + sizeof(NodeHeader));
	report->nodeType = m_pHarbor->getNodeType();
	report->nodeId = m_pHarbor->getNodeId();
	report->port = m_pHarbor->getPort();
	send(buf, sizeof(buf));
}

void NodeSession::onDisconnect(sl::api::IKernel* pKernel){
	if (m_bReady){
		m_bReady = false;
		m_pHarbor->onNodeClose(pKernel, m_nodeType, m_nodeId);
	}
	if (m_bConnect){
		START_TIMER(this, 0, TIMER_BEAT_FOREVER, RECONNECT_INTERVAL);
	}
	else{
		RELEASE_POOL_OBJECT(NodeSession, this);
	}
}

void NodeSession::setConnect(const char* ip, const int32 port){
	m_ip = ip;
	m_port = port;
	m_bConnect = true;
}

void NodeSession::prepareSendNodeMessage(const int32 messageId, const int32 size){
	char buf[sizeof(NodeHeader)+sizeof(int32)];
	NodeHeader* header = (NodeHeader*)buf;
	header->messageId = NODE_MESSAGE;
	header->len = sizeof(NodeHeader)+sizeof(int32)+size;
	*(int32*)(buf + sizeof(NodeHeader)) = messageId;
	send(buf, sizeof(buf));
}

void NodeSession::onTime(sl::api::IKernel* pKernel, int64 timetick){
	SLASSERT(m_bConnect && !m_bReady, "wtf");
	TRACE_LOG("reconnect [%s:%d] ...!", m_ip.c_str(), m_port);
	if (pKernel->startTcpClient(this, m_ip.c_str(), m_port, m_pHarbor->getSendSize(), m_pHarbor->getRecvSize())){
		pKernel->killTimer(this);
	}
}