#include "StartNodeTimer.h"

void StartNodeTimer::onStart(sl::api::IKernel* pKernel, int64 timetick){
	Starter::startTimerInit(pKernel, m_type);
}
void StartNodeTimer::onTime(sl::api::IKernel* pKernel, int64 timetick){
	Starter::startTimerOnTime(pKernel, m_type);
}
void StartNodeTimer::onTerminate(sl::api::IKernel* pKernel, int64 timetick){
	RELEASE_POOL_OBJECT(StartNodeTimer, this);
}