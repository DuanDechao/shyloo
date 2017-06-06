#include "slkr_timer.h"
namespace sl
{
namespace core
{
CKrTimer::~CKrTimer(){
	m_pITimer = nullptr;
	m_timerHander = INVALID_TIMER_HANDER;
}

void CKrTimer::onInit(int64 timetick){
	m_pITimer->onInit(Kernel::getInstance(), timetick);
}

void CKrTimer::onStart(int64 timetick){
	m_pITimer->onStart(Kernel::getInstance(), timetick);
}

void CKrTimer::onTime(int64 timetick){
	m_pITimer->onTime(Kernel::getInstance(), timetick);
}

void CKrTimer::onTerminate(bool beForced, int64 timetick){
	m_pITimer->onTerminate(Kernel::getInstance(), beForced, timetick);
}

void CKrTimer::onPause(int64 timetick){
	m_pITimer->onPause(Kernel::getInstance(), timetick);
}

void CKrTimer::onResume(int64 timetick){
	m_pITimer->onResume(Kernel::getInstance(), timetick);
}

void CKrTimer::setITimer(api::ITimer* pITimer) {
	m_pITimer = pITimer; 
}

}
}