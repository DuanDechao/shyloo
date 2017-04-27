#include "LuaWriter.h"

void LuaWriter::addBoolean(const bool value){
	lua_pushboolean(_state, value);
	++_count;
}

void LuaWriter::addInt8(const int8 value){
	lua_pushinteger(_state, value);
	++_count;
}

void LuaWriter::addInt16(const int16 value){
	lua_pushinteger(_state, value);
	++_count;
}

void LuaWriter::addInt32(const int32 value){
	lua_pushinteger(_state, value);
	++_count;
}

void LuaWriter::addInt64(const int64 value){
	lua_pushinteger(_state, value);
	++_count;
}

void LuaWriter::addFloat(const float value){
	lua_pushnumber(_state, value);
	++_count;
}

void LuaWriter::addString(const char* value){
	lua_pushstring(_state, value);
	++_count;
}

void LuaWriter::addPointer(void* value){
	lua_pushlightuserdata(_state, value);
	++_count;
}