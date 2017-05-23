#include "ShadowMgr.h"
#include "IHarbor.h"
#include "NodeProtocol.h"
#include "Attr.h"
#include "IDCCenter.h"
#include "NodeDefine.h"
#include "IDCCenter.h"
#include "EventID.h"
#include "IEventEngine.h"
#include "IObjectTimer.h"
#include "slxml_reader.h"

bool ShadowMgr::initialize(sl::api::IKernel * pKernel){
	_self = this;
	_kernel = pKernel;

	sl::XmlReader svrConf;
	if (!svrConf.loadXml(pKernel->getCoreFile())){
		SLASSERT(false, "cant load core file");
		return false;
	}
	_syncInterval = svrConf.root()["sync_rate"][0].getAttributeInt64("shadow");

	return true;
}

bool ShadowMgr::launched(sl::api::IKernel * pKernel){
	FIND_MODULE(_harbor, Harbor);
	FIND_MODULE(_objectMgr, ObjectMgr);
	FIND_MODULE(_eventEngine, EventEngine);
	FIND_MODULE(_objectTimer, ObjectTimer);
	
	RGS_NODE_HANDLER(_harbor, NodeProtocol::LOGIC_MSG_CREATE_SHADOW, ShadowMgr::onLogicCreateShadow);
	RGS_NODE_HANDLER(_harbor, NodeProtocol::LOGIC_MSG_SYNC_SHADOW, ShadowMgr::onLogicSyncShadow);
	RGS_NODE_ARGS_HANDLER(_harbor, NodeProtocol::LOGIC_MSG_DESTROY_SHADOW, ShadowMgr::onLogicDestroyShadow);

	RGS_EVENT_HANDLER(_eventEngine, logic_event::EVENT_LOGIC_DATA_LOAD_COMPLETED, ShadowMgr::onStartShadowPropSyncTimer);
	RGS_EVENT_HANDLER(_eventEngine, logic_event::EVENT_LOGIC_SCENE_OBJECT_APPEAR, ShadowMgr::onStartShadowPropSyncTimer);
	RGS_EVENT_HANDLER(_eventEngine, logic_event::EVENT_LOGIC_SCENE_OBJECT_DESTROY, ShadowMgr::onStopShadowPropSyncTimer);
	RGS_EVENT_HANDLER(_eventEngine, logic_event::EVENT_LOGIC_SCENE_OBJECT_DESTROY, ShadowMgr::onRemoveAllShadows);

	return true;
}
bool ShadowMgr::destory(sl::api::IKernel * pKernel){
	DEL this;
	return true;
}

void ShadowMgr::createShadow(IObject* object, const int32 logic){
	ITableControl* shadows = object->findTable(OCTableMacro::SHADOW::TABLE_NAME);
	SLASSERT(shadows, "wtf");
	int32 count = 0;
	const IRow* row = shadows->findRow(logic);
	if (row)
		count = row->getDataInt32(OCTableMacro::SHADOW::COUNT);
	else
		row = shadows->addRowKeyInt32(logic);

	if (_harbor->getNodeId() == logic && count == 0)
		sendCreateShadow(_kernel, object, logic);

	row->setDataInt32(OCTableMacro::SHADOW::COUNT, count + 1);

	if (object->getTempInt8(OCTempProp::RGS_SHADOW) == 0){
		RGS_PROP_CHANGER(object, ANY_CALL, ShadowMgr::syncShadow);
		object->setTempInt8(OCTempProp::RGS_SHADOW, 1);
	}
		
}

void ShadowMgr::removeShadow(IObject* object, const int32 logic){
	ITableControl* shadows = object->findTable(OCTableMacro::SHADOW::TABLE_NAME);
	SLASSERT(shadows, "wtf");
	const IRow* row = shadows->findRow(logic);
	if (!row)
		return;

	int32 count = row->getDataInt32(OCTableMacro::SHADOW::COUNT);

	if (logic != _harbor->getNodeId() && count == 1)
		sendDestroyShadow(_kernel, object, logic);

	row->setDataInt32(OCTableMacro::SHADOW::COUNT, count - 1);
}

void ShadowMgr::onLogicCreateShadow(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId, const sl::OBStream& args){
	const void* context = args.getContext();
	const int32 size = args.getSize();
	SLASSERT(size >= sizeof(shadow::CreateShadow), "wtf");
	shadow::CreateShadow* info = (shadow::CreateShadow*)context;
	shadow::Attribute* attr = (shadow::Attribute*)((const char*)context + sizeof(shadow::CreateShadow));

	SLASSERT(_objectMgr->findObject(info->id) == nullptr, "wtf");

	IObject* object = CREATE_OBJECT_BYID(_objectMgr, info->objectTypeName, info->id, true);
	SLASSERT(object, "wtf");

	for (int32 i = 0; i < info->count; i++){
		const IProp* prop = _objectMgr->getPropByNameId(attr[i].name);
		switch (attr[i].type){
		case DTYPE_INT8: object->setPropInt8(prop, attr[i].data.valueInt8);
		case DTYPE_INT16: object->setPropInt16(prop, attr[i].data.valueInt16);
		case DTYPE_INT32: object->setPropInt32(prop, attr[i].data.valueInt32);
		case DTYPE_INT64: object->setPropInt64(prop, attr[i].data.valueInt64);
		case DTYPE_STRING: object->setPropString(prop, attr[i].data.valueString);
		default: break;
		}
	}

	logic_event::Biology evt{ object };
	_eventEngine->execEvent(logic_event::EVENT_LOGIC_SHADOW_CREATED, &evt, sizeof(evt));
}

void ShadowMgr::onLogicSyncShadow(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId, const sl::OBStream& args){
	const void* context = args.getContext();
	const int32 size = args.getSize();
	SLASSERT(size >= sizeof(shadow::SyncShadow), "wtf");
	shadow::SyncShadow* info = (shadow::SyncShadow*)context;
	shadow::Attribute* attr = (shadow::Attribute*)((const char*)context + sizeof(shadow::SyncShadow));

	IObject* shadow = _objectMgr->findObject(info->id);
	SLASSERT(shadow && shadow->isShadow(), "wtf");

	for (int32 i = 0; i < info->count; i++){
		const IProp* prop = _objectMgr->getPropByNameId(attr[i].name);
		switch (attr[i].type){
		case DTYPE_INT8: shadow->setPropInt8(prop, attr[i].data.valueInt8);
		case DTYPE_INT16: shadow->setPropInt16(prop, attr[i].data.valueInt16);
		case DTYPE_INT32: shadow->setPropInt32(prop, attr[i].data.valueInt32);
		case DTYPE_INT64: shadow->setPropInt64(prop, attr[i].data.valueInt64);
		case DTYPE_STRING: shadow->setPropString(prop, attr[i].data.valueString);
		default: break;
		}
	}
}

void ShadowMgr::onLogicDestroyShadow(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId, const OArgs& args){
	int64 id = args.getInt64(0);
	IObject* shadow = _objectMgr->findObject(id);
	SLASSERT(shadow && shadow->isShadow(), "wtf");

	_objectMgr->recover(shadow);
}

void ShadowMgr::syncShadow(sl::api::IKernel* pKernel, IObject* object, const char* name, const IProp* prop, const bool sync){
	int32 setting = prop->getSetting(object);
	if (!(setting & prop_def::copy))
		return;
	
	ITableControl* shadowSyncProps = object->findTable(OCTableMacro::SHADOW_SYNC_PROP::TABLE_NAME);
	SLASSERT(shadowSyncProps, "wtf");

	const IRow* row = shadowSyncProps->findRow((int64)prop);
	if (!row)
		shadowSyncProps->addRowKeyInt64((int64)prop);
}

void ShadowMgr::onStartShadowPropSyncTimer(sl::api::IKernel* pKernel, const void* context, const int32 size){
	SLASSERT(size == sizeof(logic_event::Biology), "wtf");
	IObject * object = ((logic_event::Biology*)context)->object;
	SLASSERT(object, "wtf");

	if (object->getTempInt64(OCTempProp::SHADOW_SYNC_TIMER) == 0)
		START_OBJECT_TIMER(_objectTimer, object, OCTempProp::SHADOW_SYNC_TIMER, 0, TIMER_BEAT_FOREVER, _syncInterval, ShadowMgr::onSyncStart, ShadowMgr::onSyncTime, ShadowMgr::onSyncTerminate);
}

void ShadowMgr::onStopShadowPropSyncTimer(sl::api::IKernel* pKernel, const void* context, const int32 size){
	SLASSERT(size == sizeof(logic_event::Biology), "wtf");
	IObject * object = ((logic_event::Biology*)context)->object;
	SLASSERT(object, "wtf");

	_objectTimer->stopTimer(object, OCTempProp::SHADOW_SYNC_TIMER);
}

void ShadowMgr::onRemoveAllShadows(sl::api::IKernel* pKernel, const void* context, const int32 size){
	SLASSERT(size == sizeof(logic_event::Biology), "wtf");
	IObject * object = ((logic_event::Biology*)context)->object;
	SLASSERT(object, "wtf");

	brocastShadowPropSync(pKernel, object, [&](sl::api::IKernel* pKernel, const int32 logic){
		sendDestroyShadow(pKernel, object, logic);
	});
}

void ShadowMgr::onSyncTime(sl::api::IKernel* pKernel, IObject* object, int64){
	ITableControl* shadowSyncProps = object->findTable(OCTableMacro::SHADOW_SYNC_PROP::TABLE_NAME);
	SLASSERT(shadowSyncProps, "wtf");

	shadow::SyncShadow info;
	info.id = object->getID();
	info.count = shadowSyncProps->rowCount();

	brocastShadowPropSync(pKernel, object, [&](sl::api::IKernel* pKernel, const int32 logic){
		_harbor->prepareSend(NodeType::LOGIC, logic, NodeProtocol::LOGIC_MSG_SYNC_SHADOW, sizeof(shadow::SyncShadow) + info.count * sizeof(shadow::Attribute));
		_harbor->send(NodeType::LOGIC, logic, &info, sizeof(info));
	});

	for (int32 i = 0; i < shadowSyncProps->rowCount(); i++){
		const IRow* row = shadowSyncProps->getRow(i);
		const IProp* prop = (const IProp*)row->getDataInt64(OCTableMacro::SHADOW_SYNC_PROP::PROP);
		SLASSERT(prop && (prop->getSetting(object) & prop_def::copy), "wtf");

		shadow::Attribute attr;
		attr.name = prop->getName();
		attr.type = prop->getType(object);
		switch (attr.type){
		case DTYPE_INT8: attr.data.valueInt8 = object->getPropInt8(prop); break;
		case DTYPE_INT16: attr.data.valueInt16 = object->getPropInt16(prop); break;
		case DTYPE_INT32: attr.data.valueInt32 = object->getPropInt32(prop); break;
		case DTYPE_INT64: attr.data.valueInt64 = object->getPropInt64(prop); break;
		case DTYPE_STRING: SafeSprintf(attr.data.valueString, game::MAX_PROP_VALUE_LEN - 1, "%s", object->getPropString(prop)); break;
		default: break;
		}

		brocastShadowPropSync(pKernel, object, [&](sl::api::IKernel* pKernel, const int32 logic){
			_harbor->send(NodeType::LOGIC, logic, &attr, sizeof(attr));
		});
	}
}

void ShadowMgr::onSyncTerminate(sl::api::IKernel* pKernel, IObject* object, bool, int64){
	ITableControl* shadowSyncProps = object->findTable(OCTableMacro::SHADOW_SYNC_PROP::TABLE_NAME);
	SLASSERT(shadowSyncProps, "wtf");
	shadowSyncProps->clearRows();
}

void ShadowMgr::sendCreateShadow(sl::api::IKernel* pKernel, IObject* object, const int32 logic){
	shadow::CreateShadow info;
	info.id = object->getID();
	SafeSprintf(info.objectTypeName, game::MAX_OBJECT_TYPE_LEN - 1, "%s", object->getObjTypeString());
	info.count = 0;
	for (auto prop : object->getObjProps()){
		int32 setting = prop->getSetting(object);
		if (setting & prop_def::copy)
			info.count++;
	}

	_harbor->prepareSend(NodeType::LOGIC, logic, NodeProtocol::LOGIC_MSG_CREATE_SHADOW, sizeof(info)+info.count * sizeof(shadow::Attribute));
	_harbor->send(NodeType::LOGIC, logic, &info, sizeof(info));

	for (auto prop : object->getObjProps()){
		int32 setting = prop->getSetting(object);
		if (!(setting & prop_def::copy))
			continue;

		shadow::Attribute attr;
		attr.name = prop->getName();
		attr.type = prop->getType(object);
		switch (attr.type){
		case DTYPE_INT8: attr.data.valueInt8 = object->getPropInt8(prop); break;
		case DTYPE_INT16: attr.data.valueInt16 = object->getPropInt16(prop); break;
		case DTYPE_INT32: attr.data.valueInt32 = object->getPropInt32(prop); break;
		case DTYPE_INT64: attr.data.valueInt64 = object->getPropInt64(prop); break;
		case DTYPE_STRING: SafeSprintf(attr.data.valueString, game::MAX_PROP_VALUE_LEN -1, "%s", object->getPropString(prop)); break;
		default: break;
		}

		_harbor->send(NodeType::LOGIC, logic, &attr, sizeof(attr));
	}
}

void ShadowMgr::sendDestroyShadow(sl::api::IKernel* pKernel, IObject* object, const int32 logic){
	IArgs<1, 64> args;
	args << object->getID();
	args.fix();

	_harbor->send(NodeType::LOGIC, logic, NodeProtocol::LOGIC_MSG_DESTROY_SHADOW, args.out());
}

void ShadowMgr::brocastShadowPropSync(sl::api::IKernel* pKernel, IObject* object, const std::function<void(sl::api::IKernel* pKernel, const int32 logic)>& f){
	ITableControl* shadows = object->findTable(OCTableMacro::SHADOW::TABLE_NAME);
	SLASSERT(shadows, "wtf");

	for (int32 i = 0; i < shadows->rowCount(); i++){
		const IRow* row = shadows->getRow(i);
		int32 count = row->getDataInt32(OCTableMacro::SHADOW::COUNT);
		int32 logic = row->getDataInt32(OCTableMacro::SHADOW::LOGIC);
		if (count > 0 && logic != _harbor->getNodeId())
			f(pKernel, logic);
	}
}