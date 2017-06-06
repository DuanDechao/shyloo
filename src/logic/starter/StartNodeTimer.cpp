#include "StartNodeTimer.h"

StartNodeTimer* StartNodeTimer::create(int32 type){
	return CREATE_POOL_OBJECT(StartNodeTimer, type);
}

void StartNodeTimer::release(){
	RELEASE_POOL_OBJECT(StartNodeTimer, this);
}

void StartNodeTimer::onStart(sl::api::IKernel* pKernel, int64 timetick){
	Starter::getInstance()->onNodeTimerStart(pKernel, m_type, timetick);
}

void StartNodeTimer::onTime(sl::api::IKernel* pKernel, int64 timetick){
	Starter::getInstance()->onNodeTimer(pKernel, m_type, timetick);
}

void StartNodeTimer::onTerminate(sl::api::IKernel* pKernel, bool beForced, int64 timetick){
	Starter::getInstance()->onNodeTimerEnd(pKernel, m_type, timetick);
}