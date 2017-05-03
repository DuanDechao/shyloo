#ifndef __SL_FRAMEWORK_AI_CONDITION_H__
#define __SL_FRAMEWORK_AI_CONDITION_H__
#include "slikernel.h"
#include <string>
class IObject;
class AICondition{
public:
	AICondition(const char* condition) :_condition(condition){}
	virtual ~AICondition(){}

	virtual bool check(sl::api::IKernel* pKernel, IObject* object);

private:
	std::string _condition;
};
#endif