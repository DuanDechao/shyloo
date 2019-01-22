#ifndef __SL_CLUSTER_H__
#define __SL_CLUSTER_H__
#include "slmulti_sys.h"
#include "slsingleton.h"
#include "ICluster.h"
#include <unordered_map>
#include "IHarbor.h"
class StartUpHandler;
class IHarbor;
class Cluster : public ICluster, public INodeListener, public sl::api::ITimer, public sl::SLHolder<Cluster>{
	struct NodeInfo{
		float load;
		int32 state;
	};

	struct MachineInfo{
		float load;
	};
public:
	virtual bool initialize(sl::api::IKernel * pKernel);
	virtual bool launched(sl::api::IKernel * pKernel);
	virtual bool destory(sl::api::IKernel * pKernel);
	
	virtual void onStart(sl::api::IKernel* pKernel, int64 timetick){}
	virtual void onTime(sl::api::IKernel* pKernel, int64 timetick);
	virtual void onTerminate(sl::api::IKernel* pKernel, bool beForced, int64 timetick){}
	virtual void onPause(sl::api::IKernel* pKernel, int64 timetick) {}
	virtual void onResume(sl::api::IKernel* pKernel, int64 timetick) {}
	
	virtual void onOpen(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId, const char* ip, const int32 port);
	virtual void onClose(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId);

	virtual void addServerProcessHandler(IServerProcessHandler* handler);
	virtual int32 getBestNodeId(int32 nodeType);
	virtual int32 getNodeState(int32 nodeType, int32 nodeId);

	bool serverReady();
	bool serverReadyForLogin();
	bool serverReadyForShutDown();
	bool serverShutDown();
	
	void nodeInfoReport(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId, const sl::OBStream& args);

	sl::api::IKernel* getKernel() const {return _kernel;}

private:
	void updateLoad(sl::api::IKernel* pKernel, int64 timetick, int64 passTime);
	void onUpdateNodeInfo();
	void bestNodeId(int32 nodeType);
	int32 findFreeNodeId(int32 nodeType);
	void checkServerState();

private:
	//当前节点的数据
	Cluster*								_self;
	sl::api::IKernel*						_kernel;
	std::vector<IServerProcessHandler*>		_handlers;
	NodeInfo								_currNodeInfo;
	uint64									_lastUpdateTime;

	//master Node 的数据
	std::unordered_map<int32, std::unordered_map<int32, NodeInfo>> _allNodeInfo;
	std::unordered_map<int32, int32> _bestNodeId;
	std::unordered_map<int32, MachineInfo> _allMachineInfo;
};
#endif
