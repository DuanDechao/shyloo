#include "ScriptTable.h"

sl::SLPool<ScriptTable> ScriptTable::s_pool;
ScriptTable::ScriptTable(lua_State* state, int32 tableIndex, bool pop)
	:_state(state),
	_tableIndex(tableIndex),
	_pop(pop){
	_top = lua_gettop(state);
}

void ScriptTable::release(){
	SLASSERT(lua_gettop(_state) == _top, "wtf");
	if (_pop){
		SLASSERT(lua_gettop(_state) == _tableIndex, "can't release table is not on the top");
		lua_remove(_state, _tableIndex);
	}
	s_pool.recover(this);
}

bool ScriptTable::getBoolean(const int64 key){
	lua_pushinteger(_state, key);
	lua_rawget(_state, _tableIndex);
	SLASSERT(lua_isboolean(_state, -1), "wtf");
	return lua_toboolean(_state, -1);
}

int8 ScriptTable::getInt8(const int64 key){
	lua_pushinteger(_state, key);
	lua_rawget(_state, _tableIndex);
	SLASSERT(lua_isinteger(_state, -1), "wtf");
	return (int8)lua_tointeger(_state, -1);
}

int16 ScriptTable::getInt16(const int64 key){
	lua_pushinteger(_state, key);
	lua_rawget(_state, _tableIndex);
	SLASSERT(lua_isinteger(_state, -1), "wtfd"); 
	return (int16)lua_tointeger(_state, -1);
}

int32 ScriptTable::getInt32(const int64 key){
	lua_pushinteger(_state, key);
	lua_rawget(_state, _tableIndex);
	SLASSERT(lua_isinteger(_state, -1), "wtfd");
	return (int32)lua_tointeger(_state, -1);
}

int64 ScriptTable::getInt64(const int64 key){
	lua_pushinteger(_state, key);
	lua_rawget(_state, _tableIndex);
	SLASSERT(lua_isinteger(_state, -1), "wtfd");
	return (int64)lua_tointeger(_state, -1);
}

float ScriptTable::getFloat(const int64 key){
	lua_pushinteger(_state, key);
	lua_rawget(_state, _tableIndex);
	SLASSERT(lua_isnumber(_state, -1), "wtfd");
	return (float)lua_tonumber(_state, -1);
}

void* ScriptTable::getPointer(const int64 key){
	lua_pushinteger(_state, key);
	lua_rawget(_state, _tableIndex);
	SLASSERT(lua_islightuserdata(_state, -1), "wtfd");
	return (void*)lua_touserdata(_state, -1);
}

IScriptTable* ScriptTable::getTable(const int64 key){
	lua_pushinteger(_state, key);
	lua_rawget(_state, _tableIndex);
	SLASSERT(lua_istable(_state, -1), "wtf");
	return ScriptTable::create(_state, lua_gettop(_state), true);
}

int32 ScriptTable::getArrayCount(){
	return (int32)lua_rawlen(_state, _tableIndex);
}

bool ScriptTable::getBoolean(const char* key){
	lua_pushstring(_state, key);
	lua_rawget(_state, _tableIndex);
	SLASSERT(lua_isboolean(_state, -1), "wtf");
	return lua_toboolean(_state, -1);
}

int8 ScriptTable::getInt8(const char* key){
	lua_pushstring(_state, key);
	lua_rawget(_state, _tableIndex);
	SLASSERT(lua_isinteger(_state, -1), "wtf");
	return (int8)lua_tointeger(_state, -1);
}

int16 ScriptTable::getInt16(const char* key){
	lua_pushstring(_state, key);
	lua_rawget(_state, _tableIndex);
	SLASSERT(lua_isinteger(_state, -1), "wtf");
	return (int16)lua_tointeger(_state, -1);
}

int32 ScriptTable::getInt32(const char* key){
	lua_pushstring(_state, key);
	lua_rawget(_state, _tableIndex);
	SLASSERT(lua_isinteger(_state, -1), "wtf");
	return (int32)lua_tointeger(_state, -1);
}

int64 ScriptTable::getInt64(const char* key){
	lua_pushstring(_state, key);
	lua_rawget(_state, _tableIndex);
	SLASSERT(lua_isinteger(_state, -1), "wtf");
	return (int64)lua_tointeger(_state, -1);
}

float ScriptTable::getFloat(const char* key){
	lua_pushstring(_state, key);
	lua_rawget(_state, _tableIndex);
	SLASSERT(lua_isnumber(_state, -1), "wtf");
	return (float)lua_tonumber(_state, -1);
}

void* ScriptTable::getPointer(const char* key){
	lua_pushstring(_state, key);
	lua_rawget(_state, _tableIndex);
	SLASSERT(lua_islightuserdata(_state, -1), "wtfd");
	return (void*)lua_touserdata(_state, -1);
}

IScriptTable* ScriptTable::getTable(const char* key){
	lua_pushstring(_state, key);
	lua_rawget(_state, _tableIndex);
	SLASSERT(lua_istable(_state, -1), "wtfd");
	return ScriptTable::create(_state, lua_gettop(_state), true);
}

const char* ScriptTable::getString(const int32 key){
	lua_pushinteger(_state, key);
	lua_rawget(_state, _tableIndex);
	SLASSERT(lua_isstring(_state, -1) != 0, "wtf");
	return lua_tostring(_state, -1);
}

const char* ScriptTable::getString(const char* key){
	lua_pushstring(_state, key);
	lua_rawget(_state, _tableIndex);
	SLASSERT(lua_isstring(_state, -1) != 0, "wtf");
	return lua_tostring(_state, -1);
}

void ScriptTable::freeString(){
	lua_pop(_state, 1);
}