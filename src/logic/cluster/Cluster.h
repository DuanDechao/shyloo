#ifndef SL_CLUSTER_H
#define SL_CLUSTER_H
#include "slimodule.h"
#include "slmulti_sys.h"
#include "IHarbor.h"
#include <set>
#include "slstring.h"
#include "ICluster.h"
class Cluster : public ICluster, public sl::api::ITimer{
public:
	virtual bool initialize(sl::api::IKernel * pKernel);
	virtual bool launched(sl::api::IKernel * pKernel);
	virtual bool destory(sl::api::IKernel * pKernel);

	void newNodeComing(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId, const sl::OBStream& args);
	void onClusterIsReady(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId, const sl::OBStream& args);

	virtual void onStart(sl::api::IKernel* pKernel, int64 timetick){}
	virtual void onTime(sl::api::IKernel* pKernel, int64 timetick);
	virtual void onTerminate(sl::api::IKernel* pKernel, bool beForced, int64 timetick){}
	virtual void onPause(sl::api::IKernel* pKernel, int64 timetick) {}
	virtual void onResume(sl::api::IKernel* pKernel, int64 timetick) {}
	virtual bool clusterIsReady() {return _clusterReady;}

private:
	bool isSameDeivce(const char* localIp, const char* remoteIp);

private:
	IHarbor*			_harbor;
	std::set<int64>		_openNodes;
	sl::SLString<128>	_masterIp;
	int32				_masterPort;
	bool				_clusterReady;
};
#endif
