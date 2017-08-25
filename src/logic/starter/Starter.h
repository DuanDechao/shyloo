#ifndef SL_LOGIC_STARTER_H
#define SL_LOGIC_STARTER_H
#include "slikernel.h"
#include "IStarter.h"
#include "IHarbor.h"
#include "slsingleton.h"
#include <unordered_map>

class StartNodeTimer;
class IEventEngine;
class ICapacitySubscriber;
class Starter : public IStarter, public INodeListener, public sl::api::ITimer, public sl::SLHolder<Starter>{
	enum STATE{
		STAT_NOT_START = 0,
		STAT_STARTING,
		STAT_STARTED
	};
public:
	struct Execute{
		int32 type;
		int32 min;
		int32 max;
		int32 rate;
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

	virtual void setStrategy(IStartStrategy* strategy) { _strategy = strategy; }

	void onNodeTimerStart(sl::api::IKernel * pKernel, int32 type, int64 tick);
	void onNodeTimer(sl::api::IKernel * pKernel, int32 type, int64 tick);
	void onNodeTimerEnd(sl::api::IKernel * pKernel, int32 type, int64 tick);

	virtual void onOpen(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId, const char* ip, const int32 port);
	virtual void onClose(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId);


	virtual void onStart(sl::api::IKernel* pKernel, int64 timetick){}
	virtual void onTime(sl::api::IKernel* pKernel, int64 timetick);
	virtual void onTerminate(sl::api::IKernel* pKernel, bool beForced, int64 timetick){}
	virtual void onPause(sl::api::IKernel* pKernel, int64 timetick) {}
	virtual void onResume(sl::api::IKernel* pKernel, int64 timetick){}

	void preShutDown(sl::api::IKernel* pKernel, const void* context, const int32 size);

private:
	void startNode(sl::api::IKernel* pKernel, int32 nodeType, int32 nodeId);
	void startServer(sl::api::IKernel* pKernel);

	bool testStarted(sl::api::IKernel* pKernel);

private:
	Starter*				_self;
	sl::api::IKernel*		_kernel;
	IHarbor*				_harbor;
	IEventEngine*			_eventEngine;
	IStartStrategy*			_strategy;
	ICapacitySubscriber*	_capacitySubscriber;

	std::unordered_map<int32, Execute> _executes;
	std::unordered_map<int32, NodeGroup> _nodes;

	int64					_checkInterval;
	int64					_deadTime;

    int8					_status;
};
#endif