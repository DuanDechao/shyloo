#include "DataLand.h"
#include "sltools.h"
#include "sltime.h"
#include "IDB.h"
#include "SQLBase.h"
#include "CacheDB.h"
#include "IEventEngine.h"
#include "IHarbor.h"
#include "EventID.h"
#include "GameDefine.h"
#include "NodeDefine.h"
#include "NodeProtocol.h"

#define LAND_DATA_PROC_MAX_NUM 1000
bool DataLand::initialize(sl::api::IKernel * pKernel){
	_kernel = pKernel;
	_self = this;
	return true;
}

bool DataLand::launched(sl::api::IKernel * pKernel){
	FIND_MODULE(_harbor, Harbor);
	if (_harbor->getNodeType() == NodeType::DATABASE){
		FIND_MODULE(_db, DB);
		FIND_MODULE(_cacheDB, CacheDB);
		FIND_MODULE(_eventEngine, EventEngine);

		RGS_NODE_ARGS_HANDLER(_harbor, NodeProtocol::CLUSTER_MSG_ASK_DATA_LAND, DataLand::onClusterAskDataLand);

		RGS_EVENT_HANDLER(_eventEngine, logic_event::EVENT_SHUTDOWN_NOTIFY, DataLand::onShutdownNotify);

		START_TIMER(_self, 0, TIMER_BEAT_FOREVER, 20 * SECOND);
	}
	
	return true;
}

bool DataLand::destory(sl::api::IKernel * pKernel){
	DEL this;
	return true;
}

void DataLand::askLand(const char* table, const int32 keyCount, const OArgs& data, const char* key, int8 opt, bool sync){
	sl::BStream<2048> args;
	//args << opt << sync << table << key << keyCount << data;
	_harbor->send(NodeType::DATABASE, 1, NodeProtocol::CLUSTER_MSG_ASK_DATA_LAND, args.out());
}

void DataLand::onClusterAskDataLand(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId, const OArgs& args){
	int8 opt = args.getInt8(0);
	bool sync = args.getBool(1);
	const char* table = args.getString(2);
	const char* key = args.getString(3);
	int32 keyCount = args.getInt32(4);

	int32 totalArgsCount = args.getCount();
	SLASSERT(totalArgsCount >= keyCount, "wtf");

	std::set<int32> updateCols;
	for (int32 colIdx = keyCount + 5; colIdx < totalArgsCount; colIdx++){
		updateCols.insert(CacheDB::getInstance()->getColumnIdx(table, args.getString(colIdx)));
	}

	for (int32 keyIdx = 5; keyIdx < keyCount + 5; keyIdx++){
		int64 keyIntVal = 0;
		const char* keyStrVal = nullptr;
		switch (args.getType(keyIdx)){
		case ARGS_TYPE_INT8: keyIntVal = args.getInt8(keyIdx); break;
		case ARGS_TYPE_INT16: keyIntVal = args.getInt16(keyIdx); break;
		case ARGS_TYPE_INT32: keyIntVal = args.getInt32(keyIdx); break;
		case ARGS_TYPE_INT64: keyIntVal = args.getInt64(keyIdx); break;
		case ARGS_TYPE_STRING: keyStrVal = args.getString(keyIdx); break;
		default: SLASSERT(false, "wtf"); break;
		}

		if (keyStrVal)
			appendLandDataList(table, key, keyStrVal, opt, updateCols, sync);
		else
			appendLandDataList(table, key, keyIntVal, opt, updateCols, sync);
	}
}

template<typename TYPE>
void DataLand::appendLandDataList(const char* table, const char* key, TYPE keyVal, int8 opt, std::set<int32>& cols, bool sync){
	int32 id = LandData::calcId(table, key, keyVal);
	auto itor = _datasMap.find(id);
	
	LandData* dataNode = nullptr;
	if (itor == _datasMap.end() || itor->second->getOpt() > opt){
		dataNode = NEW LandData(table, key, keyVal, opt, sl::getTimeMilliSecond());
		dataNode->setUpdateCols(cols);

		if (itor == _datasMap.end() && sync){
			//没有更高等级操作的任务在前面，并且要求同步数据库
			landDataToDB(dataNode);
			DEL dataNode;
		}
		else{
			_datasMap[dataNode->getId()] = dataNode;
			_landDatas.pushBack(dataNode);
		}
	}
	else{
		dataNode = itor->second;
		SLASSERT((dataNode)->checkIdInfo(table, key, keyVal), "wtf");

		std::set<int32>& updateCols = dataNode->getUpdateCols();
		std::set<int32> newCols;
		std::set_union(updateCols.begin(), updateCols.end(), cols.begin(), cols.end(), inserter(newCols, newCols.begin()));
		dataNode->setUpdateCols(newCols);

		if (sync){
			landDataToDB(dataNode);
			
			auto itor = _datasMap.find(dataNode->getId());
			if (itor != _datasMap.end() && itor->second == dataNode)
				_datasMap.erase(itor);

			_landDatas.remove(dataNode);
			
			DEL dataNode;
		}
	}
	
	ECHO_TRACE("ask land data[%s %s %d]", table, key, opt);
}

void DataLand::onTime(sl::api::IKernel* pKernel, int64 timetick){
	if (_landDatas.isEmpty())
		return;

	int32 writeCount = 0;
	while (!_landDatas.isEmpty()){
		LandData* data = (LandData*)_landDatas.front();
		if (writeCount > LAND_DATA_PROC_MAX_NUM)
			break;
		
		ECHO_TRACE("land data[%s %s %d]...", data->getTableName(), data->getKeyName(), data->getOpt());
		landDataToDB(data);
		writeCount++;

		auto itor = _datasMap.find(data->getId());
		if (itor != _datasMap.end() && itor->second == data)
			_datasMap.erase(itor);

		_landDatas.popFront();

		DEL data;
	}
}

void DataLand::landDataToDB(LandData* data){
	SLASSERT(data->getKeyType() == TYPE_INTEGER || data->getKeyType() == TYPE_STRING, "wtf");
	
	std::set<int32>& updateCols = data->getUpdateCols();
	auto readColsFunc = [&](sl::api::IKernel* pKernel, ICacheDBReader* reader){
		auto addFunc = [&reader, &data](int32 col){
			reader->readColumn(CacheDB::getInstance()->getColumnByIdx(data->getTableName(), col));
		};
		std::for_each(updateCols.begin(), updateCols.end(), addFunc);
	};
	
	switch (data->getOpt()){
	case DB_OPT::DB_OPT_UPDATE:{
		auto readResultFunc = [&](sl::api::IKernel* pKernel, ICacheDBReadResult* result){
			if (result->count() > 0){
				updateToDB(data, updateCols, result);
			}
		};

		if (data->getKeyType() == TYPE_INTEGER)
			_cacheDB->readByIndex(data->getTableName(), readColsFunc, readResultFunc, data->getKeyIntVal());
		else if (data->getKeyType() == TYPE_STRING)
			_cacheDB->readByIndex(data->getTableName(), readColsFunc, readResultFunc, data->getKeyStrVal());
		else{
			SLASSERT(false, "unknown Type");
		}
		
		break;
	}

	case DB_OPT::DB_OPT_SAVE:{
		auto saveResultFunc = [&](sl::api::IKernel* pKernel, ICacheDBReadResult* result){
			if (result->count() > 0){
				saveToDB(data, updateCols, result);
			}
		};

		if (data->getKeyType() == TYPE_INTEGER)
			_cacheDB->read(data->getTableName(), readColsFunc, saveResultFunc, 1, data->getKeyIntVal());
		else if (data->getKeyType() == TYPE_STRING)
			_cacheDB->read(data->getTableName(), readColsFunc, saveResultFunc, 1, data->getKeyStrVal());
		else{
			SLASSERT(false, "unknown Type");
		}

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
		auto readFunc = [&idx, &result, &data, &adder](int32 col){
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
		auto readFunc = [&idx, &result, &data, &adder](int32 col){
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

		landDataToDB(data);

		auto itor = _datasMap.find(data->getId());
		if (itor != _datasMap.end() && itor->second == data)
			_datasMap.erase(itor);

		DEL data;
	}

	SLASSERT(_datasMap.empty() && _landDatas.isEmpty(), "wtf");
}


