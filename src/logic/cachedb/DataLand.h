#ifndef __SL_CORE_DATALAND_H__
#define __SL_CORE_DATALAND_H__
#include "slsingleton.h"
#include <unordered_map>
#include "slstring.h"
#include "GameDefine.h"
#include "slimodule.h"
#include "sllist.h"
#include <set>

#define TYPE_INTEGER  1
#define TYPE_STRING   2
union KeyValue{
	int64 valueInt;
	char valueStr[64];
};
class LandData : public sl::ISLListNode{
public:
	LandData(const char* table, const char* key, int64 val, int8 opt, int64 tick) :_tableName(table), _keyName(key),_id(0), _opt(opt), _keyType(0), _tick(tick){
		_key.valueInt = val;
		_keyType = TYPE_INTEGER;
		_id = calcId(table, key, val);
	}

	LandData(const char* table, const char* key, const char* val, int8 opt, int64 tick) :_tableName(table), _keyName(key), _opt(opt), _keyType(0), _tick(tick){
		SafeSprintf(_key.valueStr, sizeof(_key.valueStr), "%s", val);
		_keyType = TYPE_STRING;
		_id = calcId(table, key, val);
	}

	inline static int32 calcId(const char* table, const char* key, int64 val){
		string idStr = string(table) + key + sl::CStringUtils::Int64AsString(val);
		return sl::CalcStringUniqueId(idStr.c_str());
	}

	inline static int32 calcId(const char* table, const char* key, const char* val){
		string idStr = string(table) + key + val;
		return  sl::CalcStringUniqueId(idStr.c_str());
	}

	bool checkIdInfo(const char* tableName, const char* keyName, int64 keyVal){
		if (_keyType != TYPE_INTEGER){
			SLASSERT(false, "wtf");
			return false;
		}

		if (_tableName != tableName || _keyName != keyName || _key.valueInt != keyVal){
			SLASSERT(false, "wtf");
			return false;
		}

		return true;
	}

	bool checkIdInfo(const char* tableName, const char* keyName, const char* keyVal){
		if (_keyType != TYPE_STRING){
			SLASSERT(false, "wtf");
			return false;
		}

		if (_tableName != tableName || _keyName != keyName || strcmp(_key.valueStr, keyVal) != 0){
			SLASSERT(false, "wtf");
			return false;
		}

		return true;
	}

	inline int32 getId() const { return _id; }
	inline const char* getTableName() const { return _tableName.c_str(); }
	inline const char* getKeyName() const { return _keyName.c_str(); }
	inline int8 getKeyType() const { return _keyType; }
	inline int64 getKeyIntVal() const { return _key.valueInt; }
	inline const char* getKeyStrVal() const { return _key.valueStr; }
	
	inline void setTick(int64 tick) { _tick = tick; }
	inline int64 getTick() const { return _tick; }
	
	inline int8 getOpt() const { return _opt; }
	inline void setOpt(int8 opt){ _opt = opt; }

	inline std::set<int32>& getUpdateCols() { return _updateCols; }
	inline void setUpdateCols(std::set<int32>& cols) { _updateCols = cols; }

private:
	sl::SLString<64> _tableName;
	sl::SLString<64> _keyName;
	int32 _id;
	int8 _opt;
	int8 _keyType;
	KeyValue _key;
	int64 _tick;
	std::set<int32> _updateCols;
};

class OArgs;
class IDB;
class ICacheDB;
class ICacheDBReadResult;
class IEventEngine;
class IHarbor;
class DataLand : public sl::api::IModule, public sl::api::ITimer, public sl::SLHolder<DataLand>{
public:
	virtual bool initialize(sl::api::IKernel * pKernel);
	virtual bool launched(sl::api::IKernel * pKernel);
	virtual bool destory(sl::api::IKernel * pKernel);

	virtual void askLand(const char* table, const int32 dataCount, const OArgs& data, const char* key, int8 opt, bool sync = false);

	virtual void onStart(sl::api::IKernel* pKernel, int64 timetick){}
	virtual void onTime(sl::api::IKernel* pKernel, int64 timetick);
	virtual void onTerminate(sl::api::IKernel* pKernel, bool beForced, int64 timetick) {}
	virtual void onPause(sl::api::IKernel* pKernel, int64 timetick) {}
	virtual void onResume(sl::api::IKernel* pKernel, int64 timetick) {}

	void onClusterAskDataLand(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId, const OArgs& args);
	void onShutdownNotify(sl::api::IKernel* pKernel, const void* context, const int32 size);

private:
	template<typename TYPE> 
	void appendLandDataList(const char* table, const char* key, TYPE keyVal, int8 opt, std::set<int32>& cols, bool sync);

	void landDataToDB(LandData* data);

	void updateToDB(LandData* data, std::set<int32>& cols, ICacheDBReadResult* result);
	void saveToDB(LandData* data, std::set<int32>& cols, ICacheDBReadResult* result);
	void delToDB(LandData* data);

	void landAllData(sl::api::IKernel* pKernel);

private:
	sl::api::IKernel*		_kernel;
	DataLand*				_self;
	IHarbor*				_harbor;
	IDB*					_db;
	ICacheDB*				_cacheDB;
	IEventEngine*			_eventEngine;

	sl::SLList				_landDatas;
	std::unordered_map<int32, LandData*> _datasMap;
};
#endif
