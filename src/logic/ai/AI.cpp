#include "AI.h"
#include "slfile_utils.h"
#include "AILoader.h"
#include "AIMachine.h"
#include "Attr.h"
#include "IDCCenter.h"
#include "IObjectTimer.h"

bool AI::initialize(sl::api::IKernel * pKernel){
	_self = this;
	_kernel = pKernel;

	char path[256] = { 0 };
	SafeSprintf(path, sizeof(path)-1, "%s/ai", pKernel->getEnvirPath());

	bool ret = true;
	sl::CFileUtils::ListFileInDirection(path, ".xml", [&](const char * name, const char * path) {
		if (_ais.find(name) == _ais.end()){
			SLASSERT(false, "ai config xml name repeated");
			ret = false;
			return;
		}
		AIConfig conf;
		conf.machine = AILoader::load(pKernel, path, conf.interval);
		_ais[name] = conf;
	});

	return ret;
}

bool AI::launched(sl::api::IKernel * pKernel){
	FIND_MODULE(_scriptEngine, ScriptEngine);
	FIND_MODULE(_objectTimer, ObjectTimer);
	return true;
}

bool AI::destory(sl::api::IKernel * pKernel){
	if (!_ais.empty()){
		auto itor = _ais.begin();
		for (; itor != _ais.end(); ++itor){
			if (itor->second.machine)
				DEL itor->second.machine;
		}
	}
	_ais.clear();

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
	if (strcmp(ai, "") == 0 || _ais.find(ai) == _ais.end()){
		SLASSERT(false, "has no ai %s", ai);
		return;
	}

	if (object->getPropInt64(OCTempProp::AITIMER) == 0){
		START_OBJECT_TIMER(_objectTimer, object, OCTempProp::AITIMER, 0, TIMER_BEAT_FOREVER, object->getPropInt32(attr_def::aiInterval), AI::onAIStart, AI::onAITick, AI::onAIEnd);
	}
	else{
		SLASSERT(false, "already has a ai timer");
	}
	ECHO_TRACE("start ai %s for object %lld", ai, object->getID());
}

void AI::stopAI(sl::api::IKernel* pKernel, IObject* object){
	_objectTimer->stopTimer(object, OCTempProp::AITIMER);
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

	auto aiItor = _ais.find(ai);
	if (aiItor == _ais.end() || !aiItor->second.machine){
		SLASSERT(false, "ai %s is not exist", ai);
		return;
	}

	aiItor->second.machine->run(pKernel, object);
}

void AI::onAIEnd(sl::api::IKernel* pKernel, IObject* object, bool, int64){
}
