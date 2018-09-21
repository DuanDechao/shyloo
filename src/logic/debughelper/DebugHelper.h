#ifndef SL_LOGIC_DEBUGHELPER_H
#define SL_LOGIC_DEBUGHELPER_H
#include "slikernel.h"
#include "IDebugHelper.h"
#include "slsingleton.h"
#include "log4cxx/logger.h"

class DebugHelper :public IDebugHelper, public sl::SLHolder<DebugHelper>{
public:
	virtual bool initialize(sl::api::IKernel * pKernel);
	virtual bool launched(sl::api::IKernel * pKernel);
	virtual bool destory(sl::api::IKernel * pKernel);
private:
	DebugHelper*				_self;
};

#endif
