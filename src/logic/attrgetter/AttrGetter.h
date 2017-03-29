#ifndef __SL_FRAMEWORK_ATTR_GETTER_H__
#define __SL_FRAMEWORK_ATTR_GETTER_H__
#include "slikernel.h"
#include "slimodule.h"
class IObjectMgr;
class AttrGetter :public sl::api::IModule{
public:
	virtual bool initialize(sl::api::IKernel * pKernel);
	virtual bool launched(sl::api::IKernel * pKernel);
	virtual bool destory(sl::api::IKernel * pKernel);

private:
	sl::api::IKernel*	_kernel;
	IObjectMgr*			_objectMgr;
};
#endif