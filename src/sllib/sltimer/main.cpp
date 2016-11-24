#include "sltimer.h"
#include "sltype.h"
#include "sltime_stamp.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
using namespace sl;
using namespace sl::timer;

class TestTimer : public ISLTimer
{
public:
	virtual void SLAPI onInit(int64 timetick){printf("onInit() %lld\n", timetick);}
	virtual void SLAPI onStart(int64 timetick){printf("onStart %lld\n",timetick );}
	virtual void SLAPI onTime(int64 timetick) {printf("onTime %lld\n",timetick );}
	virtual void SLAPI onTerminate(int64 timetick) {printf("onTerminate %lld\n",timetick );}
	virtual void SLAPI onPause(int64 timetick) {printf("onPause %lld\n",timetick );}
	virtual void SLAPI onResume(int64 timetick) {printf("onResume %lld\n",timetick );}
};
int main()
{

	ISLTimerMgr* timerMgr = getSLTimerModule();
	TestTimer * pTestTimer = new TestTimer();
	SLTimerHandle * timerHandle = (SLTimerHandle *)timerMgr->startTimer(pTestTimer, 5000, 3, 5000);
	uint64 now = TimeStamp::toSeconds(timestamp());
	while(true){
		timerMgr->process(timestamp());
		uint64 ri = TimeStamp::toSeconds(timestamp());
		if(ri - now == 10)
		{
			timerMgr->pauseTimer(timerHandle);
		}
		if(ri - now == 50)
		{
			timerMgr->resumeTimer(timerHandle);
		}
	}
	delete pTestTimer;
	return 0;
}