#include "slkr_timer.h"
#include "slkernel.h"
namespace sl{
namespace core{

sl::SLPool<CKrTimer> CKrTimer::s_pool;
void CKrTimer::onInit(int64 timetick){
	_timer->onInit(Kernel::getInstance(), timetick);
}

void CKrTimer::onStart(int64 timetick){
	_timer->onStart(Kernel::getInstance(), timetick);
}

void CKrTimer::onTime(int64 timetick){
	_timer->onTime(Kernel::getInstance(), timetick);
}

void CKrTimer::onTerminate(bool beForced, int64 timetick){
	_timer->onTerminate(Kernel::getInstance(), beForced, timetick);
}

void CKrTimer::onPause(int64 timetick){
	_timer->onPause(Kernel::getInstance(), timetick);
}

void CKrTimer::onResume(int64 timetick){
	_timer->onResume(Kernel::getInstance(), timetick);
}

}
}