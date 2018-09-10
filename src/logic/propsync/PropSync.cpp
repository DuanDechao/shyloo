#include "PropSync.h"
#include "IHarbor.h"
#include "IEventEngine.h"
#include "EventID.h"
#include "IDCCenter.h"
#include "Attr.h"
#include "slbinary_stream.h"
#include "GameDefine.h"
#include "Protocol.pb.h"
#include "IPacketSender.h"
#include "ProtocolID.pb.h"

bool PropSync::initialize(sl::api::IKernel * pKernel){
	_self = this;
	_kernel = pKernel;
	return true;
}

bool PropSync::launched(sl::api::IKernel * pKernel){
	if(SLMODULE(Harbor)->getNodeType() == NodeType::SCENE){
		RGS_EVENT_HANDLER(SLMODULE(EventEngine), logic_event::EVENT_CELL_ENTITY_CREATED, PropSync::onCellEntityCreated);
	}
	return true;
}

bool PropSync::destory(sl::api::IKernel * pKernel){
	DEL this;
	return true;
}

bool PropSync::syncAllProps(IObject* object){
	int32 selfSyncFlags = prop_def::ObjectDataFlag::BASE_AND_CLIENT;
	int32 otherSyncFlags = prop_def::ObjectDataFlag::ED_FLAG_UNKNOWN; 
	if(SLMODULE(Harbor)->getNodeType() == NodeType::SCENE){
		selfSyncFlags = prop_def::ObjectDataFlagRelation::OBJECT_CLIENT_OWN_DATA_FLAGS;
		otherSyncFlags = prop_defs::ObjectDataFlagRelation::OBJECT_CLIENT_OTHER_DATA_FLAGS;
	}
	
	const std::vector<const IProp*>& props = object->getObjProps();	
	const int32 propsSize = props.size();
	sl::BStream<game::MAX_PACKET_SIZE> selfPropData;
	sl::BStream<game::MAX_PACKET_SIZE> othersPropData;
	selfPropData << object->getID();
	othersPropData << object->getID();

	bool sendSelf = false, sendOthers = false;
	for(int32 idx = 0; idx < propsSize; idx++){
		int32 setting = props[idx]->getSetting(object);
		if(setting & selfSyncFlags){
			sendSelf= true;
			addPropDataToStream(object, props[idx], selfPropData);
		}
		if(setting & otherSyncFlags){
			sendOthers = true;
			addPropDataToStream(object, props[idx], othersPropData);
		}
	}

	if(sendSelf)
		SLMODULE(PacketSender)->sendToClient(object, selfPropData.out());

	if(sendOthers)
		SLMODULE(PacketSender)->sendToOthers(object, othersPropData.out());
}

bool PropSync::syncAllPropsToSelf(IObject* object){
	const std::vector<const IProp*>& props = object->getObjProps();	
	const int32 propsSize = props.size();
	sl::BStream<game::MAX_PACKET_SIZE> selfPropData;
}

void PropSync::onCellEntityCreated(sl::api::IKernel* pKernel, const void* context, const int32 size){
	SLASSERT(size == sizeof(logic_event::CellEntityCreatedFromCell), "wtf");
	logic_event::CellEntityCreatedFromCell* evt= (logic_event::CellEntityCreatedFromCell*)context;
	RGS_PROP_CHANGER(evt->object, ANY_CALL, PropSync::syncChangedProps);
}

void PropSync::syncChangedProps(sl::api::IKernel* pKernel, IObject* object, const char* name, const IProp* prop, const bool sync){
	int32 setting = prop->getSetting(object);
	if(setting & prop_def::ObjectDataFlagRelation::OBJECT_CLIENT_OWN_DATA_FLAGS)
		sendToSelf(pKernel, object, prop);
	
	if(setting & prop_def::ObjectDataFlagRelation::OBJECT_CLIENT_OTHERS_DATA_FLAGS)
		sendToOthers(pKernel, object, prop);
}

void PropSync::sendToSelf(sl::api::IKernel* pKernel, IObject* object, const IProp* prop){
	sl::BStream<game::MAX_PACKET_SIZE> args;
	args << object->getID();
	addPropDataToStream(object, prop, args);
	SLMODULE(PacketSender)->sendToClient(object, args.out());
}

void PropSync::sendToOthers(sl::api::IKernel* pKernel, IObject* object, const IProp* prop){
	sl::BStream<game::MAX_PACKET_SIZE> args;
	int32 setting = prop->getSetting(object);
	args << object->getID();
	addPropDataToStream(object, prop, args);
	SLMODULE(PacketSender)->sendToOthers(object, args.out());
}

void PropSync::addPropDataToStream(IObject* object, const IProp* prop, IBStream& stream){
	stream << (int16)prop->getIndex(object);
	switch (prop->getType(object)):
	case DTYPE_INT8: stream << object->getPropInt8(prop); break;
	case DTYPE_INT16: stream << object->getPropInt16(prop); break;
	case DTYPE_INT32: stream << object->getPropInt32(prop); break;
	case DTYPE_INT64: stream << object->getPropInt64(prop); break;
	case DTYPE_UINT8: stream << object->getPropUint8(prop); break;
	case DTYPE_UINT16: stream << object->getPropUint16(prop); break;
	case DTYPE_UINT32: stream << object->getPropUint32(prop); break;
	case DTYPE_UINT64: stream << object->getPropUint64(prop); break;
	case DTYPE_STRING: stream << object->getPropString(prop); break;
	case DTYPE_FLOAT: stream << object->getPropFloat(prop); break;
	case DTYPE_BLOB: {
			int32 size = 0;
			const char* data =(const char*)(object->getPropBlob(prop, size));
			sl::OBStream out(data, size);
			stream << out;
			break;
		} 
	default: SLASSERT(false, "invaild type %d", prop->getType(object)); break;
	}
}
