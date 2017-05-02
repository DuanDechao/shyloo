#ifndef __SL_CORE_SCRIPT_RESULT_H__
#define __SL_CORE_SCRIPT_RESULT_H__
#include "IScriptEngine.h"
#include "lua/lua.hpp"

class ScriptResult: public IScriptResult{
public:
	ScriptResult(lua_State* state);

	virtual int32 count() const { return _count; }
	virtual bool getBool(const int32 index)const;
	virtual int8 getInt8(const int32 index)const;
	virtual int16 getInt16(const int32 index)const;
	virtual int32 getInt32(const int32 index)const;
	virtual int64 getInt64(const int32 index)const;
	virtual float getFloat(const int32 index)const;
	virtual const char* getString(const int32 index)const;
	virtual void* getPointer(const int32 index)const;
	virtual IScriptTable* getTable(const int32 index)const;
	
private:
	lua_State* _state;
	int32	   _count;
	int32	   _start;
};
#endif