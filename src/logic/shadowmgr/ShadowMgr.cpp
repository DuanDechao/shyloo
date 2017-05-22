#include "ShadowMgr.h"
#include "IHarbor.h"
#include "NodeProtocol.h"
#include "Attr.h"
#include "IDCCenter.h"
#include "NodeDefine.h"
#include "IDCCenter.h"
#include "EventID.h"
#include "IEventEngine.h"

bool ShadowMgr::initialize(sl::api::IKernel * pKernel){
	_self = this;
	_kernel = pKernel;
	return true;
}

bool ShadowMgr::launched(sl::api::IKernel * pKernel){
	FIND_MODULE(_harbor, Harbor);
	FIND_MODULE(_objectMgr, ObjectMgr);
	FIND_MODULE(_eventEngine, EventEngine);
	
	RGS_NODE_HANDLER(_harbor, NodeProtocol::LOGIC_MSG_CREATE_SHADOW, ShadowMgr::onLogicCreateShadow);
	RGS_NODE_HANDLER(_harbor, NodeProtocol::LOGIC_MSG_SYNC_SHADOW, ShadowMgr::onLogicSyncShadow);
	RGS_NODE_HANDLER(_harbor, NodeProtocol::LOGIC_MSG_DESTROY_SHADOW, ShadowMgr::onLogicDestroyShadow);

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

void ShadowMgr::delShadow(IObject* object, const int32 logic){
	ITableControl* shadows = object->findTable(OCTableMacro::SHADOW::TABLE_NAME);
	SLASSERT(shadows, "wtf");
	int32 count = 0;
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
	SLASSERT(size == sizeof(shadow::Attribute) + sizeof(int64), "wtf");
	int64 id = *(int64*)context;
	shadow::Attribute* attr = (shadow::Attribute*)((const char*)context + sizeof(int64));

	IObject* shadow = _objectMgr->findObject(id);
	SLASSERT(shadow && shadow->isShadow(), "wtf");

	const IProp* prop = _objectMgr->getPropByNameId(attr->name);
	switch (attr->type){
	case DTYPE_INT8: shadow->setPropInt8(prop, attr->data.valueInt8);
	case DTYPE_INT16: shadow->setPropInt16(prop, attr->data.valueInt16);
	case DTYPE_INT32: shadow->setPropInt32(prop, attr->data.valueInt32);
	case DTYPE_INT64: shadow->setPropInt64(prop, attr->data.valueInt64);
	case DTYPE_STRING: shadow->setPropString(prop, attr->data.valueString);
	default: break;
	}
}

void ShadowMgr::onLogicSyncShadow(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId, const sl::OBStream& args){
	
}

void ShadowMgr::syncShadow(sl::api::IKernel* pKernel, IObject* object, const char* name, const IProp* prop, const bool sync){
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
		_harbor->prepareSend(NodeType::LOGIC, logic, NodeProtocol::LOGIC_MSG_SYNC_SHADOW, sizeof(int64) + sizeof(shadow::Attribute));
		int64 id = object->getID();
		_harbor->send(NodeType::LOGIC, logic, &id, sizeof(int64));
		_harbor->send(NodeType::LOGIC, logic, &attr, sizeof(attr));
	})
}

void ShadowMgr::sendCreateShadow(sl::api::IKernel* pKernel, IObject* object, const int32 logic){
	shadow::CreateShadow info;
	info.id = object->getID();
	SafeSprintf(info.objectTypeName, game::MAX_OBJECT_TYPE_LEN - 1, "%s", object->getObjTypeString());
	info.count = 0;
	for (auto prop : object->getObjProps(object)){
		int32 setting = prop->getSetting(object);
		if (setting & prop_def::copy)
			info.count++;
	}

	_harbor->prepareSend(NodeType::LOGIC, logic, NodeProtocol::LOGIC_MSG_CREATE_SHADOW, sizeof(info)+info.count * sizeof(shadow::Attribute));
	_harbor->send(NodeType::LOGIC, logic, &info, sizeof(info));

	for (auto prop : object->getObjProps(object)){
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