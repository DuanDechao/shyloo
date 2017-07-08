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

namespace sl{
namespace core{

class LogNode :public sl::ISLListNode{
public:
	LogNode() :_filter(0),_tick(0), _file(""), _line(0), _log("") {}
	~LogNode(){}

	inline void setTick(int64 tick) { _tick = tick; }
	inline int64 getTick() const { return _tick; }

	inline void setFilter(int32 filter) { _filter = filter; }
	inline int32 getFilter() const { return _filter; }

	inline void setLog(const char* log) { _log = log; }
	inline const char* getLog() const { return _log.c_str(); }

	inline void setFileLine(const char* file, int32 line) { _file = file; _line = line; }
	inline const char* getFile() const { return _file.c_str(); }
	inline int32 getLine() const { return _line; }

private:
	int32 _filter;
	int64 _tick;
	sl::SLString<MAX_PATH> _file;
	int32 _line;
	sl::SLString<10240> _log;
};


class LogEngine : public SLSingleton<LogEngine>{
	friend class SLSingleton<LogEngine>;
public:
	virtual bool ready();
	virtual bool initialize();
	virtual bool destory();

	virtual void logSync(int32 filter, const char* log, const char* file, const int32 line);
	virtual void logAsync(int32 filter, const char* log, const char* file, const int32 line);
	virtual int64 loop(int64 overTime);

	void threadRun();

private:
	LogEngine(){}
	~LogEngine(){}

private:
	std::thread		_thread;
	sl::SLList		_waitQueue;
	sl::SLList		_readyQueue;
	sl::SLList		_runningQueue;
	sl::CLog		_syncLogFile;
	sl::CLog		_asyncLogFile;
	int64           _lastAsyncWriteTick;
	sl::spin_mutex  _mutex;
	bool            _terminate;
};

}
}

#endif
