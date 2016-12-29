#include "StartNodeTimer.h"

void StartNodeTimer::onStart(sl::api::IKernel* pKernel, int64 timetick){
	m_starter->startTimerInit(pKernel, m_type);
}
void StartNodeTimer::onTime(sl::api::IKernel* pKernel, int64 timetick){
	m_starter->startTimerOnTime(pKernel, m_type);
}
void StartNodeTimer::onTerminate(sl::api::IKernel* pKernel, int64 timetick){
	RELEASE_POOL_OBJECT(StartNodeTimer, this);
}