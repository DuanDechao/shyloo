#ifndef SL_LOGIC_HARBOR_H
#define SL_LOGIC_HARBOR_H
#include "slikernel.h"
#include "NodeSession.h"
#include <unordered_map>
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
	void onNodeMessage(sl::api::IKernel* pKernel);
private:
	int32 m_nodeType;
	int32 m_nodeId;
	int32 m_port;

	std::unordered_map<int32, std::unordered_map<int32, NodeSession*>> m_allNode;

};
#endif