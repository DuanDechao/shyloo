#ifndef SL_LOGIC_NODE_SESSION_H
#define SL_LOGIC_NODE_SESSION_H
#include "slikernel.h"
#include "slobjectpool.h"
using namespace sl;
class Harbor;
class NodeSession : public sl::api::ITcpSession
{
public:
	NodeSession()
		:m_pHarbor(nullptr),
		m_bReady(false),
		m_nodeType(0),
		m_nodeId(0)
	{}

	NodeSession(Harbor* harbor)
		:m_pHarbor(harbor),
		 m_bReady(false),
		 m_nodeType(0),
		 m_nodeId(0)
	{}

	virtual int32 onRecv(sl::api::IKernel* pKernel, const char* pContext, int dwLen);
	virtual void onConnected();
	virtual void onTerminate(){}

	void send(const void* pContext, const int32 size);
private:
	Harbor*		m_pHarbor;
	bool		m_bReady;
	int32		m_nodeType;
	int32		m_nodeId;
};
CREATE_OBJECT_POOL(NodeSession);
#endif