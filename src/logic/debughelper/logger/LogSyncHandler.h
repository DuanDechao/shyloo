#ifndef SL_LOGIC_LOG_SYNC_HANDLER
#define SL_LOGIC_LOG_SYNC_HANDLER
#include "slikernel.h"
class Logger;
class LogSyncHandler: public sl::api::ITimer{
public:
	LogSyncHandler(Logger* logger):_logger(logger){}

	virtual ~LogSyncHandler(){}
	virtual void onStart(sl::api::IKernel* pKernel, int64 timetick){}
	virtual void onTime(sl::api::IKernel* pKernel, int64 timetick);
	virtual void onTerminate(sl::api::IKernel* pKernel, bool beForced, int64 timetick);
	virtual void onPause(sl::api::IKernel* pKernel, int64 timetick){}
	virtual void onResume(sl::api::IKernel* pKernel, int64 timetick){}

private:
	Logger*		_logger;
}; 
#endif
