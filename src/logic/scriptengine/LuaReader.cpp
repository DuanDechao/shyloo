#include "LuaReader.h"
#include "ScriptTable.h"
int32 LuaReader::count() const{
	return lua_gettop(_state);
}

bool LuaReader::getBoolean(const int32 index) const {
	SLASSERT(index >= 0 && index < lua_gettop(_state), "out of range");
	SLASSERT(lua_isboolean(_state, index + 1), "wtf");
	return lua_toboolean(_state, index + 1);
}

int8 LuaReader::getInt8(const int32 index) const{
	SLASSERT(index >= 0 && index < lua_gettop(_state), "out of range");
	SLASSERT(lua_isinteger(_state, index + 1), "wtf");
	return (int8)lua_tointeger(_state, index + 1);
}

int16 LuaReader::getInt16(const int32 index) const{
	SLASSERT(index >= 0 && index < lua_gettop(_state), "out of range");
	SLASSERT(lua_isinteger(_state, index + 1), "wtf");
	return (int16)lua_tointeger(_state, index + 1);
}

int32 LuaReader::getInt32(const int32 index) const{
	SLASSERT(index >= 0 && index < lua_gettop(_state), "out of range");
	SLASSERT(lua_isinteger(_state, index + 1), "wtf");
	return (int32)lua_tointeger(_state, index + 1);
}

int64 LuaReader::getInt64(const int32 index) const{
	SLASSERT(index >= 0 && index < lua_gettop(_state), "out of range");
	SLASSERT(lua_isinteger(_state, index + 1), "wtf");
	return (int64)lua_tointeger(_state, index + 1);
}

float LuaReader::getFloat(const int32 index) const{
	SLASSERT(index >= 0 && index < lua_gettop(_state), "out of range");
	SLASSERT(lua_isnumber(_state, index + 1), "wtf");
	return (float)lua_tonumber(_state, index + 1);
}
const char* LuaReader::getString(const int32 index) const{
	SLASSERT(index >= 0 && index < lua_gettop(_state), "out of range");
	SLASSERT(lua_isstring(_state, index + 1), "wtf");
	return (const char*)lua_tostring(_state, index + 1);
}
void* LuaReader::getPointer(const int32 index) const{
	SLASSERT(index >= 0 && index < lua_gettop(_state), "out of range");
	SLASSERT(lua_islightuserdata(_state, index + 1) || lua_isnil(_state, index + 1), "wtf");
	return (void*)lua_touserdata(_state, index + 1);
}

IScriptTable* LuaReader::getTable(const int32 index) const{
	SLASSERT(index >= 0 && index < lua_gettop(_state), "out of range");
	SLASSERT(lua_istable(_state, index + 1), "wtf");
	return ScriptTable::create(_state, index + 1, false);
}
