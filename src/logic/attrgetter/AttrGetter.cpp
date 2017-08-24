#define ATTR_EXPORT
#include "AttrGetter.h"
#include "Attr.h"
#include "IDCCenter.h"

void getAttrProp(IObjectMgr* objectMgr);

bool AttrGetter::initialize(sl::api::IKernel * pKernel){
	_kernel = pKernel;
	FIND_MODULE(_objectMgr, ObjectMgr);

	getAttrProp(_objectMgr);
	return true;
}

bool AttrGetter::launched(sl::api::IKernel * pKernel){
	return true;
}

bool AttrGetter::destory(sl::api::IKernel * pKernel){
	DEL this;
	return true;
}

const IProp* attr_def::account = nullptr;
const IProp* attr_def::agent = nullptr;
const IProp* attr_def::ai = nullptr;
const IProp* attr_def::aiInterval = nullptr;
const IProp* attr_def::appear = nullptr;
const IProp* attr_def::exp = nullptr;
const IProp* attr_def::firstLogin = nullptr;
const IProp* attr_def::gate = nullptr;
const IProp* attr_def::hp = nullptr;
const IProp* attr_def::id = nullptr;
const IProp* attr_def::logic = nullptr;
const IProp* attr_def::maxHp = nullptr;
const IProp* attr_def::maxMp = nullptr;
const IProp* attr_def::mp = nullptr;
const IProp* attr_def::name = nullptr;
const IProp* attr_def::occupation = nullptr;
const IProp* attr_def::offlineTime = nullptr;
const IProp* attr_def::oldX = nullptr;
const IProp* attr_def::oldY = nullptr;
const IProp* attr_def::oldZ = nullptr;
const IProp* attr_def::recoverTimer = nullptr;
const IProp* attr_def::scene = nullptr;
const IProp* attr_def::sceneId = nullptr;
const IProp* attr_def::sex = nullptr;
const IProp* attr_def::startTime = nullptr;
const IProp* attr_def::state = nullptr;
const IProp* attr_def::status = nullptr;
const IProp* attr_def::type = nullptr;
const IProp* attr_def::vision = nullptr;
const IProp* attr_def::x = nullptr;
const IProp* attr_def::y = nullptr;
const IProp* attr_def::z = nullptr;

const IProp* OCTempProp::AITIMER = nullptr;
const IProp* OCTempProp::AI_REF = nullptr;
const IProp* OCTempProp::DAY_CHANGE_TIMER = nullptr;
const IProp* OCTempProp::IS_APPEAR = nullptr;
const IProp* OCTempProp::MONTH_CHANGE_TIMER = nullptr;
const IProp* OCTempProp::PROP_UPDATE_TIMER = nullptr;
const IProp* OCTempProp::RGS_SHADOW = nullptr;
const IProp* OCTempProp::SCENE_X_NODE = nullptr;
const IProp* OCTempProp::SCENE_Y_NODE = nullptr;
const IProp* OCTempProp::SCENE_Z_NODE = nullptr;
const IProp* OCTempProp::SHADOW_SYNC_TIMER = nullptr;
const IProp* OCTempProp::SYNCTOSCENE = nullptr;
const IProp* OCTempProp::SYNCTOSCENE_TIMER = nullptr;
const IProp* OCTempProp::WEEK_CHANGE_TIMER = nullptr;

void getAttrProp(IObjectMgr* objectMgr){
	attr_def::account = objectMgr->getPropByName("account");
	attr_def::agent = objectMgr->getPropByName("agent");
	attr_def::ai = objectMgr->getPropByName("ai");
	attr_def::aiInterval = objectMgr->getPropByName("aiInterval");
	attr_def::appear = objectMgr->getPropByName("appear");
	attr_def::exp = objectMgr->getPropByName("exp");
	attr_def::firstLogin = objectMgr->getPropByName("firstLogin");
	attr_def::gate = objectMgr->getPropByName("gate");
	attr_def::hp = objectMgr->getPropByName("hp");
	attr_def::id = objectMgr->getPropByName("id");
	attr_def::logic = objectMgr->getPropByName("logic");
	attr_def::maxHp = objectMgr->getPropByName("maxHp");
	attr_def::maxMp = objectMgr->getPropByName("maxMp");
	attr_def::mp = objectMgr->getPropByName("mp");
	attr_def::name = objectMgr->getPropByName("name");
	attr_def::occupation = objectMgr->getPropByName("occupation");
	attr_def::offlineTime = objectMgr->getPropByName("offlineTime");
	attr_def::oldX = objectMgr->getPropByName("oldX");
	attr_def::oldY = objectMgr->getPropByName("oldY");
	attr_def::oldZ = objectMgr->getPropByName("oldZ");
	attr_def::recoverTimer = objectMgr->getPropByName("recoverTimer");
	attr_def::scene = objectMgr->getPropByName("scene");
	attr_def::sceneId = objectMgr->getPropByName("sceneId");
	attr_def::sex = objectMgr->getPropByName("sex");
	attr_def::startTime = objectMgr->getPropByName("startTime");
	attr_def::state = objectMgr->getPropByName("state");
	attr_def::status = objectMgr->getPropByName("status");
	attr_def::type = objectMgr->getPropByName("type");
	attr_def::vision = objectMgr->getPropByName("vision");
	attr_def::x = objectMgr->getPropByName("x");
	attr_def::y = objectMgr->getPropByName("y");
	attr_def::z = objectMgr->getPropByName("z");

	OCTempProp::AITIMER = objectMgr->getTempPropByName("AITIMER");
	OCTempProp::AI_REF = objectMgr->getTempPropByName("AI_REF");
	OCTempProp::DAY_CHANGE_TIMER = objectMgr->getTempPropByName("DAY_CHANGE_TIMER");
	OCTempProp::IS_APPEAR = objectMgr->getTempPropByName("IS_APPEAR");
	OCTempProp::MONTH_CHANGE_TIMER = objectMgr->getTempPropByName("MONTH_CHANGE_TIMER");
	OCTempProp::PROP_UPDATE_TIMER = objectMgr->getTempPropByName("PROP_UPDATE_TIMER");
	OCTempProp::RGS_SHADOW = objectMgr->getTempPropByName("RGS_SHADOW");
	OCTempProp::SCENE_X_NODE = objectMgr->getTempPropByName("SCENE_X_NODE");
	OCTempProp::SCENE_Y_NODE = objectMgr->getTempPropByName("SCENE_Y_NODE");
	OCTempProp::SCENE_Z_NODE = objectMgr->getTempPropByName("SCENE_Z_NODE");
	OCTempProp::SHADOW_SYNC_TIMER = objectMgr->getTempPropByName("SHADOW_SYNC_TIMER");
	OCTempProp::SYNCTOSCENE = objectMgr->getTempPropByName("SYNCTOSCENE");
	OCTempProp::SYNCTOSCENE_TIMER = objectMgr->getTempPropByName("SYNCTOSCENE_TIMER");
	OCTempProp::WEEK_CHANGE_TIMER = objectMgr->getTempPropByName("WEEK_CHANGE_TIMER");
}

