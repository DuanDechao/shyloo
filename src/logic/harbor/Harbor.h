#ifndef SL_LOGIC_HARBOR_H
#define SL_LOGIC_HARBOR_H
#include "IHarbor.h"
#include "slikernel.h"
#include "NodeSession.h"
#include <unordered_map>

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


class Harbor
{
public:
	Harbor()
		:m_nodeType(0),
		 m_nodeId(0),
		 m_port(0)
	{}
	int32 getNodeType() const { return m_nodeType; }
	int32 getNodeId() const { return m_nodeId; }
	int32 getPort() const { return m_port; }

	void onNodeOpen(sl::api::IKernel* pKernel, int32 nodeType, int32 nodeId, const char* ip, int32 nodePort, NodeSession* session);
	void onNodeMessage(sl::api::IKernel* pKernel, int32 nodeType, int32 nodeId, const char* pszBuf, const int32 size);

private:
	int32 m_nodeType;
	int32 m_nodeId;
	int32 m_port;

	std::unordered_map<int32, std::unordered_map<int32, NodeSession*>> m_allNode;
	std::unordered_map<int32, std::list<INodeMessageHandler *>> m_allCB;

};
#endif