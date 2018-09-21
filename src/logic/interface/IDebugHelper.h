#ifndef __SL_INTERFACE_IDEBUGHELPER_H__
#define __SL_INTERFACE_IDEBUGHELPER_H__
#include "slimodule.h"
class IDebugHelper : public sl::api::IModule{
public:
	virtual ~IDebugHelper() {}
};

class ILogger : public sl::api::IModule{
public:
	virtual ~ILogger() {}
	virtual void printMsg(const char* format, ...) = 0; 
	virtual void errorMsg(const char* format, ...) = 0;
	virtual void infoMsg(const char* format, ...) = 0;
	virtual void debugMsg(const char* format, ...) = 0;
	virtual void warningMsg(const char* format, ...) = 0;
	virtual void fatalMsg(const char* format, ...) = 0;
};

#define TRACE_LOG(format, a...)			SLMODULE(Logger)->printMsg(format, ##a)
#define ERROR_LOG(format, a...)			SLMODULE(Logger)->errorMsg(format, ##a)
#define INFO_LOG(format, a...)			SLMODULE(Logger)->infoMsg(format, ##a)
#define DEBUG_LOG(format, a...)			SLMODULE(Logger)->debugMsg(format, ##a)
#define WARNING_LOG(format, a...)		SLMODULE(Logger)->warningMsg(format, ##a)
#define FATAL_LOG(format, a...)			SLMODULE(Logger)->fatalMsg(format, ##a)

#endif
