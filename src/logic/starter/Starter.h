#ifndef SL_LOGIC_STARTER_H
#define SL_LOGIC_STARTER_H
#include "slikernel.h"
#include "slimodule.h"
#include "IHarbor.h"
#include "slsingleton.h"
#include <unordered_map>
class StartNodeTimer;
class Starter : public sl::api::IModule, public INodeListener, public sl::SLHolder<Starter>{
public:
	struct Execute{
		int32 type;
		int32 min;
		int32 max;
		int32 delay;
		StartNodeTimer* timer;
	};

	struct Node{
		bool online;
		int64 closeTick;
	};
	
	struct NodeGroup{
		int32 max;
		std::unordered_map<int32, Node> nodes;
	};

	virtual bool initialize(sl::api::IKernel * pKernel);
	virtual bool launched(sl::api::IKernel * pKernel);
	virtual bool destory(sl::api::IKernel * pKernel);

	void onNodeTimerStart(sl::api::IKernel * pKernel, int32 type, int64 tick);
	void onNodeTimer(sl::api::IKernel * pKernel, int32 type, int64 tick);
	void onNodeTimerEnd(sl::api::IKernel * pKernel, int32 type, int64 tick);

	virtual void onOpen(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId, const char* ip, const int32 port);
	virtual void onClose(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId);

private:
	void startNode(sl::api::IKernel* pKernel, int32 nodeType, int32 nodeId);

private:
	Starter*				_self;
	sl::api::IKernel*		_kernel;
	IHarbor*				_harbor;

	std::unordered_map<int32, Execute> _executes;
	std::unordered_map<int32, NodeGroup> _nodes;

	int64					_checkInterval;
	int64					_deadTime;
};
#endif