#ifndef __SL_FRAMEWORK_AI_CONDITION_H__
#define __SL_FRAMEWORK_AI_CONDITION_H__
#include "slikernel.h"
class IObject;
class AICondition{
public:
	virtual ~AICondition(){}
	virtual bool check(sl::api::IKernel* pKernel, IObject* object) = 0;
};
#endif