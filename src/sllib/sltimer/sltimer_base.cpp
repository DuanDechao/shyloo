#include "sltimer_base.h"
namespace sl
{
namespace timer
{
static CObjectPool<CSLTimerBase> g_objPool("Channel");
CObjectPool<CSLTimerBase>& CSLTimerBase::ObjPool()
{
	return g_objPool;
}

CSLTimerBase* CSLTimerBase::createPoolObject()
{
	return g_objPool.FetchObj();
}

void CSLTimerBase::reclaimPoolObject(CSLTimerBase* obj)
{
	g_objPool.ReleaseObj(obj);
}

void CSLTimerBase::destroyObjPool()
{
	g_objPool.Destroy();
}

size_t CSLTimerBase::getPoolObjectBytes()
{
	size_t bytes = sizeof(m_stat) + sizeof(m_pauseStamp) +
		sizeof(m_pTimerObj) + sizeof(m_expireStamp) + sizeof(m_bDelay) + sizeof(m_iCount);

	return bytes;
}

CSLTimerBase::SmartPoolObjectPtr CSLTimerBase::createSmartPoolObj()
{
	return SmartPoolObjectPtr(new SmartPoolObject<CSLTimerBase>(ObjPool().FetchObj(), g_objPool));
}



void CSLTimerBase::onReclaimObject()
{
	m_stat = TIME_DESTORY;
	m_pauseStamp = 0;
	m_pTimerObj = nullptr;
	m_expireStamp = 0;
	m_bDelay = true;
	m_iCount = 0;
}

CSLTimerBase::TimerState CSLTimerBase::updateState()
{
	if(!good())
		return TimerState::TIME_DESTORY;

	if(m_bDelay)
	{
		onStart();

		return TimerState::TIME_RECREATE;
	}
	else{
		onTimer();

		if(m_iCount > 0)
			--m_iCount;

		if(m_iCount == 0)
			return TimerState::TIME_DESTORY;

		return TimerState::TIME_RECREATE;
	}
}

void CSLTimerBase::onInit()
{
	uint64 nowTime = timestamp() / stampsPerSecond();
	m_pTimerObj->onInit((int64)nowTime);
}

void CSLTimerBase::onStart()
{
	uint64 nowTime = timestamp() / stampsPerSecond();
	m_pTimerObj->onStart((int64)nowTime);
	m_bDelay = false;
}

void CSLTimerBase::onTimer()
{
	uint64 nowTime = timestamp() / stampsPerSecond();
	m_pTimerObj->onTime(nowTime);
}

void CSLTimerBase::onEnd()
{
	uint64 nowTime = timestamp() / stampsPerSecond();
	m_pTimerObj->onTerminate(nowTime);

	CSLTimerBase::reclaimPoolObject(this);
}


}
}