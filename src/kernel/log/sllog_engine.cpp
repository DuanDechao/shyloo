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
#define TIME_OUT_FOR_CUT_FILE 2 * 60 * 60 * 1000
bool LogEngine::initialize(){
	_terminate = false;
	string syncFileName = ConfigEngine::getInstance()->getCoreConfig()->logFile + "_" + sl::getCurrentTimeStr("%4d_%02d_%02d_%02d_%02d_%02d_sync.log");
	_syncLogFile.Init(sl::ENamed, ConfigEngine::getInstance()->getCoreConfig()->logPath.c_str(), syncFileName.c_str());
	const char* format = "time|pid|newline|fileline||type|debugout";
	_syncLogFile.SetFormatByStr(format);

	_lastAsyncWriteTick = 0;

	return true;
}

bool LogEngine::ready(){
	_thread = std::thread(&LogEngine::threadRun, this);
	return true;
}

bool LogEngine::destory(){
	_terminate = true;
	_thread.join();

	DEL this;
	return true;
}

void LogEngine::logSync(int32 filter, const char* log, const char* file, const int32 line){
	_syncLogFile.SetFilter(filter);
	_syncLogFile.m_pFile = file;
	_syncLogFile.m_uiLine = line;
	_syncLogFile.Log(log);
}

void LogEngine::logAsync(int32 filter, const char* log, const char* file, const int32 line){
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
	return 0;
}

void LogEngine::threadRun(){
	while (true){
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
			}

			if (_runningQueue.isEmpty())
				break;

			LogNode* currNode = (LogNode*)_runningQueue.popFront();
			if (currNode){
				_syncLogFile.SetFilter(currNode->getFilter());
				_syncLogFile.m_pFile = currNode->getFile();
				_syncLogFile.m_uiLine = currNode->getLine();
				_asyncLogFile.Log(currNode->getLog());
			}

			readCount++;

			if (_terminate)
				break;

		} while (readCount < MAX_READ_COUNT);

		Sleep(1);
	}
}

}
}