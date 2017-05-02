#ifndef __SL_CORE_SCRIPT_ENGINE_H__
#define __SL_CORE_SCRIPT_ENGINE_H__
#include "slsingleton.h"
#include "IScriptEngine.h"
#include "lua/lua.hpp"
#include "slikernel.h"
class ScriptCallor;
class ScriptEngine : public IScriptEngine, public sl::SLHolder<ScriptEngine>{
public:
	virtual bool initialize(sl::api::IKernel * pKernel);
	virtual bool launched(sl::api::IKernel * pKernel);
	virtual bool destory(sl::api::IKernel * pKernel);

	virtual IScriptCallor* prepareCall(const char* module, const char* func);
	virtual void RsgModuleFunc(const char* module, const char* func, const ScriptFuncType& f, const char* debug);

	static int32 callBack(lua_State* state);

	inline void pushInt(int64 value){ lua_pushinteger(_state, value); }
	inline void pushBoolean(bool value) { lua_pushboolean(_state, value); }
	inline void pushDouble(double value){ lua_pushnumber(_state, value); }
	inline void pushString(const char* value){ lua_pushstring(_state, value);}
	inline void pushPointer(void* value){ lua_pushlightuserdata(_state, value); }
	inline void resetTop(const int32 top) { lua_settop(_state, top); }
	bool executeGlobalFunction(sl::api::IKernel* pKernel, int32 argc, const char* func, const ScriptResultReadFuncType& f);

	void test();

private:
	void addSearchPath(sl::api::IKernel* pKernel, const char* path);
	bool loadScriptEntryFile(sl::api::IKernel* pKernel, const char* file);
	bool executeFunction(sl::api::IKernel* pKernel, int32 argc, const ScriptResultReadFuncType& f);

private:
	static sl::api::IKernel* s_kernel;
	ScriptEngine*	  _self;
	ScriptCallor*	  _callor;

	lua_State*	_state;
};
#endif