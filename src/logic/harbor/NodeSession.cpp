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
const int32 NODE_SHM_MESSAGE = 2;
const int32 NODE_MESSAGE = 3;

int32 NodeSession::onRecv(sl::api::IKernel* pKernel, const char* pContext, int dwLen){
	NodeHeader* header = (NodeHeader*)pContext;
	if (!m_bReady){
		if (header->messageId != NODE_REPORT)
			return 0;

		NodeReport* report =(NodeReport*)(pContext + sizeof(NodeHeader));
		m_nodeId = report->nodeId;
		m_nodeType = report->nodeType;
		
		m_pHarbor->onNodeOpen(pKernel, m_nodeType, m_nodeId, getRemoteIP(), report->port, this);

		m_useShm = true;
		m_shmQueue = Harbor::getInstance()->newShmQueue(m_bConnect, getShmKeyFile(pKernel, m_nodeType, m_nodeId).c_str(), 62355);

		m_bReady = true;
	}
	else{
		if (header->messageId == NODE_SHM_MESSAGE){
			int32 readLen = 0;
			m_shmQueue->getData(m_msgBuf.buf + m_msgBuf.currSize, 1024 - m_msgBuf.currSize, readLen);
			if (m_msgBuf.msgSize == 0 && readLen > 0){
				SLASSERT(m_msgBuf.currSize == 0, "wtf");
				m_msgBuf.msgSize = *(int32*)(m_msgBuf.buf + sizeof(int32));
			}
			m_msgBuf.currSize += readLen;

			if (m_msgBuf.msgSize > 0 && m_msgBuf.currSize >= m_msgBuf.msgSize){
				m_pHarbor->onNodeMessage(pKernel, m_nodeType, m_nodeId, m_msgBuf.buf + sizeof(NodeHeader), m_msgBuf.currSize - sizeof(NodeHeader));
				m_msgBuf.msgSize = 0;
				m_msgBuf.currSize = 0;
			}
		}

		if (header->messageId == NODE_MESSAGE)
			m_pHarbor->onNodeMessage(pKernel, m_nodeType, m_nodeId, pContext + sizeof(NodeHeader), dwLen - sizeof(NodeHeader));
	}
	return dwLen;
}

void NodeSession::send(const void* pContext, const int32 size){
	if (m_useShm){
		m_shmQueue->putData(pContext, size);
		char buf[sizeof(NodeHeader)];
		NodeHeader* header = (NodeHeader*)buf;
		header->messageId = NODE_SHM_MESSAGE;
		header->len = sizeof(NodeHeader);
		ITcpSession::send(buf, sizeof(buf));
	}
	else{
		ITcpSession::send(pContext, size);
	}
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
	ITcpSession::send(buf, sizeof(buf));
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

string NodeSession::getShmKeyFile(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId){
	char keyPath[128] = { 0 };
	if (m_bConnect)
		SafeSprintf(keyPath, sizeof(keyPath), "./shmkey/%d_%d_%d_%d.key", m_pHarbor->getNodeType(), m_pHarbor->getNodeId(), nodeType, nodeId);
	else
		SafeSprintf(keyPath, sizeof(keyPath), "./shmkey/%d_%d_%d_%d.key", nodeType, nodeId, m_pHarbor->getNodeType(), m_pHarbor->getNodeId());

#ifdef SL_OS_LINUX
	mode_t iMode = umask(0);
	FILE* pstFile = fopen(keyPath, "a+");
	umask(iMode);
#else
	FILE* pstFile = fopen(keyPath, "a+");
#endif

	if (pstFile){
		fclose(pstFile);
	}
	else{
		SLASSERT(false, "wtf");
	}

	char keyPathStr[128] = { 0 };
	SafeSprintf(keyPathStr, sizeof(keyPathStr), "TEXT(%s)", keyPath);
	return keyPathStr;
}