#ifndef __SL_IIDMGR_H__
#define __SL_IIDMGR_H__
#include "slimodule.h"

class ICapacityPublisher : public sl::api::IModule{
public:
	virtual ~ICapacityPublisher() {}
};

class ICapacitySubscriber : public sl::api::IModule{
public:
	virtual ~ICapacitySubscriber() {}
};
#endif