#ifndef SL_LOGIC_HARBOR_H
#define SL_LOGIC_HARBOR_H
#include "IHarbor.h"
#include "slikernel.h"
#include "NodeSession.h"
#include <unordered_map>
#include <string>
class Harbor;
class NodeSessionServer : public sl::api::ITcpServer
{
public:
	NodeSessionServer(Harbor* pHarbor) :m_pHarbor(pHarbor){}
	virtual ~NodeSessionServer(){}
	virtual sl::api::ITcpSession* mallocTcpSession(sl::api::IKernel* pKernel);

private:
	Harbor* m_pHarbor;
};


class INodeMessageHandler
{
public:
	virtual ~INodeMessageHandler(){}
	virtual void DealNodeMessage(sl::api::IKernel*, const int32, const int32, const char* pContext, const int32 size) = 0;
};

class NodeCBMessageHandler : public INodeMessageHandler
{
public:
	NodeCBMessageHandler(const node_cb cb) : m_cb(cb){}
	virtual ~NodeCBMessageHandler() {}

	virtual void DealNodeMessage(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId, const char* pContext, const int32 size){
		m_cb(pKernel, nodeType, nodeId, pContext, size);
	}
private:
	node_cb		m_cb;
};


class NodeArgsCBMessageHandler : public INodeMessageHandler
{
public:
	NodeArgsCBMessageHandler(const node_args_cb cb) : m_cb(cb){}
	virtual ~NodeArgsCBMessageHandler() {}

	virtual void DealNodeMessage(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId, const char* pContext, const int32 size){
		OArgs args(pContext, size);
		m_cb(pKernel, nodeType, nodeId, args);
	}
private:
	node_args_cb		m_cb;
};


class Harbor: public IHarbor, public sl::api::ITimer
{
public:
	virtual bool initialize(sl::api::IKernel * pKernel);
	virtual bool launched(sl::api::IKernel * pKernel);
	virtual bool destory(sl::api::IKernel * pKernel);

	int32 getNodeType() const { return m_nodeType; }
	int32 getNodeId() const { return m_nodeId; }
	int32 getPort() const { return m_port; }
	int32 getSendSize() const { return m_sendSize; }
	int32 getRecvSize() const { return m_recvSize; }

	virtual void onStart(sl::api::IKernel* pKernel, int64 timetick){}
	virtual void onTime(sl::api::IKernel* pKernel, int64 timetick);
	virtual void onTerminate(sl::api::IKernel* pKernel, int64 timetick){}
	virtual void onPause(sl::api::IKernel* pKernel, int64 timetick) {}
	virtual void onResume(sl::api::IKernel* pKernel, int64 timetick) {}

	virtual void onNodeOpen(sl::api::IKernel* pKernel, int32 nodeType, int32 nodeId, const char* ip, int32 nodePort, NodeSession* session);
	virtual void onNodeClose(sl::api::IKernel* pKernel, int32 nodeType, int32 nodeId);
	virtual void onNodeMessage(sl::api::IKernel* pKernel, int32 nodeType, int32 nodeId, const char* pszBuf, const int32 size);
	virtual void addNodeListener(INodeListener* pNodeListener);
	
	virtual void prepareSend(int32 nodeType, int32 nodeId, int32 messageId, int32 size);
	virtual void send(int32 nodeType, int32 nodeId, const void* pContext, const int32 size);
	virtual void send(int32 nodeType, int32 nodeId, int32 messageId, const OArgs& args);
	virtual void connect(const char* ip, const int32 port);

	virtual void rgsNodeMessageHandler(int32 messageId, node_args_cb handler);
	virtual void rgsNodeMessageHandler(int32 messageId, node_cb handler);
	virtual void startListening(sl::api::IKernel* pKernel);

private:
	sl::api::IKernel*	m_pKernel;
	NodeSessionServer*	m_pServer;
	int32				m_nodeType;
	int32				m_nodeId;
	int32				m_port;
	int32				m_recvSize;
	int32				m_sendSize;
	std::unordered_map<int32, std::unordered_map<int32, NodeSession*>> m_allNode;
	std::unordered_map<int32, std::list<INodeMessageHandler *>> m_allCBPool;
	std::unordered_map<int32, std::string> m_nodeNames;
	std::list<INodeListener*>	m_listenerPool;

};
#endif