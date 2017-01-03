#ifndef SL_MASTER_H
#define SL_MASTER_H
#include "slmulti_sys.h"
#include "slimodule.h"
#include <unordered_map>
#include "IHarbor.h"
class OArgs;
class Master : public sl::api::IModule, public INodeListener
{
public:
	struct Node{
		int32 nodeType;
		int32 nodeId;
		char ip[256];
		int32 port;
	};
	virtual bool initialize(sl::api::IKernel * pKernel);
	virtual bool launched(sl::api::IKernel * pKernel);
	virtual bool destory(sl::api::IKernel * pKernel);

	virtual void onOpen(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId, const char* ip, const int32 port);
	void sendNodeComing(const int32 nodeType, const int32 nodeId, const int32 newNodeType, const int32 newNodeId, const char* ip, const int32 port);

private:
	static IHarbor* s_harbor;
	static std::unordered_map<int64, Node> s_nodes;
};
#endif