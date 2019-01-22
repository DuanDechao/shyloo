#include "DebugHelper.h"
#include "sllog.h"
bool DebugHelper::initialize(sl::api::IKernel * pKernel){
	_self = this;
	_logger = pKernel->createLogger();
	const char* serverName = pKernel->getCmdArg("name");
	const char* serverNodeId = pKernel->getCmdArg("node_id");
	_logger->pushHeader(serverName);
	_logger->pushHeader(serverNodeId);
	_logger->pushHeader(" - ");

	char logFile[128];
	SafeSprintf(logFile, 128, "%s%s.log", serverName, serverNodeId);
	_logger->setFileHandler(logFile);
	_logger->setLogPath("./logger");
	return true;
}

bool DebugHelper::launched(sl::api::IKernel * pKernel){
	return true;
}

bool DebugHelper::destory(sl::api::IKernel * pKernel){
	DEL this;
	return true;
}
