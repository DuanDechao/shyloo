#include "AICondition.h"
#include "IScriptEngine.h"
#include "AI.h"
bool AICondition::check(sl::api::IKernel* pKernel, IObject* object){
	IScriptCallor* callor = AI::getInstance()->GetScriptEngine()->prepareCall("ai.condition", "call");
	callor->addString(_condition.c_str());
	callor->addPointer(object);
	bool ret = true;
	callor->call(pKernel, [&ret](sl::api::IKernel* pKernel, const IScriptResult* result){
		ret = result->getBool(0);
	});

	return ret;
}