#ifndef __SL_CAPACITY_SUBSCRIBER_H__
#define __SL_CAPACITY_SUBSCRIBER_H__
#include "ICapacity.h"
class IHarbor;
class CapacitySubscriber : public ICapacitySubscriber{
public:
	virtual bool initialize(sl::api::IKernel * pKernel);
	virtual bool launched(sl::api::IKernel * pKernel);
	virtual bool destory(sl::api::IKernel * pKernel);

private:
	IHarbor* _harbor;
};
#endif