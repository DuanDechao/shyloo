#ifndef SL_LOG_ENGINE_H
#define SL_LOG_ENGINE_H
#include "sliconfig_engine.h"
#include <vector>
#include "slsingleton.h"
#include "sllist.h"
#include <thread>
#include "slstring.h"
#include "sllog.h"
#include "slmutex.h"
#include "slikernel.h"
#include "slcycle_queue.h"

namespace sl{
namespace core{
using namespace sl::api;
class LogNode :public sl::ISLListNode{
public:
	LogNode(ILogger* logger) :_logger(logger),_filter(0),_tick(0), _file(""), _line(0), _log(""), _threadId(0) {}
	~LogNode(){}

	inline void setTick(int64 tick) { _tick = tick; }
	inline int64 getTick() const { return _tick; }

	inline void setFilter(int32 filter) { _filter = filter; }
	inline int32 getFilter() const { return _filter; }

	inline void setLog(const char* header, const char* log) { _log << header;  _log << log; }
	inline const char* getLog() const { return _log.c_str(); }
	inline const char* getBuf() {return _log.c_str() + _log.length();}
	inline int32 getBufRemainSize() {return 10240 - _log.length();}

	inline void setFileLine(const char* file, int32 line) { _file = file; _line = line; }
	inline const char* getFile() const { return _file.c_str(); }
	inline int32 getLine() const { return _line; }
	inline ILogger* getLogger() const { return _logger;}

private:
	int32					_filter;
	int64					_tick;
	ThreadID				_threadId;
	sl::SLString<MAX_PATH>	_file;
	int32					_line;
	sl::SLString<10240>		_log;
	ILogger*				_logger;
};

class LogEngine;
class Logger : public ILogger{
public:
	Logger(LogEngine* engine): _logEngine(engine), _logLevel(sl::ELogFilter::ETrace), _logPath("./"){}
	virtual ~Logger(){}

	virtual void pushHeader(const char* header) {_headers << header;}
	virtual void setFileHandler(const char* file) { _syslog = ""; _logFile = file;}
	virtual void setSyslogHandler(const char* file) {_syslog = file; _logFile = "";}
	virtual void setLogLevel(const int32 level) {_logLevel = level;}
	virtual void setLogPath(const char* path) {_logPath = path;}

	virtual void traceLog(const char* format, ...); 
	virtual void errorLog(const char* format, ...);
	virtual void infoLog(const char* format, ...);
	virtual void debugLog(const char* format, ...);
	virtual void warningLog(const char* format, ...);
	virtual void fatalLog(const char* format, ...);
	
	virtual const char* headers() const {return  _headers.c_str();}
	virtual const char* fileHandler() const {return _logFile.c_str();}
	virtual const char* sysLogHandler() const {return _syslog.c_str();}
	virtual const int32 logLevel() const {return _logLevel;}
	virtual const char* logPath() const {return _logPath.c_str();}

	
	void asyncLog(int32 filter, const char* format, va_list argp);

private:
	LogEngine*					_logEngine;
	int32						_logLevel;
	sl::SLString<1024>			_headers;
	std::string					_logFile;
	std::string					_syslog;
	std::string					_logPath;
};

class LogEngine : public SLSingleton<LogEngine>{
	friend class SLSingleton<LogEngine>;
public:
	virtual bool ready();
	virtual bool initialize();
	virtual bool destory();

	virtual void logSync(int32 filter, const char* log, const char* file, const int32 line);
	virtual void logAsync(LogNode* node);
	virtual ILogger* createLogger();

	virtual int64 loop(int64 overTime);

	void threadRun();

private:
	LogEngine(){}
	~LogEngine(){}

	bool queuesIsEmpty();

private:
	typedef std::unordered_map<ThreadID, sl::CycleQueue<LogNode*>*> LOG_QUEUES;
	std::thread				_thread;
	LOG_QUEUES				_runningQueues;
	sl::CLog				_syncLogFile;
	sl::CLog				_asyncLogFile;
	int64					_lastAsyncWriteTick;
	std::vector<ILogger*>	_loggers;
	bool					_terminate;
	ThreadID				_mainThreadId;
};

}
}

#endif
