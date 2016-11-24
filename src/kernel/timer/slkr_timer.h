#ifndef SL_KERNEL_TIMER_H
#define SL_KERNEL_TIMER_H
#include "slikernel.h"
#include "sltimer.h"
#include "slkernel.h"
#include "slobjectpool.h"
namespace sl
{
namespace core
{
class CKrTimer: public timer::ISLTimer, public api::ITimerBase, public PoolObject
{
public:
	typedef SLShared_ptr<SmartPoolObject<CKrTimer>> SmartPoolObjectPtr;
	static SmartPoolObjectPtr createSmartPoolObj();
	static CObjectPool<CKrTimer>& ObjPool();
	static CKrTimer* createPoolObject();
	static void reclaimPoolObject(CKrTimer* obj);
	static void destroyObjPool();
	void onReclaimObject();
	virtual size_t getPoolObjectBytes();

public:
	virtual void SLAPI onInit(int64 timetick);

	virtual void SLAPI onStart(int64 timetick);

	virtual void SLAPI onTime(int64 timetick);

	virtual void SLAPI onTerminate(int64 timetick);

	virtual void SLAPI onPause(int64 timetick);

	virtual void SLAPI onResume(int64 timetick);

public:
	CKrTimer(){}
	virtual ~CKrTimer() {}

	void setITimer(api::ITimer* pITimer);

private:
	api::ITimer*	m_pITimer;
};

}

}

#endif