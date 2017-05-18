#include "PropDelaySender.h"
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

bool PropDelaySender::initialize(sl::api::IKernel * pKernel){
	_self = this;
	_kernel = pKernel;
	return true;
}

bool PropDelaySender::launched(sl::api::IKernel * pKernel){
	FIND_MODULE(_harbor, Harbor);
	FIND_MODULE(_eventEngine, EventEngine); 
	FIND_MODULE(_packetSender, PacketSender);

	RGS_EVENT_HANDLER(_eventEngine, logic_event::EVENT_LOGIC_DATA_LOAD_COMPLETED, PropDelaySender::onPlayerOnline);
	RGS_EVENT_HANDLER(_eventEngine, logic_event::EVENT_LOGIC_PLAYER_RECONNECT, PropDelaySender::onPlayerReconnect);

	return true;
}

bool PropDelaySender::destory(sl::api::IKernel * pKernel){
	DEL this;
	return true;
}

void PropDelaySender::syncChangedProps(IObject* object){
	sendToSelf(_kernel, object);
	sendToOthers(_kernel, object);
}

void PropDelaySender::removeChangedProp(IObject* object, const IProp* prop){
	ITableControl* sharedProps = object->findTable(OCTableMacro::PROP_SHARED_TABLE::TABLE_NAME);
	SLASSERT(sharedProps, "wtf");
	
	const IRow* row = sharedProps->findRow((int64)prop);
	if (row)
		DEL_TABLE_ROW(sharedProps, row);
}

void PropDelaySender::onPlayerOnline(sl::api::IKernel* pKernel, const void* context, const int32 size){
	SLASSERT(size == sizeof(logic_event::Biology), "wtf");
	logic_event::Biology* info = (logic_event::Biology*)context;

	RGS_PROP_CHANGER(info->object, ANY_CALL, PropDelaySender::syncToSelf);
	RGS_PROP_CHANGER(info->object, ANY_CALL, PropDelaySender::syncToOthers);
}

void PropDelaySender::onPlayerReconnect(sl::api::IKernel* pKernel, const void* context, const int32 size){
	SLASSERT(size == sizeof(logic_event::Biology), "wtf");
	logic_event::Biology* info = (logic_event::Biology*)context;

	ITableControl* selfProps = info->object->findTable(OCTableMacro::PROP_SELF_TABLE::TABLE_NAME);
	SLASSERT(selfProps, "wtf");
	selfProps->clearRows();

	ITableControl* sharedProps = info->object->findTable(OCTableMacro::PROP_SHARED_TABLE::TABLE_NAME);
	SLASSERT(sharedProps, "wtf");
	sharedProps->clearRows();
}

void PropDelaySender::syncToSelf(sl::api::IKernel* pKernel, IObject* object, const char* name, const IProp* prop, const bool sync){
	int32 setting = prop->getSetting(object);
	if (setting & prop_def::visible){
		ITableControl* propSelf = object->findTable(OCTableMacro::PROP_SELF_TABLE::TABLE_NAME);
		SLASSERT(propSelf, "wtf");

		const IRow* findRow = propSelf->findRow((int64)prop);
		if (!findRow)
			propSelf->addRowKeyInt64((int64)prop);
	}
}
void PropDelaySender::syncToOthers(sl::api::IKernel* pKernel, IObject* object, const char* name, const IProp* prop, const bool sync){
	int32 setting = prop->getSetting(object);
	if (setting & prop_def::share){
		ITableControl* propShared = object->findTable(OCTableMacro::PROP_SHARED_TABLE::TABLE_NAME);
		SLASSERT(propShared, "wtf");

		const IRow* findRow = propShared->findRow((int64)prop);
		if (!findRow)
			propShared->addRowKeyInt64((int64)prop);
	}
}

void PropDelaySender::sendToSelf(sl::api::IKernel* pKernel, IObject* object){
	ITableControl* selfProps = object->findTable(OCTableMacro::PROP_SELF_TABLE::TABLE_NAME);
	SLASSERT(selfProps, "wtf");
	int32 propCount = selfProps->rowCount();
	if (propCount > 0){
		sl::IBStream<game::MAX_PACKET_SIZE> args;
		args << (int64)object->getID() << propCount;
		for (int32 i = 0; i < propCount; i++){
			const IRow* row = selfProps->getRow(i);
			SLASSERT(row, "wtf");
			const IProp* prop = (const IProp*)row->getDataInt64(OCTableMacro::PROP_SELF_TABLE::PROP);
			int16 idxAndType = prop->getIndex(object);
			switch (prop->getType(object)){
			case DTYPE_INT8: idxAndType |= ((int16)protocol::AttribType::DTYPE_INT8) << 13; args << idxAndType << object->getPropInt8(prop); break;
			case DTYPE_INT16: idxAndType |= ((int16)protocol::AttribType::DTYPE_INT16) << 13; args << idxAndType << object->getPropInt16(prop); break;
			case DTYPE_INT32: idxAndType |= ((int16)protocol::AttribType::DTYPE_INT32) << 13; args << idxAndType << object->getPropInt32(prop); break;
			case DTYPE_INT64: idxAndType |= ((int16)protocol::AttribType::DTYPE_INT64) << 13; args << idxAndType << object->getPropInt64(prop); break;
			case DTYPE_STRING: idxAndType |= ((int16)protocol::AttribType::DTYPE_STRING) << 13; args << idxAndType << object->getPropString(prop); break;
			case DTYPE_FLOAT: idxAndType |= ((int16)protocol::AttribType::DTYPE_FLOAT) << 13; args << idxAndType << object->getPropFloat(prop); break;
			default: SLASSERT(false, "invaild type %d", prop->getType(object)); break;
			}
		}

		_packetSender->send(object->getPropInt32(attr_def::gate), object->getID(), ServerMsgID::SERVER_MSG_ATTRIB_SYNC, args.out());
		selfProps->clearRows();
	}
}

void PropDelaySender::sendToOthers(sl::api::IKernel* pKernel, IObject* object){
	ITableControl* sharedProps = object->findTable(OCTableMacro::PROP_SHARED_TABLE::TABLE_NAME);
	SLASSERT(sharedProps, "wtf");
	int32 propCount = sharedProps->rowCount();
	if (propCount > 0){
		sl::IBStream<game::MAX_PACKET_SIZE> args;
		args << (int64)object->getID() << propCount;
		for (int32 i = 0; i < propCount; i++){
			const IRow* row = sharedProps->getRow(i);
			SLASSERT(row, "wtf");
			const IProp* prop = (const IProp*)row->getDataInt64(OCTableMacro::PROP_SELF_TABLE::PROP);
			int16 idxAndType = prop->getIndex(object);
			switch (prop->getType(object)){
			case DTYPE_INT8: idxAndType |= ((int16)protocol::AttribType::DTYPE_INT8) << 13; args << idxAndType << object->getPropInt8(prop); break;
			case DTYPE_INT16: idxAndType |= ((int16)protocol::AttribType::DTYPE_INT16) << 13; args << idxAndType << object->getPropInt16(prop); break;
			case DTYPE_INT32: idxAndType |= ((int16)protocol::AttribType::DTYPE_INT32) << 13; args << idxAndType << object->getPropInt32(prop); break;
			case DTYPE_INT64: idxAndType |= ((int16)protocol::AttribType::DTYPE_INT64) << 13; args << idxAndType << object->getPropInt64(prop); break;
			case DTYPE_STRING: idxAndType |= ((int16)protocol::AttribType::DTYPE_STRING) << 13; args << idxAndType << object->getPropString(prop); break;
			case DTYPE_FLOAT: idxAndType |= ((int16)protocol::AttribType::DTYPE_FLOAT) << 13; args << idxAndType << object->getPropFloat(prop); break;
			default: SLASSERT(false, "invaild type %d", prop->getType(object)); break;
			}
		}

		//TODO 向关注者广播
		
		sharedProps->clearRows();
	}
}
