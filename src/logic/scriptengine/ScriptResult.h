#ifndef __SL_CORE_SCRIPT_RESULT_H__
#define __SL_CORE_SCRIPT_RESULT_H__
#include "IScriptEngine.h"
#include "lua/lua.hpp"

class ScriptResult: public IScriptResult{
public:
	ScriptResult(lua_State* state);

	virtual int32 count(){ return _count; }
	virtual bool getBool(const int32 index);
	virtual int8 getInt8(const int32 index);
	virtual int16 getInt16(const int32 index);
	virtual int32 getInt32(const int32 index);
	virtual int64 getInt64(const int32 index);
	virtual float getFloat(const int32 index);
	virtual const char* getString(const int32 index);
	virtual void* getPointer(const int32 index);
	
private:
	lua_State* _state;
	int32	   _count;
	int32	   _start;
};
#endif