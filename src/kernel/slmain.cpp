#include "sltimer.h"
#include "sltype.h"
#include "sltime.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
using namespace sl;
using namespace sl::timer;

class TestTimer : public ISLTimer
{
public:
	virtual void SLAPI onInit(int64 timetick){printf("onInit() %lld", timetick);}
	virtual void SLAPI onStart(int64 timetick){printf("onStart %lld",timetick );}
	virtual void SLAPI onTime(int64 timetick) {printf("onTime %lld",timetick );}
	virtual void SLAPI onTerminate(int64 timetick) {printf("onTerminate %lld",timetick );}
	virtual void SLAPI onPause(int64 timetick) {printf("onPause %lld",timetick );}
	virtual void SLAPI onResume(int64 timetick) {printf("onResume %lld",timetick );}
};
int main()
{
	
	ISLTimerMgr* timerMgr = getSLTimerModule();
	TestTimer * pTestTimer = new TestTimer();
	timerMgr->startTimer(pTestTimer, 5000, 3, 5000);
	while(true){
		timerMgr->process(getTimeMilliSecond());
	}
	delete pTestTimer;
	return 0;
}