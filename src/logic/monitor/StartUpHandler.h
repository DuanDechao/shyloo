#ifndef SL_START_UP_HANDLER_H
#define SL_START_UP_HANDLER_H
#include "slikernel.h"
class StartUpHandler : public sl::api::ITimer{
public:
	StartUpHandler();
	virtual void onStart(sl::api::IKernel* pKernel, int64 timetick){}
	virtual void onTime(sl::api::IKernel* pKernel, int64 timetick);
	virtual void onTerminate(sl::api::IKernel* pKernel, bool beForced, int64 timetick){}
	virtual void onPause(sl::api::IKernel* pKernel, int64 timetick) {}
	virtual void onResume(sl::api::IKernel* pKernel, int64 timetick){}

protected:
	bool process();

private:
	bool	_serverReady;
	bool	_serverReadyForLogin;
};

#endif
