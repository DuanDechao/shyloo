#include "slkernel.h"
#include "sllog_engine.h"
#include "slmulti_sys.h"
#include "slxml_reader.h"
#include "sltools.h"
#include "sltime.h"
#include "slconfig_engine.h"
#include <mutex>

namespace sl{
namespace core{
#define MAX_READ_COUNT 50
#define TIME_OUT_FOR_CUT_FILE 2 * HOUR
bool LogEngine::initialize(){
	_terminate = false;

	string syncFileName = ConfigEngine::getInstance()->getCoreConfig()->logFile + "_" + sl::getCurrentTimeStr("%4d_%02d_%02d_%02d_%02d_%02d_sync.log");
	_syncLogFile.Init(sl::ENamed, ConfigEngine::getInstance()->getCoreConfig()->logPath.c_str(), syncFileName.c_str());
	_syncLogFile.SetFormatByStr(ConfigEngine::getInstance()->getCoreConfig()->logFormat.c_str());
	
	_asyncLogFile.SetFormatByStr(ConfigEngine::getInstance()->getCoreConfig()->logFormat.c_str());
	_lastAsyncWriteTick = 0;

	_thread = std::thread(&LogEngine::threadRun, this);

	return true;
}

bool LogEngine::ready(){
	return true;
}

bool LogEngine::destory(){
	_terminate = true;
	_thread.join();

	DEL this;
	return true;
}

void LogEngine::logSync(int32 filter, const char* log, const char* file, const int32 line){
#ifndef _DEBUG
	if (filter == sl::ELogFilter::EDebug)
		return;
#endif
	_syncLogFile.SetFilter(filter);
	_syncLogFile.m_pFile = file;
	_syncLogFile.m_uiLine = line;
	_syncLogFile.Log(log);
}

void LogEngine::logAsync(int32 filter, const char* log, const char* file, const int32 line){
#ifndef _DEBUG
	if (filter == sl::ELogFilter::EDebug)
		return;
#endif

	LogNode* node = NEW LogNode;
	node->setIgnoreOwner(true);
	node->setTick(sl::getTimeMilliSecond());
	node->setLog(log);
	node->setFilter(filter);
	node->setFileLine(file, line);

	_waitQueue.pushBack(node);
}

int64 LogEngine::loop(int64 overTime){
	if (_waitQueue.isEmpty())
		return 0;

	std::unique_lock<sl::spin_mutex> lock(_mutex);
	_readyQueue.merge(_waitQueue);
	_waitQueue.clear();

	return 0;
}

void LogEngine::threadRun(){
	while (!_terminate){
		if (_lastAsyncWriteTick == 0 || sl::getTimeMilliSecond() - _lastAsyncWriteTick >= TIME_OUT_FOR_CUT_FILE){
			string syncFileName = ConfigEngine::getInstance()->getCoreConfig()->logFile + "_" + sl::getCurrentTimeStr("%4d_%02d_%02d_%02d_%02d_%02d_async.log");
			_asyncLogFile.Init(sl::ENamed, ConfigEngine::getInstance()->getCoreConfig()->logPath.c_str(), syncFileName.c_str());
			_lastAsyncWriteTick = sl::getTimeMilliSecond();
		}

		int32 readCount = 0;
		do{
			if (_runningQueue.isEmpty() && !_readyQueue.isEmpty()){
				std::unique_lock<sl::spin_mutex> lock(_mutex);
				_runningQueue.merge(_readyQueue);
				_readyQueue.clear();
			}

			if (_runningQueue.isEmpty())
				break;

			LogNode* currNode = (LogNode*)_runningQueue.popFront();
			if (currNode){
				_asyncLogFile.SetFilter(currNode->getFilter());
				_asyncLogFile.m_pFile = currNode->getFile();
				_asyncLogFile.m_uiLine = currNode->getLine();
				_asyncLogFile.ResetLogTick(currNode->getTick());
				_asyncLogFile.Log(currNode->getLog());
			}

			readCount++;

			if (_terminate)
				break;

		} while (readCount < MAX_READ_COUNT);

		CSLEEP(1);
	}
}

}
}
