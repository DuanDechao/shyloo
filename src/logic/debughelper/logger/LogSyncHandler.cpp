#include "LogSyncHandler.h"
#include "Logger.h"
void LogSyncHandler::onTime(sl::api::IKernel* pKernel, int64 timetick){
	_logger->syncLog();
}

void LogSyncHandler::onTerminate(sl::api::IKernel* pKernel, bool beForced, int64 timetick){
	_logger->cancelSyncHandler();
}
