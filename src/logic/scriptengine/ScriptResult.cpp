#include "ScriptResult.h"
#include "ScriptTable.h"
#define MAX_SCRIPT_RESULT_COUNT 8
ScriptResult::ScriptResult(lua_State* state){
	_state = state;
	if (_state){
		_count = 0;
		for (int32 i = 0; i < MAX_SCRIPT_RESULT_COUNT; i++){
			if (lua_isnil(_state, -MAX_SCRIPT_RESULT_COUNT + i))
				break;
			++_count;
		}
	}
	else{
		_count = 0;
	}
	_start = lua_gettop(_state) - MAX_SCRIPT_RESULT_COUNT + 1;
}

bool ScriptResult::getBool(const int32 index) const{
	SLASSERT(index < _count, "wtf");
	SLASSERT(lua_isboolean(_state, _start + index), "wtf");
	return lua_toboolean(_state, _start + index);
}

int8 ScriptResult::getInt8(const int32 index)const{
	SLASSERT(index < _count, "wtf");
	SLASSERT(lua_isinteger(_state, _start + index), "wtfd");
	return (int8)lua_tointeger(_state, _start + index);
}

int16 ScriptResult::getInt16(const int32 index)const{
	SLASSERT(index < _count, "wtf");
	SLASSERT(lua_isinteger(_state, _start + index), "wtf");
	return (int16)lua_tointeger(_state, _start + index);
}

int32 ScriptResult::getInt32(const int32 index)const{
	SLASSERT(index < _count, "wtf");
	SLASSERT(lua_isinteger(_state, _start + index), "wtrfd");
	return (int32)lua_tointeger(_state, _start + index);
}

int64 ScriptResult::getInt64(const int32 index)const{
	SLASSERT(index < _count, "wtf");
	SLASSERT(lua_isinteger(_state, _start + index), "wtrfd");
	return (int64)lua_tointeger(_state, _start + index);
}

float ScriptResult::getFloat(const int32 index)const{
	SLASSERT(index < _count, "wtf");
	SLASSERT(lua_isnumber(_state, _start + index), "wtrfd");
	return (float)lua_tonumber(_state, _start + index);
}

const char* ScriptResult::getString(const int32 index)const{
	SLASSERT(index < _count, "wtf");
	SLASSERT(lua_isstring(_state, _start + index), "wtrfd");
	return (const char*)lua_tostring(_state, _start + index);
}

void* ScriptResult::getPointer(const int32 index)const{
	SLASSERT(index < _count, "wtf");
	SLASSERT(lua_isuserdata(_state, _start + index), "wtrfd");
	return (void*)lua_touserdata(_state, _start + index);
}

IScriptTable* ScriptResult::getTable(const int32 index)const{
	SLASSERT(index < _count, "wtf");
	SLASSERT(lua_istable(_state, _start + index), "wtrfd");
	return ScriptTable::create(_state, _start + index, false);
}