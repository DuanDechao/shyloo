#ifndef __SL_CORE_LUA_READER_H__
#define __SL_CORE_LUA_READER_H__
#include "slmulti_sys.h"
#include "lua/lua.hpp"
#include "IScriptEngine.h"

class LuaReader: public IScriptParamsReader{
public:
	LuaReader(lua_State* state) :_state(state) {}

	virtual int32 count() const;
	virtual bool getBoolean(const int32 index) const;
	virtual int8 getInt8(const int32 index) const;
	virtual int16 getInt16(const int32 index) const;
	virtual int32 getInt32(const int32 index) const;
	virtual int64 getInt64(const int32 index) const;
	virtual float getFloat(const int32 index) const;
	virtual const char* getString(const int32 index) const;
	virtual void* getPointer(const int32 index) const;
	virtual IScriptTable* getTable(const int32 index) const;

private:
	lua_State* _state;
};
#endif