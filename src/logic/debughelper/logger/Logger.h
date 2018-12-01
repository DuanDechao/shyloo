#ifndef SL_LOGIC_LOGGER_H
#define SL_LOGIC_LOGGER_H
#include "slikernel.h"
#include "IDebugHelper.h"
#include "slsingleton.h"
#include "log4cxx/logger.h"
#include <vector>
class LogSyncHandler;
class Logger :public ILogger, public sl::SLHolder<Logger>{
public:
	enum LOG_LEVEL{
		TRACE = 0,
		DEBUG,
		INFO,
		WARNING,
		ERROR,
		FATAL
	};

	const char LOG_LEVEL_NAME[20][20] ={
		"TRACE",
		"DEBUG",
		"INFO",
		"WARN",
		"ERROR",
		"FATAL",
	};

	struct LOG_ITEM{
		int8 logType;
		int64 t;
		uint32 time;
		int32 nodeType;
		int32 nodeId;
		const char* logStr;
	};

public:
	virtual bool initialize(sl::api::IKernel * pKernel);
	virtual bool launched(sl::api::IKernel * pKernel);
	virtual bool destory(sl::api::IKernel * pKernel);

	virtual void changeLogger(const std::string& name);
	virtual void printMsg(const char* format, ...); 
	virtual void errorMsg(const char* format, ...);
	virtual void infoMsg(const char* format, ...);
	virtual void debugMsg(const char* format, ...);
	virtual void warningMsg(const char* format, ...);
	virtual void fatalMsg(const char* format, ...);
	
	inline void cancelSyncHandler() {_isSyncActive = false;}
	void activeSyncHandler();
	void syncLog(); 
	void onClusterMsgSyncLog(sl::api::IKernel* pKernel, int32 nodeType, int32 nodeId, const sl::OBStream& args);
	void onOpen(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId, const char* ip, const int32 port);

private:
	void onMessage(const int8 type, const char* format, va_list argp);
	void writeLog(LOG_ITEM* item);
	bool canSyncLog();

private:
	Logger*				_self;
	sl::api::IKernel*	_kernel;
	log4cxx::LoggerPtr  _logger;
	ThreadID			_mainThreadID;
	std::vector<sl::BStream<1024>> _bufferedLogPackets;
	int32				_tickMaxBufferedLogs;
	int32				_hasBufferedLogPackets;
	LogSyncHandler*		_syncHandler;
	bool				_isSyncActive;
};

#endif
