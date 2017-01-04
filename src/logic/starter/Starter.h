#ifndef SL_LOGIC_STARTER_H
#define SL_LOGIC_STARTER_H
#include "slikernel.h"
#include "slimodule.h"
#include "IHarbor.h"
#include <unordered_map>
class StartNodeTimer;
class Starter : public sl::api::IModule, public INodeListener
{
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

	static void onNodeTimerStart(sl::api::IKernel * pKernel, int32 type, int64 tick);
	static void onNodeTimer(sl::api::IKernel * pKernel, int32 type, int64 tick);
	static void onNodeTimerEnd(sl::api::IKernel * pKernel, int32 type, int64 tick);

	virtual void onOpen(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId, const char* ip, const int32 port);
	virtual void onClose(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId);

private:
	void startNode(sl::api::IKernel* pKernel, int32 nodeType, int32 nodeId);

private:
	static Starter*					s_self;
	static sl::api::IKernel*		s_kernel;
	static IHarbor*					s_harbor;

	static std::unordered_map<int32, Execute> s_executes;
	static std::unordered_map<int32, NodeGroup> s_nodes;

	static int64					s_checkInterval;
	static int64					s_deadTime;
};
#endif