#ifndef __SL_CORE_DATALAND_H__
#define __SL_CORE_DATALAND_H__
#include "slsingleton.h"
#include <unordered_map>
#include "slstring.h"
#include "GameDefine.h"
#include "slimodule.h"
#include "sllist.h"

//#define TYPE_INTEGER  1
//#define TYPE_STRING   2
//union KeyValue{
//	int64 valueInt;
//	char valueStr[64];
//};
//class DataLand : public sl::ISLListNode{
//	DataLand(const char* table, const char* key, int64 val, int8 opt):_tableName(table),_keyName(key),_opt(opt){
//		_key.valueInt = val;
//		_keyType = TYPE_INTEGER;
//		string idStr = string(table) + key;
//		_id = ((uint64)sl::CalcStringUniqueId(idStr.c_str()) << 32) | ()val;
//	}
//	DataLand(const char* table, const char* key, const char* val, int8 opt){
//		SafeSprintf(_key.valueStr, sizeof(_key.valueStr), "%s", val);
//		_keyType = TYPE_STRING;
//		string idStr = string(table) + key;
//		_id = ((uint64)sl::CalcStringUniqueId(idStr.c_str()) << 32) | val;
//	}
//private:
//	sl::SLString<64> _tableName;
//	sl::SLString<64> _keyName;
//	uint64 _id;
//	int8 _opt;
//	int8 _keyType;
//	KeyValue _key;
//};

class OArgs;
class IDB;
class DataLand : public sl::api::IModule, public sl::api::ITimer, public sl::SLHolder<DataLand>{
	struct LandData{
		int8 opt;
		sl::SLString<64> table;
		sl::SLString<64> key;
		int32 keyCount;
		char data[2048];
		int64 tick;
	};
public:
	virtual bool initialize(sl::api::IKernel * pKernel);
	virtual bool launched(sl::api::IKernel * pKernel);
	virtual bool destory(sl::api::IKernel * pKernel);

	virtual void askLand(const char* table, const int32 dataCount, const OArgs& data, const char* key, int8 opt);

	virtual void onStart(sl::api::IKernel* pKernel, int64 timetick){}
	virtual void onTime(sl::api::IKernel* pKernel, int64 timetick);
	virtual void onTerminate(sl::api::IKernel* pKernel, bool beForced, int64 timetick) {}
	virtual void onPause(sl::api::IKernel* pKernel, int64 timetick) {}
	virtual void onResume(sl::api::IKernel* pKernel, int64 timetick) {}

private:
	void landDataToDB(LandData& data);

private:
	sl::api::IKernel*		_kernel;
	DataLand*				_self;
	IDB*					_db;

	std::list<LandData>     _landDatas;
};
#endif