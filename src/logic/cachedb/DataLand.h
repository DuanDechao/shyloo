#ifndef __SL_CORE_DATALAND_H__
#define __SL_CORE_DATALAND_H__
#include "slsingleton.h"
#include <unordered_map>
#include "slstring.h"
#include "GameDefine.h"
#include "slimodule.h"
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