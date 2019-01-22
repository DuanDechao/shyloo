#include "ScriptTimers.h"
#include "pyscript/scriptobject.h"

sl::SLPool<ScriptTimer> ScriptTimer::s_scriptTimerPool;

void ScriptTimer::onStart(sl::api::IKernel* pKernel, int64 timetick){
	timeTrigger(pKernel, timetick);
}

void ScriptTimer::onTime(sl::api::IKernel* pKernel, int64 timetick){
	timeTrigger(pKernel, timetick);
}

void ScriptTimer::timeTrigger(sl::api::IKernel* pKernel, int64 timetick){
	if(PyObject_HasAttrString(_entity, _callBackName.c_str()) > 0){
		PyObject* pyResult = PyObject_CallMethod(_entity, const_cast<char*>(_callBackName.c_str()), const_cast<char*>("IO"), _timerId,  _userData);
		if(pyResult != NULL){
			Py_DECREF(pyResult);
		}
		else{
			SCRIPT_ERROR_CHECK();
		}
	}
}

void ScriptTimer::onTerminate(sl::api::IKernel* pKernel, bool beForced, int64 timetick){
	this->release();
}

int64 ScriptTimers::addTimer(sl::api::IKernel* pKernel, PyObject* entity, int64 delay, int64 interval, const char* callbackName, PyObject* userData){
	ScriptTimer* newTimer = ScriptTimer::create(entity, _lastTimerId++, callbackName, userData);
	_timers[newTimer->getTimerId()] = newTimer;
	int64 period = interval ? -1 : 0;
	START_TIMER(newTimer, delay, period, interval);
	return newTimer->getTimerId();
}

void ScriptTimers::delTimer(sl::api::IKernel* pKernel, PyObject* entity, int64 timerId){
	if(_timers.find(timerId) == _timers.end()){
		ECHO_ERROR("delTimer failed, can't find timerId[%lld]", timerId);
		return;
	}
	pKernel->killTimer(_timers[timerId]);
}
