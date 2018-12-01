#ifndef SL_NODE_H
#define SL_NODE_H
#include "slmulti_sys.h"
#include "slimodule.h"
#include <unordered_map>
#include <unordered_set>
#include "IHarbor.h"
class OArgs;
class Node : public sl::api::IModule, public INodeListener{
public:
	virtual bool initialize(sl::api::IKernel * pKernel);
	virtual bool launched(sl::api::IKernel * pKernel);
	virtual bool destory(sl::api::IKernel * pKernel);

	virtual void onOpen(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId, const char* ip, const int32 port);
	virtual void onClose(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId);
	
	void newNodeComing(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId, const sl::OBStream& args);

private:
	void registerToSlave(int32 slaveNodeId);
	bool isSameMachine(const char* localIp, const char* remoteIp);

private:
	Node*				_self;
	std::string			_masterIp;
	int32				_masterPort;
	std::set<int64>		_openNodes;
};
#endif
