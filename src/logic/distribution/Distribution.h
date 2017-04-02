 #ifndef __SL_FRAMEWORK_DISTRIBUTION_H__
#define __SL_FRAMEWORK_DISTRIBUTION_H__
#include "slikernel.h"
#include "IDistribution.h"
#include <unordered_map>

class IHarbor;
class ICapacitySubscriber;
class Distribution :public IDistribution{
public:
	virtual bool initialize(sl::api::IKernel * pKernel);
	virtual bool launched(sl::api::IKernel * pKernel);
	virtual bool destory(sl::api::IKernel * pKernel);

	void onGateDistributeLogic(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId, const OArgs& args);
	void onLogicAddPlayer(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId, const OArgs& args);
	void onLogicRemovePlayer(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId, const OArgs& args);

private:
	sl::api::IKernel*		_kernel;
	IHarbor*				_harbor;
	ICapacitySubscriber*	_capacity;

	std::unordered_map<int64, int32> _distributes;
	std::unordered_map<int64, int32> _players;
};
#endif