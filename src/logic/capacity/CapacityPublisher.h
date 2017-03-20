#ifndef __SL_CORE_CAPACITY_PUBLISHER_H__
#define __SL_CORE_CAPACITY_PUBLISHER_H__
#include "ICapacity.h"
class IHarbor;
class CapacityPublisher : public ICapacityPublisher{
public:
	virtual bool initialize(sl::api::IKernel * pKernel);
	virtual bool launched(sl::api::IKernel * pKernel);
	virtual bool destory(sl::api::IKernel * pKernel);

private:
	IHarbor* _harbor;
};
#endif