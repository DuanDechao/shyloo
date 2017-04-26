#include "ScriptEngine.h"
#include "GameDefine.h"

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
bool ScriptEngine::initialize(sl::api::IKernel * pKernel){
	_kernel = pKernel;
	_self = this;

	_state = SLLua_newState();
	luaL_openlibs(_state);

	char scriptPath[game::MAX_PATH_LEN] = { 0 };
	SafeSprintf(scriptPath, sizeof(scriptPath), "%s/script/", pKernel->getEnvirPath());
	return true;	
}
bool ScriptEngine::launched(sl::api::IKernel * pKernel){
	return true;
}
bool ScriptEngine::destory(sl::api::IKernel * pKernel){
	return true;
}

void ScriptEngine::addSearchPath(sl::api::IKernel* pKernel, const char* path){
	lua_getglobal(_state, "package");
	lua_getfield(_state, -1, "path");
	const char* currPath = lua_tostring(_state, -1);
	lua_pushfstring(_state, "%s;%s/?.lua", currPath, path);
	lua_setfield(_state, -3, "path");
	lua_pop(_state, 2);
}