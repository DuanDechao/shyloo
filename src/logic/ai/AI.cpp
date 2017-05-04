#include "AI.h"
#include "slfile_utils.h"
#include "AILoader.h"
#include "AIMachine.h"
#include "Attr.h"
#include "IDCCenter.h"
#include "OCTimer.h"

IScriptEngine* AI::s_scriptEngine = nullptr;
AI::AI_CONFIG_MAP AI::s_ais;
bool AI::initialize(sl::api::IKernel * pKernel){
	_self = this;
	_kernel = pKernel;

	char path[256] = { 0 };
	SafeSprintf(path, sizeof(path)-1, "%s/ai", pKernel->getEnvirPath());

	bool ret = true;
	sl::ListFileInDirection(path, ".xml", [&](const char * name, const char * path) {
		if (s_ais.find(name) == s_ais.end()){
			SLASSERT(false, "ai config xml name repeated");
			ret = false;
			return;
		}
		AIConfig conf;
		conf.machine = AILoader::load(pKernel, path, conf.interval);
		s_ais[name] = conf;
	});

	return ret;
}

bool AI::launched(sl::api::IKernel * pKernel){
	FIND_MODULE(s_scriptEngine, ScriptEngine);
	return true;
}

bool AI::destory(sl::api::IKernel * pKernel){
	if (!s_ais.empty()){
		auto itor = s_ais.begin();
		for (; itor != s_ais.end(); ++itor){
			if (itor->second.machine)
				DEL itor->second.machine;
		}
	}
	s_ais.clear();

	DEL this;
	return true;
}

void AI::startAI(IObject* object){
	int32 aiCount = object->getTempInt32(OCTempProp::AI_REF);
	object->setTempInt32(OCTempProp::AI_REF, aiCount + 1);

	if (aiCount == 0)
		startAI(_kernel, object);
}

void AI::stopAI(IObject* object){
	int32 aiCount = object->getTempInt32(OCTempProp::AI_REF);
	object->setTempInt32(OCTempProp::AI_REF, aiCount - 1);

	if (aiCount == 1)
		stopAI(_kernel, object);
}

void AI::startAI(sl::api::IKernel* pKernel, IObject* object){
	SLASSERT(object, "wtf");
	const char* ai = object->getPropString(attr_def::ai);
	if (strcmp(ai, "") == 0 || s_ais.find(ai) == s_ais.end()){
		SLASSERT(false, "has no ai %s", ai);
		return;
	}

	OCTimer* aiTimer = (OCTimer*)object->getTempInt64(OCTempProp::AITIMER);
	if (aiTimer){
		SLASSERT(false, "already has a ai timer");
		return;
	}

	OCTimer* timer = OCTimer::create(pKernel, object, OCTempProp::AITIMER, AI::onAIStart, AI::onAITick, AI::onAIEnd);
	object->setTempInt64(OCTempProp::AITIMER, (int64)timer);
	START_TIMER(timer, 0, TIMER_BEAT_FOREVER, object->getPropInt32(attr_def::aiInterval));
	ECHO_TRACE("start ai %s for object %lld", ai, object->getID());
}

void AI::stopAI(sl::api::IKernel* pKernel, IObject* object){
	OCTimer* timer = (OCTimer*)object->getTempInt64(OCTempProp::AITIMER);
	SLASSERT(timer, "cannot find ai timer");
	if (timer){
		pKernel->killTimer(timer);
	}
}

void AI::onAIStart(sl::api::IKernel* pKernel, IObject* object, int64){

}

void AI::onAITick(sl::api::IKernel* pKernel, IObject* object, int64){
	SLASSERT(object, "cannot find object");
	const char* ai = object->getPropString(attr_def::ai);
	if (strcmp(ai, "") == 0){
		SLASSERT(false, "have no ai");
		return;
	}

	auto aiItor = s_ais.find(ai);
	if (aiItor == s_ais.end() || !aiItor->second.machine){
		SLASSERT(false, "ai %s is not exist", ai);
		return;
	}

	aiItor->second.machine->run(pKernel, object);
}

void AI::onAIEnd(sl::api::IKernel* pKernel, IObject* object, bool, int64){
	OCTimer* timer = (OCTimer*)object->getTempInt64(OCTempProp::AITIMER);
	SLASSERT(timer, "cannot find ai timer");
	if (timer){
		timer->release();
		object->setTempInt64(OCTempProp::AITIMER, 0);
	}
}
