#include "NodeSession.h"
#include "Harbor.h"
#include "IDebugHelper.h"
struct NodeHeader{
	int32 messageId;
	int32 len;
};
struct NodeReport{
	int32 nodeType;
	int32 nodeId;
	int32 port;
	bool askAllocNodeId;
};

struct NodeAllocId{
	int32 newNodeId;
};
const int32 NODE_REPORT = 1;
const int32 NODE_ALLOCID = 2;
const int32 NODE_MESSAGE = 3;

sl::SLPool<NodeSession> NodeSession::s_pool;
int32 NodeSession::onRecv(sl::api::IKernel* pKernel, const char* pContext, int dwLen){
	NodeHeader* header = (NodeHeader*)pContext;
	if (!_ready){
		if (header->messageId != NODE_REPORT)
			return 0;

		NodeReport* report =(NodeReport*)(pContext + sizeof(NodeHeader));
		_nodeId = report->nodeId;
		_nodeType = report->nodeType;
		if(_nodeId <= 0){
			_nodeId = _harbor->allocNodeIdByType(_nodeType);
			sendAllocNewNodeId(_nodeId);
		}
		_harbor->onNodeOpen(pKernel, _nodeType, _nodeId, getRemoteIP(), report->port, this);

		_ready = true;
	}
	else{
		if (header->messageId == NODE_ALLOCID){
			NodeAllocId* allocId =(NodeAllocId*)(pContext + sizeof(NodeHeader));
			_harbor->setLocalNodeId(allocId->newNodeId);
		}
		if (header->messageId == NODE_MESSAGE)
			_harbor->onNodeMessage(pKernel, _nodeType, _nodeId, pContext + sizeof(NodeHeader), dwLen - sizeof(NodeHeader));
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
	report->nodeType = _harbor->getNodeType();
	report->nodeId = _harbor->getNodeId();
	report->port = _harbor->getPort();
	send(buf, sizeof(buf));
}

void NodeSession::sendAllocNewNodeId(const int32 newNodeId){
	char buf[sizeof(NodeHeader)+sizeof(NodeAllocId)];
	NodeHeader* header = (NodeHeader*)buf;
	header->messageId = NODE_ALLOCID;
	header->len = sizeof(NodeHeader)+sizeof(NodeAllocId);
	NodeAllocId* allocId = (NodeAllocId*)(buf + sizeof(NodeHeader));
	allocId->newNodeId = newNodeId;
	send(buf, sizeof(buf));
}

void NodeSession::onDisconnect(sl::api::IKernel* pKernel){
	if (_ready){
		_ready = false;
		_harbor->onNodeClose(pKernel, _nodeType, _nodeId);
	}
	if (_connect){
		START_TIMER(this, 0, TIMER_BEAT_FOREVER, RECONNECT_INTERVAL);
	}
	else{
		release();
	}
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
	SLASSERT(_connect && !_ready, "wtf");
	TRACE_LOG("reconnect [%s:%d] ...!", _ip.c_str(), _port);

	if (_ipcTransfor && _harbor->useIpc()){
		int64 localId = (int64)_harbor->getNodeId() | ((int64)_harbor->getNodeType() << 32);
		int64 remoteId = (int64)_nodeId | ((int64)_nodeType << 32);
		if (pKernel->addIPCClient(this, localId, remoteId))
			pKernel->killTimer(this);
	}
	else{
		if (pKernel->startTcpClient(this, _ip.c_str(), _port))
			pKernel->killTimer(this);
	}
}
