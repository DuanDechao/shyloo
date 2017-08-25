#ifndef __SL_CAPACITY_SUBSCRIBER_H__
#define __SL_CAPACITY_SUBSCRIBER_H__
#include "ICapacity.h"
#include <unordered_map>
class IHarbor;
class CapacitySubscriber : public ICapacitySubscriber{
	struct LoadData{
		float real;
	};
public:
	virtual bool initialize(sl::api::IKernel * pKernel);
	virtual bool launched(sl::api::IKernel * pKernel);
	virtual bool destory(sl::api::IKernel * pKernel);

	void nodeLoadReport(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId, const OArgs& args);

	virtual int32 choose(int32 nodeType);

	virtual bool checkOverLoad(const int32 nodeType, const int32 overload);

private:
	int32 chooseStrategy1(int32 nodeType);
	int32 chooseStrategy2(int32 nodeType);

private:
	IHarbor* _harbor;
	std::unordered_map<int32, std::unordered_map<int32, LoadData>> _allNodeLoad;
};
#endif