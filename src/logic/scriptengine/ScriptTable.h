#ifndef __SL_CORE_SCRIPT_TABLE_H__
#define __SL_CORE_SCRIPT_TABLE_H__
#include "slmulti_sys.h"
#include "lua/lua.hpp"
#include "slpool.h"
#include "IScriptEngine.h"
class ScriptTable: public IScriptTable{
	friend class sl::SLPool<ScriptTable>;
public:
	static ScriptTable* create(lua_State* state, int32 tableIndex, bool pop) { return CREATE_FROM_POOL(s_pool, state, tableIndex, pop); }
	virtual void release();

	virtual bool getBoolean(const int64 key);
	virtual int8 getInt8(const int64 key);
	virtual int16 getInt16(const int64 key);
	virtual int32 getInt32(const int64 key);
	virtual int64 getInt64(const int64 key);
	virtual float getFloat(const int64 key);
	virtual void* getPointer(const int64 key);
	virtual IScriptTable* getTable(const int64 key);
	virtual int32 getArrayCount();

	virtual bool getBoolean(const char* key);
	virtual int8 getInt8(const char* key);
	virtual int16 getInt16(const char* key);
	virtual int32 getInt32(const char* key);
	virtual int64 getInt64(const char* key);
	virtual float getFloat(const char* key);
	virtual void* getPointer(const char* key);
	virtual IScriptTable* getTable(const char* key);

protected:
	virtual const char* getString(const int32 key);
	virtual const char* getString(const char* key);
	virtual void freeString();

private:
	ScriptTable(lua_State* state, int32 tableIndex, bool pop);
	virtual ~ScriptTable() {}

private:
	lua_State* _state;
	int32 _tableIndex;
	int32 _pop;
	int32 _top;

	static sl::SLPool<ScriptTable> s_pool;
};
#endif