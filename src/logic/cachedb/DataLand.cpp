#include "DataLand.h"
#include "sltools.h"
#include "sltime.h"
#include "IDB.h"
#include "SQLBase.h"

#define LAND_DATA_DELAY_TIME (1 * MINUTE)
#define LAND_DATA_PRE_MAX_NUM 1000
bool DataLand::initialize(sl::api::IKernel * pKernel){
	_kernel = pKernel;
	_self = this;
	return true;
}

bool DataLand::launched(sl::api::IKernel * pKernel){
	FIND_MODULE(_db, DB);
	START_TIMER(_self, 0, TIMER_BEAT_FOREVER, 30 * SECOND);
	return true;
}

bool DataLand::destory(sl::api::IKernel * pKernel){
	DEL this;
	return true;
}

void DataLand::askLand(const char* table, const int32 keyCount, const OArgs& data, const char* key, int8 opt){
	LandData landData;
	landData.opt = opt;
	landData.table = table;
	landData.keyCount = keyCount;
	landData.key = key;
	sl::SafeMemcpy(landData.data, sizeof(landData.data), data.getContext(), data.getSize());
	landData.tick = sl::getTimeMilliSecond();
	_landDatas.push_back(landData);
	ECHO_TRACE("ask land data[%s %s %d]", table, key, opt);
}

void DataLand::onTime(sl::api::IKernel* pKernel, int64 timetick){
	if (_landDatas.empty())
		return;

	int32 writeCount = 0;
	while (!_landDatas.empty()){
		LandData& data = _landDatas.front();
		if (timetick - data.tick < LAND_DATA_DELAY_TIME || writeCount > LAND_DATA_PRE_MAX_NUM)
			break;

		landDataToDB(data);
		writeCount++;
		ECHO_TRACE("land data[%s %s %d]...", data.table.c_str(), data.key.c_str(), data.opt);

		_landDatas.pop_front();
	}
}

void DataLand::landDataToDB(LandData& data){
	const OArgs args(data.data, sizeof(data.data));
	int32 totalCount = args.getCount();
	SLASSERT(totalCount >= data.keyCount, "wtf");

	for (int32 keyIdx = 0; keyIdx < data.keyCount; keyIdx++){
		IDBCall* callor = CREATE_DB_CALL(_db, data.tick, 0);
		switch (data.opt){
		case DB_OPT::DB_OPT_UPDATE:{
			callor->update(data.table.c_str(), [&](sl::api::IKernel* pKernel, IDBUpdateParamAdder* adder, IDBCallCondition* condition){
				for (int32 dataIdx = data.keyCount; dataIdx < totalCount; dataIdx += 2){
					switch (args.getType(dataIdx + 1)){
					case ARGS_TYPE_INT8: adder->AddColumn(args.getString(dataIdx), args.getInt8(dataIdx + 1)); break;
					case ARGS_TYPE_INT16: adder->AddColumn(args.getString(dataIdx), args.getInt16(dataIdx + 1)); break;
					case ARGS_TYPE_INT32: adder->AddColumn(args.getString(dataIdx), args.getInt32(dataIdx + 1)); break;
					case ARGS_TYPE_INT64: adder->AddColumn(args.getString(dataIdx), args.getInt64(dataIdx + 1)); break;
					case ARGS_TYPE_STRING: adder->AddColumn(args.getString(dataIdx), args.getString(dataIdx + 1)); break;
					case ARGS_TYPE_STRUCT:{
							int32 size = 0;
							const void* val = args.getStruct(dataIdx + 1, size);
							adder->AddColumn(args.getString(dataIdx), val, size);
							break;
						}
					default: SLASSERT(false, "wtf"); break;
					}
				}

				switch (args.getType(keyIdx)){
				case ARGS_TYPE_INT8: condition->AddCondition(data.key.c_str(), IDBCallCondition::DBOP_EQ, args.getInt8(keyIdx)); break;
				case ARGS_TYPE_INT16: condition->AddCondition(data.key.c_str(), IDBCallCondition::DBOP_EQ, args.getInt16(keyIdx)); break;
				case ARGS_TYPE_INT32: condition->AddCondition(data.key.c_str(), IDBCallCondition::DBOP_EQ, args.getInt32(keyIdx)); break;
				case ARGS_TYPE_INT64: condition->AddCondition(data.key.c_str(), IDBCallCondition::DBOP_EQ, args.getInt64(keyIdx)); break;
				case ARGS_TYPE_STRING: condition->AddCondition(data.key.c_str(), IDBCallCondition::DBOP_EQ, args.getString(keyIdx)); break;
				default: SLASSERT(false, "wtf"); break;
				}
			}, nullptr);

			break;
		}

		case DB_OPT::DB_OPT_SAVE:{
			callor->save(data.table.c_str(), [&](sl::api::IKernel* pKernel, IDBSaveParamAdder* adder){
				for (int32 dataIdx = data.keyCount; dataIdx < totalCount; dataIdx += 2){
					switch (args.getType(dataIdx + 1)){
					case ARGS_TYPE_INT8: adder->AddColumn(args.getString(dataIdx), args.getInt8(dataIdx + 1)); break;
					case ARGS_TYPE_INT16: adder->AddColumn(args.getString(dataIdx), args.getInt16(dataIdx + 1)); break;
					case ARGS_TYPE_INT32: adder->AddColumn(args.getString(dataIdx), args.getInt32(dataIdx + 1)); break;
					case ARGS_TYPE_INT64: adder->AddColumn(args.getString(dataIdx), args.getInt64(dataIdx + 1)); break;
					case ARGS_TYPE_STRING: adder->AddColumn(args.getString(dataIdx), args.getString(dataIdx + 1)); break;
					case ARGS_TYPE_STRUCT:{
							int32 size = 0;
							const void* val = args.getStruct(dataIdx + 1, size);
							adder->AddColumn(args.getString(dataIdx), val, size);
							break;
						}
					default: SLASSERT(false, "wtf"); break;
					}
				}

				switch (args.getType(keyIdx)){
				case ARGS_TYPE_INT8: adder->AddColumn(data.key.c_str(), args.getInt8(keyIdx)); break;
				case ARGS_TYPE_INT16: adder->AddColumn(data.key.c_str(), args.getInt16(keyIdx)); break;
				case ARGS_TYPE_INT32: adder->AddColumn(data.key.c_str(), args.getInt32(keyIdx)); break;
				case ARGS_TYPE_INT64: adder->AddColumn(data.key.c_str(), args.getInt64(keyIdx)); break;
				case ARGS_TYPE_STRING: adder->AddColumn(data.key.c_str(), args.getString(keyIdx)); break;
				default: SLASSERT(false, "wtf"); break;
				}
			}, nullptr);

			break;
		}

		case DB_OPT::DB_OPT_DELETE:{
			callor->del(data.table.c_str(), [&](sl::api::IKernel* pKernel, IDBCallCondition* condition){
				switch (args.getType(keyIdx)){
				case ARGS_TYPE_INT8: condition->AddCondition(data.key.c_str(), IDBCallCondition::DBOP_EQ, args.getInt8(keyIdx)); break;
				case ARGS_TYPE_INT16: condition->AddCondition(data.key.c_str(), IDBCallCondition::DBOP_EQ, args.getInt16(keyIdx)); break;
				case ARGS_TYPE_INT32: condition->AddCondition(data.key.c_str(), IDBCallCondition::DBOP_EQ, args.getInt32(keyIdx)); break;
				case ARGS_TYPE_INT64: condition->AddCondition(data.key.c_str(), IDBCallCondition::DBOP_EQ, args.getInt64(keyIdx)); break;
				case ARGS_TYPE_STRING: condition->AddCondition(data.key.c_str(), IDBCallCondition::DBOP_EQ, args.getString(keyIdx)); break;
				default: SLASSERT(false, "wtf"); break;
				}
			}, nullptr);
		
			break;
		}

		default: SLASSERT(false, "unkown opt type !"); break;
		}
	}
}



