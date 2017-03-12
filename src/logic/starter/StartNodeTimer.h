#ifndef SL_START_NODE_TIMER_H
#define SL_START_NODE_TIMER_H
#include "slikernel.h"
#include "Starter.h"
#include "slobjectpool.h"
using namespace sl;
class StartNodeTimer : public sl::api::ITimer{
public:
	StartNodeTimer(){}
	StartNodeTimer(int32 type) :m_type(type){}

	static StartNodeTimer* create(int32 type);

	void release();

	virtual void onInit(sl::api::IKernel* pKernel, int64 timetick){}
	virtual void onStart(sl::api::IKernel* pKernel, int64 timetick);
	virtual void onTime(sl::api::IKernel* pKernel, int64 timetick);
	virtual void onTerminate(sl::api::IKernel* pKernel, int64 timetick);
	virtual void onPause(sl::api::IKernel* pKernel, int64 timetick){}
	virtual void onResume(sl::api::IKernel* pKernel, int64 timetick) {}

private:
	int32		m_type;
};

CREATE_OBJECT_POOL(StartNodeTimer);
#endif