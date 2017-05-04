#ifndef _SL_INTERFACE_OCTIMER_H__
#define _SL_INTERFACE_OCTIMER_H__
#include "slikernel.h"
#include "slpool.h"
#include "IDCCenter.h"

class IProp;
class OCTimer: public sl::api::ITimer{
public:
	typedef std::function<void(sl::api::IKernel* pKernel, IObject* object, int64 tick)> START_FUNC_TYPE;
	typedef std::function<void(sl::api::IKernel* pKernel, IObject* object, int64 tick)> ONTIME_FUNC_TYPE;
	typedef std::function<void(sl::api::IKernel* pKernel, IObject* object, bool, int64 tick)> TERMINATE_FUNC_TYPE;
	

	static OCTimer* create(sl::api::IKernel* pKernel, IObject* object, const IProp* prop, const START_FUNC_TYPE& start, const ONTIME_FUNC_TYPE& onTime, const TERMINATE_FUNC_TYPE& terminate){
		OCTimer* timer = (OCTimer* )object->getTempInt64(prop);
		if (timer){
			SLASSERT(false, "wtf");
			pKernel->killTimer(timer);
		}

		return CREATE_FROM_POOL(s_ocTimerPool, object, prop, start, onTime, terminate);
	}

	void release(){
		s_ocTimerPool.recover(this);
	}

	virtual void onInit(sl::api::IKernel* pKernel, int64 timetick){
		ITableControl* checkTimers = _object->findTable(OCTableMacro::CHECK_TIMER::TABLE_NAME);
		SLASSERT(checkTimers, "wtf");
		checkTimers->addRowKeyInt64((int64)this);
	}
	
	virtual void onStart(sl::api::IKernel* pKernel, int64 timetick){
		if (_start)
			_start(pKernel, _object, timetick);
	}

	virtual void onTime(sl::api::IKernel* pKernel, int64 timetick){
		if (_onTime)
			_onTime(pKernel, _object, timetick);
	}

	virtual void onTerminate(sl::api::IKernel* pKernel, int64 timetick){
		ITableControl* checkTimers = _object->findTable(OCTableMacro::CHECK_TIMER::TABLE_NAME);
		SLASSERT(checkTimers, "wtf");
		const IRow* timerRow = checkTimers->findRow((int64)this);
		DEL_TABLE_ROW(checkTimers, timerRow);

		OCTimer* timer = (OCTimer*)_object->getTempInt64(_prop);
		SLASSERT(timer == this, "wtf");

		if (_terminate)
			_terminate(pKernel, _object, false, timetick);
	}

	virtual void onPause(sl::api::IKernel* pKernel, int64 timetick){}
	virtual void onResume(sl::api::IKernel* pKernel, int64 timetick){}

private:
	friend sl::SLPool<OCTimer>;
	OCTimer(IObject* object, const IProp* prop, const START_FUNC_TYPE& start, const ONTIME_FUNC_TYPE& time, const TERMINATE_FUNC_TYPE& terminate)
		:_object(object),
		_prop(prop),
		_start(start),
		_onTime(time),
		_terminate(terminate)
	{
		SLASSERT(_object && _prop && _onTime, "wtf");
	}

	~OCTimer(){}

private:
	IObject* _object;
	const IProp* _prop;
	const START_FUNC_TYPE _start;
	const ONTIME_FUNC_TYPE _onTime;
	const TERMINATE_FUNC_TYPE _terminate;

	static sl::SLPool<OCTimer> s_ocTimerPool;
};
#endif