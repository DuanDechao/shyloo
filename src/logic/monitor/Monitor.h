#ifndef __SL_MONITOR_H__
#define __SL_MONITOR_H__
#include "slmulti_sys.h"
#include "slimodule.h"
#include "slsingleton.h"

class IClient;
class Monitor : public sl::api::IModule, public sl::SLHolder<Monitor>{
public:
	virtual bool initialize(sl::api::IKernel * pKernel);
	virtual bool launched(sl::api::IKernel * pKernel);
	virtual bool destory(sl::api::IKernel * pKernel);

private:
	
};
#endif