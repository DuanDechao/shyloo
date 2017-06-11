#include "DataLand.h"
#include "sltools.h"
#include "sltime.h"
#include "IDB.h"
#include "SQLBase.h"
#include "CacheDB.h"
#include "IEventEngine.h"
#include "EventID.h"
#include "GameDefine.h"

#define LAND_DATA_DELAY_TIME (1 * MINUTE)
#define LAND_DATA_PRE_MAX_NUM 1000
bool DataLand::initialize(sl::api::IKernel * pKernel){
	_kernel = pKernel;
	_self = this;
	return true;
}

bool DataLand::launched(sl::api::IKernel * pKernel){
	FIND_MODULE(_db, DB);
	FIND_MODULE(_cacheDB, CacheDB);
	FIND_MODULE(_eventEngine, EventEngine);

	RGS_EVENT_HANDLER(_eventEngine, logic_event::EVENT_SHUTDOWN_NOTIFY, DataLand::onShutdownNotify);

	START_TIMER(_self, 0, TIMER_BEAT_FOREVER, 30 * SECOND);
	return true;
}

bool DataLand::destory(sl::api::IKernel * pKernel){
	DEL this;
	return true;
}

void DataLand::askLand(const char* table, const int32 keyCount, const OArgs& data, const char* key, int8 opt){
	int32 totalCount = data.getCount();
	SLASSERT(totalCount >= keyCount, "wtf");

	std::set<int32> updateCols;
	for (int32 colIdx = keyCount; colIdx < totalCount; colIdx++){
		updateCols.insert(CacheDB::getInstance()->getColumnIdx(table, data.getString(colIdx)));
	}

	for (int32 keyIdx = 0; keyIdx < keyCount; keyIdx++){
		int64 keyIntVal = 0;
		const char* keyStrVal = nullptr;
		switch (data.getType(keyIdx)){
		case ARGS_TYPE_INT8: keyIntVal = data.getInt8(keyIdx); break;
		case ARGS_TYPE_INT16: keyIntVal = data.getInt16(keyIdx); break;
		case ARGS_TYPE_INT32: keyIntVal = data.getInt32(keyIdx); break;
		case ARGS_TYPE_INT64: keyIntVal = data.getInt64(keyIdx); break;
		case ARGS_TYPE_STRING: keyStrVal = data.getString(keyIdx); break;
		default: SLASSERT(false, "wtf"); break;
		}

		if (keyStrVal)
			appendLandDataList(table, key, keyStrVal, opt, updateCols);
		else
			appendLandDataList(table, key, keyIntVal, opt, updateCols);
		
	}
}

template<typename TYPE>
void DataLand::appendLandDataList(const char* table, const char* key, TYPE keyVal, int8 opt, std::set<int32>& cols){
	int32 id = LandData::calcId(table, key, keyVal);
	auto itor = _datasMap.find(id);
	
	sl::ISLListNode* dataNode = nullptr;
	if (itor == _datasMap.end() || ((LandData*)itor->second)->getOpt() > opt){
		dataNode = NEW LandData(table, key, keyVal, opt, sl::getTimeMilliSecond());
		((LandData*)dataNode)->setUpdateCols(cols);
		_datasMap[((LandData*)dataNode)->getId()] = dataNode;
	}
	else{
		dataNode = itor->second;
		SLASSERT(((LandData*)dataNode)->checkIdInfo(table, key, keyVal), "wtf");
		
		_landDatas.remove(dataNode);

		std::set<int32>& updateCols = ((LandData*)dataNode)->getUpdateCols();
		std::set<int32> newCols;
		std::set_union(updateCols.begin(), updateCols.end(), cols.begin(), cols.end(), inserter(newCols, newCols.begin()));
		((LandData*)dataNode)->setUpdateCols(newCols);
		((LandData*)dataNode)->setTick(sl::getTimeMilliSecond());
	}

	_landDatas.pushBack(dataNode);
	ECHO_TRACE("ask land data[%s %s %d]", table, key, opt);
}

void DataLand::onTime(sl::api::IKernel* pKernel, int64 timetick){
	if (_landDatas.isEmpty())
		return;

	int32 writeCount = 0;
	while (!_landDatas.isEmpty()){
		LandData* data = (LandData*)_landDatas.front();
		if (timetick - data->getTick() < LAND_DATA_DELAY_TIME || writeCount > LAND_DATA_PRE_MAX_NUM)
			break;
		
		ECHO_TRACE("land data[%s %s %d]...", data->getTableName(), data->getKeyName(), data->getOpt());

		if (data->getKeyType() == TYPE_INTEGER)
			landDataToDB(data, data->getKeyIntVal());
		else if (data->getKeyType() == TYPE_STRING)
			landDataToDB(data, data->getKeyStrVal());
		else{
			SLASSERT(false, "unknown Type");
		}
		
		writeCount++;

		auto itor = _datasMap.find(data->getId());
		if (itor != _datasMap.end() && itor->second == data)
			_datasMap.erase(itor);

		_landDatas.popFront();

		DEL data;
	}
}

template<typename TYPE>
void DataLand::landDataToDB(LandData* data, TYPE keyVal){
	SLASSERT(data->getKeyType() == TYPE_INTEGER || data->getKeyType() == TYPE_STRING, "wtf");
	switch (data->getOpt()){
	case DB_OPT::DB_OPT_UPDATE:{
		std::set<int32>& updateCols = data->getUpdateCols();
		_cacheDB->readByIndex(data->getTableName(), [&](sl::api::IKernel* pKernel, ICacheDBReader* reader){
			auto& addFunc = [&reader,&data](int32 col){
				reader->readColumn(CacheDB::getInstance()->getColumnByIdx(data->getTableName(), col));
			};
			std::for_each(updateCols.begin(), updateCols.end(), addFunc);
									   
		}, [&](sl::api::IKernel* pKernel, ICacheDBReadResult* result){
			if (result->count() > 0){
				updateToDB(data, updateCols, result);
			}
		}, keyVal);
		break;
	}

	case DB_OPT::DB_OPT_SAVE:{
		std::set<int32>& updateCols = data->getUpdateCols();
		_cacheDB->read(data->getTableName(), [&](sl::api::IKernel* pKernel, ICacheDBReader* reader){
			auto& addFunc = [&reader, &data](int32 col){
				reader->readColumn(CacheDB::getInstance()->getColumnByIdx(data->getTableName(), col));
			};
			std::for_each(updateCols.begin(), updateCols.end(), addFunc);

		}, [&](sl::api::IKernel* pKernel, ICacheDBReadResult* result){
			if (result->count() > 0){
				saveToDB(data, updateCols, result);
			}
		}, 1, keyVal);
		break;
								
	}

	case DB_OPT::DB_OPT_DELETE:{
		delToDB(data);
		break;
	}

	default: SLASSERT(false, "unkown opt type !"); break;
	}
}

void DataLand::updateToDB(LandData* data, std::set<int32>& cols, ICacheDBReadResult* result){
	IDBCall* callor = CREATE_DB_CALL(_db, data->getTick(), 0);
	callor->update(data->getTableName(), [&](sl::api::IKernel* pKernel, IDBUpdateParamAdder* adder, IDBCallCondition* condition){
		int32 idx = 0;
		auto& readFunc = [&idx, &result, &data, &adder](int32 col){
			const char* colStr = CacheDB::getInstance()->getColumnByIdx(data->getTableName(), col);
			int8 colType = CacheDB::getInstance()->getColumnType(data->getTableName(), colStr);
			switch (colType){
			case CacheDB::CDB_TYPE_INT8: adder->AddColumn(colStr, result->getInt8(0, idx)); break;
			case CacheDB::CDB_TYPE_INT16: adder->AddColumn(colStr, result->getInt16(0, idx)); break;
			case CacheDB::CDB_TYPE_INT32: adder->AddColumn(colStr, result->getInt32(0, idx)); break;
			case CacheDB::CDB_TYPE_INT64: adder->AddColumn(colStr, result->getInt64(0, idx)); break;
			case CacheDB::CDB_TYPE_STRING: adder->AddColumn(colStr, result->getString(0, idx)); break;
			case CacheDB::CDB_TYPE_FLOAT: adder->AddColumn(colStr, result->getFloat(0, idx)); break;
			case CacheDB::CDB_TYPE_STRUCT: { int32 size = 0; const void* val = result->getBinary(0, idx, size); adder->AddColumn(colStr, val, size); break; }
			case CacheDB::CDB_TYPE_BLOB: { int32 size = 0; const void* val = result->getBinary(0, idx, size); adder->AddColumn(colStr, val, size); break; }
			default: SLASSERT(false, "wtf"); break;
			};
			++idx;
		};

		std::for_each(cols.begin(), cols.end(), readFunc);

		if (data->getKeyType() == TYPE_INTEGER)
			condition->AddCondition(data->getKeyName(), IDBCallCondition::DBOP_EQ, data->getKeyIntVal());
		else if (data->getKeyType() == TYPE_STRING)
			condition->AddCondition(data->getKeyName(), IDBCallCondition::DBOP_EQ, data->getKeyStrVal());
		else {
			SLASSERT(false, "wtf");
		}
	}, nullptr);

	if (data->getKeyType() == TYPE_INTEGER){
		ECHO_TRACE("land data update[%s %s %lld] success", data->getTableName(), data->getKeyName(), data->getKeyIntVal());
	}
	
	if (data->getKeyType() == TYPE_STRING){
		ECHO_TRACE("land data update[%s %s %s] success", data->getTableName(), data->getKeyName(), data->getKeyStrVal());
	}
}

void DataLand::saveToDB(LandData* data, std::set<int32>& cols, ICacheDBReadResult* result){
	IDBCall* callor = CREATE_DB_CALL(_db, data->getTick(), 0);
	callor->save(data->getTableName(), [&](sl::api::IKernel* pKernel, IDBSaveParamAdder* adder){
		int32 idx = 0;
		auto& readFunc = [&idx, &result, &data, &adder](int32 col){
			const char* colStr = CacheDB::getInstance()->getColumnByIdx(data->getTableName(), col);
			int8 colType = CacheDB::getInstance()->getColumnType(data->getTableName(), colStr);
			switch (colType){
			case CacheDB::CDB_TYPE_INT8: adder->AddColumn(colStr, result->getInt8(0, idx)); break;
			case CacheDB::CDB_TYPE_INT16: adder->AddColumn(colStr, result->getInt16(0, idx)); break;
			case CacheDB::CDB_TYPE_INT32: adder->AddColumn(colStr, result->getInt32(0, idx)); break;
			case CacheDB::CDB_TYPE_INT64: adder->AddColumn(colStr, result->getInt64(0, idx)); break;
			case CacheDB::CDB_TYPE_STRING: adder->AddColumn(colStr, result->getString(0, idx)); break;
			case CacheDB::CDB_TYPE_FLOAT: adder->AddColumn(colStr, result->getFloat(0, idx)); break;
			case CacheDB::CDB_TYPE_STRUCT: { int32 size = 0; const void* val = result->getBinary(0, idx, size); adder->AddColumn(colStr, val, size); break; }
			case CacheDB::CDB_TYPE_BLOB: { int32 size = 0; const void* val = result->getBinary(0, idx, size); adder->AddColumn(colStr, val, size); break; }
			default: SLASSERT(false, "wtf"); break;
			};
			++idx;
		};

		std::for_each(cols.begin(), cols.end(), readFunc);

		if (data->getKeyType() == TYPE_INTEGER)
			adder->AddColumn(data->getKeyName(), data->getKeyIntVal());
		else if (data->getKeyType() == TYPE_STRING)
			adder->AddColumn(data->getKeyName(), data->getKeyStrVal());
		else {
			SLASSERT(false, "wtf");
		}
	}, nullptr);

	if (data->getKeyType() == TYPE_INTEGER){
		ECHO_TRACE("land data save[%s %s %lld] success", data->getTableName(), data->getKeyName(), data->getKeyIntVal());
	}

	if (data->getKeyType() == TYPE_STRING){
		ECHO_TRACE("land data save[%s %s %s] success", data->getTableName(), data->getKeyName(), data->getKeyStrVal());
	}
}

void DataLand::delToDB(LandData* data){
	IDBCall* callor = CREATE_DB_CALL(_db, data->getTick(), 0);
	callor->del(data->getTableName(), [&](sl::api::IKernel* pKernel, IDBCallCondition* condition){
		if (data->getKeyType() == TYPE_INTEGER)
			condition->AddCondition(data->getKeyName(), IDBCallCondition::DBOP_EQ, data->getKeyIntVal());
		else if (data->getKeyType() == TYPE_STRING)
			condition->AddCondition(data->getKeyName(), IDBCallCondition::DBOP_EQ, data->getKeyStrVal());
		else {
			SLASSERT(false, "wtf");
		}
	}, nullptr);

	if (data->getKeyType() == TYPE_INTEGER){
		ECHO_TRACE("land data delete[%s %s %lld] success", data->getTableName(), data->getKeyName(), data->getKeyIntVal());
	}

	if (data->getKeyType() == TYPE_STRING){
		ECHO_TRACE("land data delete[%s %s %s] success", data->getTableName(), data->getKeyName(), data->getKeyStrVal());
	}
}

void DataLand::onShutdownNotify(sl::api::IKernel* pKernel, const void* context, const int32 size){
	SLASSERT(size == sizeof(logic_event::ShutDown), "wtf");
	logic_event::ShutDown* evt = (logic_event::ShutDown*)context;
	if (evt->step == ShutdownStep::SHUTDOWN_DB_NOTIFY){
		landAllData(pKernel);
		_eventEngine->execEvent(logic_event::EVENT_SHUTDOWN_COMPLETE, evt, size);
	}
}

void DataLand::landAllData(sl::api::IKernel* pKernel){
	while (!_landDatas.isEmpty()){
		LandData* data = (LandData*)_landDatas.popFront();

		ECHO_TRACE("land data[%s %s %d]...", data->getTableName(), data->getKeyName(), data->getOpt());

		if (data->getKeyType() == TYPE_INTEGER)
			landDataToDB(data, data->getKeyIntVal());
		else if (data->getKeyType() == TYPE_STRING)
			landDataToDB(data, data->getKeyStrVal());
		else{
			SLASSERT(false, "unknown Type");
		}

		auto itor = _datasMap.find(data->getId());
		if (itor != _datasMap.end() && itor->second == data)
			_datasMap.erase(itor);

		DEL data;
	}

	SLASSERT(_datasMap.empty() && _landDatas.isEmpty(), "wtf");
}


