#ifndef _SL_AI_COMM_CONDITION_H_
#define _SL_AI_COMM_CONDITION_H_
#include "../AICondition.h"
class IObject;

class Condition : public AICondition{
	typedef bool (Condition::*ConditionFunc)(IObject* object, ...);

public:
	Condition(const char* funcStr);
	virtual bool check(sl::api::IKernel* pKernel, IObject* object);

private:
	bool IsMine(IObject* object, ...);


private:
	ConditionFunc	_func;
};
#endif