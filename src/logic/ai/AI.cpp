#include "AI.h"

IScriptEngine* AI::s_scriptEngine = nullptr;
bool AI::initialize(sl::api::IKernel * pKernel){
	_self = this;
	_kernel = pKernel;

	return true;
}

bool AI::launched(sl::api::IKernel * pKernel){
	FIND_MODULE(s_scriptEngine, ScriptEngine);
	return true;
}

bool AI::destory(sl::api::IKernel * pKernel){
	DEL this;
	return true;
}

void AI::startAI(IObject* object){

}

void AI::stopAI(IObject* object){

}
