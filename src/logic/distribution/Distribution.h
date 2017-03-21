#ifndef __SL_FRAMEWORK_DISTRIBUTION_H__
#define __SL_FRAMEWORK_DISTRIBUTION_H__
#include "slikernel.h"
#include "IDistribution.h"
class Distribution :public IDistribution{
public:
	virtual bool initialize(sl::api::IKernel * pKernel);
	virtual bool launched(sl::api::IKernel * pKernel);
	virtual bool destory(sl::api::IKernel * pKernel);

private:
	sl::api::IKernel*	_kernel;
};
#endif