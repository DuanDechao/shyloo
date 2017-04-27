#ifndef __SL_CORE_LUA_WRITER_H__
#define __SL_CORE_LUA_WRITER_H__
#include "slmulti_sys.h"
#include "lua/lua.hpp"
#include "IScriptEngine.h"

class LuaWriter : public IScriptParamsWriter{
public:
	LuaWriter(lua_State* state) :_state(state),_count(0) {}

	virtual int32 count() { return _count; }

	virtual void addBoolean(const bool value);
	virtual void addInt8(const int8 value);
	virtual void addInt16(const int16 value);
	virtual void addInt32(const int32 value);
	virtual void addInt64(const int64 value);
	virtual void addFloat(const float value);
	virtual void addString(const char* value);
	virtual void addPointer(void* value);

private:
	lua_State* _state;
	int32 _count;
};
#endif