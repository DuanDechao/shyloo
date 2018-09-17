#ifndef __SL_MONITOR_SERVER_STATE_H__
#define __SL_MONITOR_SERVER_STATE_H__
#include "slmulti_sys.h"
#include "slimodule.h"
#include "slsingleton.h"

class ServerState : public sl::api::IModule, public sl::SLHolder<ServerState>{
public:
	virtual bool initialize(sl::api::IKernel * pKernel);
	virtual bool launched(sl::api::IKernel * pKernel);
	virtual bool destory(sl::api::IKernel * pKernel);
};
#endif