#ifndef SL_START_NODE_TIMER_H
#define SL_START_NODE_TIMER_H
#include "slikernel.h"
#include "Starter.h"
#include "slpool.h"
class StartNodeTimer : public sl::api::ITimer{
public:
	inline static StartNodeTimer* create(int32 type){
		return CREATE_FROM_POOL(s_pool, type);
	}

	inline void release(){
		s_pool.recover(this);
	}

	virtual void onInit(sl::api::IKernel* pKernel, int64 timetick){}
	virtual void onStart(sl::api::IKernel* pKernel, int64 timetick);
	virtual void onTime(sl::api::IKernel* pKernel, int64 timetick);
	virtual void onTerminate(sl::api::IKernel* pKernel, bool beForced, int64 timetick);
	virtual void onPause(sl::api::IKernel* pKernel, int64 timetick){}
	virtual void onResume(sl::api::IKernel* pKernel, int64 timetick) {}

private:
	friend sl::SLPool<StartNodeTimer>;
	StartNodeTimer(int32 type) :_type(type){}
	~StartNodeTimer(){}

private:
	int32	_type;
	static sl::SLPool<StartNodeTimer> s_pool;
};

#endif