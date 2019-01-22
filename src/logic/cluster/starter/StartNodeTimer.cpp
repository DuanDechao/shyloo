#include "StartNodeTimer.h"

sl::SLPool<StartNodeTimer> StartNodeTimer::s_pool;
void StartNodeTimer::onStart(sl::api::IKernel* pKernel, int64 timetick){
	Starter::getInstance()->onNodeTimerStart(pKernel, _type, timetick);
}

void StartNodeTimer::onTime(sl::api::IKernel* pKernel, int64 timetick){
	Starter::getInstance()->onNodeTimer(pKernel, _type, timetick);
}

void StartNodeTimer::onTerminate(sl::api::IKernel* pKernel, bool beForced, int64 timetick){
	Starter::getInstance()->onNodeTimerEnd(pKernel, _type, timetick);
}