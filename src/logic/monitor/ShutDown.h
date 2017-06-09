#ifndef __SL_MONITOR_SHUTDOWN_H__
#define __SL_MONITOR_SHUTDOWN_H__
#include "slmulti_sys.h"
#include "slimodule.h"
#include "IHarbor.h"
#include "slsingleton.h"

class IHarbor;
class ShutDown : public sl::api::IModule, public sl::SLHolder<ShutDown>{
public:
	virtual bool initialize(sl::api::IKernel * pKernel);
	virtual bool launched(sl::api::IKernel * pKernel);
	virtual bool destory(sl::api::IKernel * pKernel);
};
#endif