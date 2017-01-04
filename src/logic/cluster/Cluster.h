#ifndef SL_CLUSTER_H
#define SL_CLUSTER_H
#include "slimodule.h"
#include "slmulti_sys.h"
#include "IHarbor.h"
#include <set>
class Cluster : public sl::api::IModule, public INodeListener
{
public:
	virtual bool initialize(sl::api::IKernel * pKernel);
	virtual bool launched(sl::api::IKernel * pKernel);
	virtual bool destory(sl::api::IKernel * pKernel);

	static void newNodeComing(sl::api::IKernel* pKernel, int32 nodeType, int32 nodeId, const OArgs& args);

private:
	static IHarbor* s_harbor;
	static std::set<int64> s_openNodes;
};
#endif