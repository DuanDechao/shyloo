#ifndef SL_LOGIC_IDMGR_H
#define SL_LOGIC_IDMGR_H
#include "slikernel.h"
#include "IIdMgr.h"
#include "slsingleton.h"
class IHarbor;
class OArgs;
class IdMgr :public IIdMgr, public sl::api::ITimer, public sl::SLHolder<IdMgr>{
public:
	virtual bool initialize(sl::api::IKernel * pKernel);
	virtual bool launched(sl::api::IKernel * pKernel);
	virtual bool destory(sl::api::IKernel * pKernel);

	virtual void onStart(sl::api::IKernel* pKernel, int64 timetick){}
	virtual void onTime(sl::api::IKernel* pKernel, int64 timetick);
	virtual void onTerminate(sl::api::IKernel* pKernel, bool beForced, int64 timetick){}
	virtual void onPause(sl::api::IKernel* pKernel, int64 timetick){}
	virtual void onResume(sl::api::IKernel* pKernel, int64 timetick) {}

	virtual uint64 allocID();
	void askIds(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId, const OArgs& args);
	void giveIds(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId, const OArgs& args);

private:
	uint64 generateId();

private:
	IdMgr*				_self;
	IHarbor*			_harbor;
	int32				_svrNodeType;
	int32				_areaId;
	int32				_poolSize;
	std::vector<uint64>	_idPool;
	bool				_bIsMultiProcess;
};

#endif