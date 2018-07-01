#ifndef SL_SCRIPT_TIMERS_H
#define SL_SCRIPT_TIMERS_H
#include "slikernel.h"
#include "slstring.h"
#include "Python.h"
#include <unordered_map>
#include "slpool.h"
class ScriptTimer: public sl::api::ITimer{
public:
	static ScriptTimer* create(PyObject* entity, int64 timerId, const char* callbackName, PyObject* userData){
		return CREATE_FROM_POOL(s_scriptTimerPool, entity, timerId, callbackName, userData);
	}

	void release(){
		s_scriptTimerPool.recover(this);
	}

	virtual void onStart(sl::api::IKernel* pKernel, int64 timetick){}
	virtual void onTime(sl::api::IKernel* pKernel, int64 timetick);
	virtual void onTerminate(sl::api::IKernel* pKernel, bool beForced, int64 timetick);
	virtual void onPause(sl::api::IKernel* pKernel, int64 timetick){}
	virtual void onResume(sl::api::IKernel* pKernel, int64 timetick){}

	inline int64 getTimerId() const {return _timerId;} 

private:
	friend sl::SLPool<ScriptTimer>;

	ScriptTimer(PyObject* entity, int64 timerId, const char* callbackName, PyObject* userData)
		:_entity(entity),
		_timerId(timerId),
		_callBackName(callbackName),
		_userData(userData)
	{}
	~ScriptTimer(){}

private:
	PyObject*	_entity;
	sl::SLString<256> _callBackName;
	PyObject*	_userData;
	int64		_timerId;
	static sl::SLPool<ScriptTimer> s_scriptTimerPool;
};

class ScriptTimers{
public:
	ScriptTimers():_lastTimerId(1){}
	int64 addTimer(sl::api::IKernel* pKernel, PyObject* entity, int64 period, int64 interval, const char* callbackName, PyObject* userData);
	void delTimer(sl::api::IKernel* pKernel, PyObject* entity, int64 timerId);

private:
	std::unordered_map<int64, ScriptTimer*> _timers;
	int64 _lastTimerId;

};
#endif
