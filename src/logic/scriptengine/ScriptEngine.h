#ifndef __SL_CORE_SCRIPT_ENGINE_H__
#define __SL_CORE_SCRIPT_ENGINE_H__
#include "slsingleton.h"
#include "IScriptEngine.h"
#include "lua/lua.hpp"
#include "slikernel.h"
class ScriptEngine : public IScriptEngine, public sl::SLHolder<ScriptEngine>{
public:
	virtual bool initialize(sl::api::IKernel * pKernel);
	virtual bool launched(sl::api::IKernel * pKernel);
	virtual bool destory(sl::api::IKernel * pKernel);

private:
	void addSearchPath(sl::api::IKernel* pKernel, const char* path);

private:
	sl::api::IKernel* _kernel;
	ScriptEngine*	  _self;

	lua_State*	_state;
};
#endif