#include "Condition.h"

Condition::Condition(const char* funcStr){
	_func = &Condition::IsMine;
}

bool Condition::check(sl::api::IKernel* pKernel, IObject* object){
	return (this->*_func)(object);
}

bool Condition::IsMine(IObject* object, ...){
	return true;
}
