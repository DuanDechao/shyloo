#ifndef __SL_INTERFACE_CAPACITY_H__
#define __SL_INTERFACE_CAPACITY_H__
#include "slimodule.h"

class ICapacityPublisher : public sl::api::IModule{
public:
	virtual ~ICapacityPublisher() {}

	virtual void increaseLoad(int32 load) = 0;
	virtual void decreaseLoad(int32 load) = 0;
};

class ICapacitySubscriber : public sl::api::IModule{
public:
	virtual ~ICapacitySubscriber() {}

	virtual int32 choose(int32 nodeType) = 0;
	virtual bool checkOverLoad(const int32 nodeType, const int32 overload) = 0;
};
#endif