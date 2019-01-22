#ifndef __SL_INTERFACE_IDEBUGHELPER_H__
#define __SL_INTERFACE_IDEBUGHELPER_H__
#include "slimodule.h"
#include "slikernel.h"
class IDebugHelper : public sl::api::IModule{
public:
	virtual ~IDebugHelper() {}
	virtual sl::api::ILogger* getLogger() = 0;
};
/*
class ILogger : public sl::api::IModule{
public:
	virtual ~ILogger() {}
	virtual void printMsg(const char* format, ...) = 0; 
	virtual void errorMsg(const char* format, ...) = 0;
	virtual void infoMsg(const char* format, ...) = 0;
	virtual void debugMsg(const char* format, ...) = 0;
	virtual void warningMsg(const char* format, ...) = 0;
	virtual void fatalMsg(const char* format, ...) = 0;
};*/

#define TRACE_LOG(format, a...)			SLMODULE(DebugHelper)->getLogger()->traceLog(format, ##a)
#define ERROR_LOG(format, a...)			SLMODULE(DebugHelper)->getLogger()->errorLog(format, ##a)
#define INFO_LOG(format, a...)			SLMODULE(DebugHelper)->getLogger()->infoLog(format, ##a)
#define DEBUG_LOG(format, a...)			SLMODULE(DebugHelper)->getLogger()->debugLog(format, ##a)
#define WARNING_LOG(format, a...)		SLMODULE(DebugHelper)->getLogger()->warningLog(format, ##a)
#define FATAL_LOG(format, a...)			SLMODULE(DebugHelper)->getLogger()->fatalLog(format, ##a)

#endif
