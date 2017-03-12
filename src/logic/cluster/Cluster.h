#ifndef SL_CLUSTER_H
#define SL_CLUSTER_H
#include "slimodule.h"
#include "slmulti_sys.h"
#include "IHarbor.h"
#include <set>
class Cluster : public sl::api::IModule{
public:
	virtual bool initialize(sl::api::IKernel * pKernel);
	virtual bool launched(sl::api::IKernel * pKernel);
	virtual bool destory(sl::api::IKernel * pKernel);

	void newNodeComing(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId, const OArgs& args);

private:
	IHarbor* _harbor;
	std::set<int64> _openNodes;
};
#endif