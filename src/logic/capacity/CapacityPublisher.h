#ifndef __SL_CORE_CAPACITY_PUBLISHER_H__
#define __SL_CORE_CAPACITY_PUBLISHER_H__
#include "ICapacity.h"
#include "slikernel.h"
#include "IHarbor.h"

class CapacityPublisher: public ICapacityPublisher, public INodeListener, public sl::api::ITimer{
public:
	virtual bool initialize(sl::api::IKernel * pKernel);
	virtual bool launched(sl::api::IKernel * pKernel);
	virtual bool destory(sl::api::IKernel * pKernel);

	virtual void increaseLoad(int32 load);
	virtual void decreaseLoad(int32 load);

	virtual void onStart(sl::api::IKernel* pKernel, int64 timetick) {}
	virtual void onTime(sl::api::IKernel* pKernel, int64 timetick);
	virtual void onTerminate(sl::api::IKernel* pKernel, bool beForced, int64 timetick) {}
	virtual void onPause(sl::api::IKernel* pKernel, int64 timetick) {}
	virtual void onResume(sl::api::IKernel* pKernel, int64 timetick) {}

	virtual void onOpen(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId, const char* ip, const int32 port);
	virtual void onClose(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId){}

private:
	CapacityPublisher* _self;
	IHarbor* _harbor;
	int32    _currLoad;
	int32    _lastLoad;
	int32	 _maxLoad;
};
#endif