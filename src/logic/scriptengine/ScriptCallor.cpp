#include "ScriptCallor.h"
#include "ScriptEngine.h"

void ScriptCallor::addBool(bool value){
	SLASSERT(_using, "wtf");
	_engine->pushBoolean(value);
	++_count; 
}

void ScriptCallor::addInt8(int8 value){
	SLASSERT(_using, "wtf");
	_engine->pushInt(value);
	++_count;
}

void ScriptCallor::addInt16(int16 value){
	SLASSERT(_using, "wtf");
	_engine->pushInt(value);
	++_count;
}

void ScriptCallor::addInt32(int32 value){
	SLASSERT(_using, "wtf");
	_engine->pushInt(value);
	++_count;
}

void ScriptCallor::addInt64(int64 value){
	SLASSERT(_using, "wtf");
	_engine->pushInt(value);
	++_count;
}

void ScriptCallor::addFloat(float value){
	SLASSERT(_using, "wtf");
	_engine->pushDouble(value);
	++_count;
}

void ScriptCallor::addDouble(double value){
	SLASSERT(_using, "wtf");
	_engine->pushDouble(value);
	++_count;
}

void ScriptCallor::addString(const char* value){
	SLASSERT(_using, "wtf");
	_engine->pushString(value);
	++_count;
}

void ScriptCallor::addPointer(void* value){
	SLASSERT(_using, "wtf");
	_engine->pushPointer(value);
	++_count;
}

bool ScriptCallor::call(sl::api::IKernel* pKernel, const ScriptResultReadFuncType& f){
	bool ret = _engine->executeGlobalFunction(pKernel, _count + 2, "call", f);
	_using = false;
	_engine->resetTop(_top);
	return ret;
}