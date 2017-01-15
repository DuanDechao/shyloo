#ifndef SL_LOGIC_SLAVE_H
#define SL_LOGIC_SLAVE_H
#include "slikernel.h"
#include "IIdmgr.h"
class IHarbor;
class OArgs;
class IdMgr :public IIdMgr, sl::api::ITimer
{
public:
	virtual bool initialize(sl::api::IKernel * pKernel);
	virtual bool launched(sl::api::IKernel * pKernel);
	virtual bool destory(sl::api::IKernel * pKernel);

	virtual void onStart(sl::api::IKernel* pKernel, int64 timetick){}
	virtual void onTime(sl::api::IKernel* pKernel, int64 timetick);
	virtual void onTerminate(sl::api::IKernel* pKernel, int64 timetick){}
	virtual void onPause(sl::api::IKernel* pKernel, int64 timetick){}
	virtual void onResume(sl::api::IKernel* pKernel, int64 timetick) {}

	virtual uint64 allocID();
	static void askIds(sl::api::IKernel* pKernel, int32 nodeType, int32 nodeId, const OArgs& args);
	static void giveIds(sl::api::IKernel* pKernel, int32 nodeType, int32 nodeId, const OArgs& args);

private:
	uint64 generateId();

private:
	static IdMgr*				s_self;
	static IHarbor*				s_harbor;
	static int32				s_svrNodeType;
	static int32				s_areaId;
	static int32				s_poolSize;
	static std::vector<uint64>	s_idPool;
	static bool					s_bIsMultiProcess;
};

#endif