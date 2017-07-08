#include "ScriptEngine.h"
#include "GameDefine.h"
#include "ScriptResult.h"
#include "ScriptCallor.h"
#include "LuaReader.h"
#include "LuaWriter.h"

extern "C"{
	void* SLScriptAlloc(void * ud, void * ptr, size_t osize, size_t nsize){
		if (nsize == 0){
			if (ptr)
				SLFREE(ptr);
			return NULL;
		}
		else{
			return SLREALLOC(ptr, nsize);
		}
	}

	int SLPanic(lua_State* L){
		lua_writestringerror("PANIC: unprotected error in call to lua API (%s)\n",
			lua_tostring(L, -1));
		return 0;
	}

	lua_State* SLLua_newState(){
		lua_State* L = lua_newstate(SLScriptAlloc, NULL);
		if (L) lua_atpanic(L, &SLPanic);
		return L;
	}
}

sl::api::IKernel* ScriptEngine::s_kernel = nullptr;

bool ScriptEngine::initialize(sl::api::IKernel * pKernel){
	s_kernel = pKernel;
	_self = this;

	_state = SLLua_newState();
	luaL_openlibs(_state);

	char scriptPath[game::MAX_PATH_LEN] = { 0 };
	SafeSprintf(scriptPath, sizeof(scriptPath), "%s/script/", pKernel->getEnvirPath());
	addSearchPath(pKernel, scriptPath);

	if (!loadScriptEntryFile(pKernel, "entry")){
		SLASSERT(false, "load entry script file failed");
		return false;
	}

	_callor = NEW ScriptCallor(this);

	return true;	
}
bool ScriptEngine::launched(sl::api::IKernel * pKernel){
	test();
	return true;
}
bool ScriptEngine::destory(sl::api::IKernel * pKernel){
	DEL _callor;
	return true;
}

IScriptCallor* ScriptEngine::prepareCall(const char* module, const char* func){
	SLASSERT(!_callor->isUsing(), "wtf");
	_callor->use(lua_gettop(_state));
	pushString(module);
	pushString(func);
	
	return _callor;
}

void ScriptEngine::RsgModuleFunc(const char* module, const char* func, const ScriptFuncType& f, const char* debug){
	char moduleName[512] = { 0 };
	SafeSprintf(moduleName, sizeof(moduleName), "shyloo.%s", module);

	lua_getglobal(_state, "package");
	lua_getfield(_state, -1, "loaded");
	lua_pushstring(_state, moduleName);
	lua_rawget(_state, -2);

	if (lua_isnil(_state, -1)){
		lua_pop(_state, 1);
		lua_pushstring(_state, moduleName);
		lua_newtable(_state);
		lua_rawget(_state, -3);

		lua_pushstring(_state, moduleName);
		lua_rawget(_state, -2);
	}
	SLASSERT(lua_istable(_state, -1), "where is script module %s", moduleName);

	lua_pushstring(_state, func);
	(*(ScriptFuncType*)lua_newuserdata(_state, sizeof(ScriptFuncType))) = f;
	lua_pushcclosure(_state, ScriptEngine::callBack, 1);
	
	lua_rawset(_state, -3);
	lua_pop(_state, 3);
}

int32 ScriptEngine::callBack(lua_State* state){
	ScriptFuncType func = *(ScriptFuncType*)lua_touserdata(state, lua_upvalueindex(1));
	SLASSERT(func, "where is c func");

	LuaReader reader(state);
	LuaWriter writer(state);

	func(s_kernel, &reader, &writer);

	return writer.count();
}

bool ScriptEngine::executeGlobalFunction(sl::api::IKernel* pKernel, int32 argc, const char* func, const ScriptResultReadFuncType& f){
	lua_getglobal(_state, func);
	if (!lua_isfunction(_state, -1)){
		lua_pop(_state, 1);
		return false;
	}

	lua_insert(_state, -(argc + 1));
	return executeFunction(pKernel, argc, f);
}

void ScriptEngine::addSearchPath(sl::api::IKernel* pKernel, const char* path){
	lua_getglobal(_state, "package");
	lua_getfield(_state, -1, "path");
	const char* currPath = lua_tostring(_state, -1);
	lua_pushfstring(_state, "%s;%s/?.lua", currPath, path);
	lua_setfield(_state, -3, "path");
	lua_pop(_state, 2);
}

bool ScriptEngine::loadScriptEntryFile(sl::api::IKernel* pKenel, const char* file){
	char code[512];
	SafeSprintf(code, 511, "return require \"%s\"", file);

	luaL_loadstring(_state, code);

	return executeFunction(pKenel, 0, nullptr);
}

bool ScriptEngine::executeFunction(sl::api::IKernel* pKernel, int32 argc, const ScriptResultReadFuncType& f){
	int32 functionIndex = -(argc + 1);
	if (!lua_isfunction(_state, functionIndex)){
		lua_pop(_state, argc + 1); // pop all args and function
		return false;
	}

	int32 trackback = 0;
	lua_getglobal(_state, "__G__TRACKBACK__");
	if (!lua_isfunction(_state, -1)){
		lua_pop(_state, 1);
	}
	else{
		lua_insert(_state, functionIndex - 1);
		trackback = functionIndex - 1;
	}

	int32 error = 0;
	error = lua_pcall(_state, argc, 8, trackback);
	if (error){
		if (trackback == 0){
			ECHO_ERROR("lua error:%s", lua_tostring(_state, -1));
			lua_pop(_state, 1);
		}
		else{
			lua_pop(_state, 2);
		}
		return false;
	}

	ScriptResult result(_state);
	if (f){
		f(pKernel, &result);
	}

	lua_pop(_state, 8);

	if (trackback != 0)
		lua_pop(_state, 1);

	return true;
}

void ScriptEngine::test(){
	/*IScriptCallor* callor = prepareCall("ai.condition", "call");
	callor->addString("IsNumber(4)");
	callor->addPointer(nullptr); 
	callor->call(s_kernel, [&](sl::api::IKernel* pKernel, const IScriptResult* result){
		result->getBool(0);
		//int32 temp = 0;
	});*/
}

