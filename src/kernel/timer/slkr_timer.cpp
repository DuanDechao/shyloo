#include "slkr_timer.h"
namespace sl
{
namespace core
{
static CObjectPool<CKrTimer> g_objPool("CKrTimer");
CObjectPool<CKrTimer>& CKrTimer::ObjPool()
{
	return g_objPool;
}

CKrTimer* CKrTimer::createPoolObject()
{
	return g_objPool.FetchObj();
}

void CKrTimer::reclaimPoolObject(CKrTimer* obj)
{
	g_objPool.ReleaseObj(obj);
}

void CKrTimer::destroyObjPool()
{
	g_objPool.Destroy();
}

size_t CKrTimer::getPoolObjectBytes()
{
	size_t bytes = sizeof(m_pITimer);
	return bytes;
}

CKrTimer::SmartPoolObjectPtr CKrTimer::createSmartPoolObj()
{
	return SmartPoolObjectPtr(new SmartPoolObject<CKrTimer>(ObjPool().FetchObj(), g_objPool));
}

void CKrTimer::onInit(int64 timetick)
{
	m_pITimer->onInit(Kernel::getSingletonPtr(), timetick);
}

void CKrTimer::onStart(int64 timetick)
{
	m_pITimer->onStart(Kernel::getSingletonPtr(), timetick);
}

void CKrTimer::onTime(int64 timetick)
{
	m_pITimer->onTime(Kernel::getSingletonPtr(), timetick);
}

void CKrTimer::onTerminate(int64 timetick)
{
	m_pITimer->onTerminate(Kernel::getSingletonPtr(), timetick);
}

void CKrTimer::onPause(int64 timetick)
{
	m_pITimer->onPause(Kernel::getSingletonPtr(), timetick);
}

void CKrTimer::onResume(int64 timetick)
{
	m_pITimer->onResume(Kernel::getSingletonPtr(), timetick);
}

void CKrTimer::setITimer(api::ITimer* pITimer) 
{
	m_pITimer = pITimer; 
}

}
}