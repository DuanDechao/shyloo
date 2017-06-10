#ifndef __SL_MONITOR_SHUTDOWN_H__
#define __SL_MONITOR_SHUTDOWN_H__
#include "slmulti_sys.h"
#include "slimodule.h"
#include "IHarbor.h"
#include "slsingleton.h"
#include <vector>
#include <unordered_map>
#include <unordered_set>

class IHarbor;
class IMonitor;
class IEventEngine;
class ShutDown : public sl::api::IModule, public INodeListener, public sl::SLHolder<ShutDown>{
	enum SelectType{
		RANDOM = 0,
		ALL,
	};
	enum OptType{
		NOTIFY = 0,
		CLOSE,
	};
	struct NodeInfo{
		int32 nodeType;
		SelectType selectType;
	};
	struct ShutDownStep{
		OptType optType;
		std::vector<NodeInfo> nodes;
	};

public:
	virtual bool initialize(sl::api::IKernel * pKernel);
	virtual bool launched(sl::api::IKernel * pKernel);
	virtual bool destory(sl::api::IKernel * pKernel);

	bool loadShutdownConfig(sl::api::IKernel* pKernel);

	virtual void onOpen(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId, const char* ip, const int32 port);
	virtual void onClose(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId);

	void shutdownServerReq(sl::api::IKernel* pKernel, const int64 id, const sl::OBStream& args);

	void onMasterAskShutdown(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId, const OArgs& args);
	void onClusterShutdownAck(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId, const OArgs& args);

	void onClusterShutdownComplete(sl::api::IKernel* pKernel, const void* context, const int32 size);

private:
	void terminate(sl::api::IKernel* pKernel);
	void startNewStep(sl::api::IKernel* pKernel);
	void checkNextStep(sl::api::IKernel* pKernel);
	void sendNewStep(sl::api::IKernel* pKernel);

private:
	sl::api::IKernel*	_kernel;
	ShutDown*			_self;
	IHarbor*			_harbor;
	IMonitor*			_monitor;
	IEventEngine*		_eventEngine;

	bool				_shutdowning;
	std::unordered_map<int32, unordered_set<int32>> _allNodes;
	std::unordered_map<int32, unordered_set<int32>> _currOptNodes;
	std::vector<ShutDownStep> _allSteps;
	int32				_currStep;
	

};
#endif