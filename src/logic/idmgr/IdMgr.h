#ifndef SL_LOGIC_IDMGR_H
#define SL_LOGIC_IDMGR_H
#include "slikernel.h"
#include "IIdMgr.h"
#include "slsingleton.h"
class IHarbor;
class sl::OBStream;
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
	virtual uint64 generateLocalId();
	void askIds(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId, const sl::OBStream& args);
	void giveIds(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId, const sl::OBStream& args);

private:
	IdMgr*				_self;
	IHarbor*			_harbor;
	int32				_areaId;
	int32				_poolSize;
	std::vector<uint64>	_idPool;
	bool				_bIsMultiProcess;
};

#endif
