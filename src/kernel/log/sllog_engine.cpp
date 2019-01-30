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
#define LOG_QUEUE_SIZE 8196
	
void Logger::traceLog(const char* format, ...){
	va_list argList;
	va_start(argList, format);
	asyncLog(sl::ELogFilter::ETrace, format, argList);
	va_end(argList);
} 

void Logger::errorLog(const char* format, ...){
	va_list argList;
	va_start(argList, format);
	asyncLog(sl::ELogFilter::EError, format, argList);
	va_end(argList);
}

void Logger::infoLog(const char* format, ...){
	va_list argList;
	va_start(argList, format);
	asyncLog(sl::ELogFilter::EInfo, format, argList);
	va_end(argList);
}

void Logger::debugLog(const char* format, ...){
	va_list argList;
	va_start(argList, format);
	asyncLog(sl::ELogFilter::EDebug, format, argList);
	va_end(argList);
}
void Logger::warningLog(const char* format, ...){
	va_list argList;
	va_start(argList, format);
	asyncLog(sl::ELogFilter::EWarning, format, argList);
	va_end(argList);
}

void Logger::fatalLog(const char* format, ...){
	va_list argList;
	va_start(argList, format);
	asyncLog(sl::ELogFilter::EFatal, format, argList);
	va_end(argList);
}

void Logger::asyncLog(int32 filter, const char* format, va_list argp){
	if(logLevel() < filter)
		return;

	LogNode* node = NEW LogNode(this);
	node->setIgnoreOwner(true);
	node->setTick(sl::getTimeMilliSecond());
	SafeSprintf((char*)node->getBuf(), node->getBufRemainSize(), "%s", headers());
	vsnprintf((char*)node->getBuf(), node->getBufRemainSize(), format, argp);
	node->setFilter(filter);

	_logEngine->logAsync(node);
}

bool LogEngine::initialize(){
	_terminate = false;
	_asyncLogFile.SetFormatByStr("time|pid|newline|type");
	
	_thread = std::thread(&LogEngine::threadRun, this);
	_mainThreadId = pthread_self();
	return true;
}

bool LogEngine::ready(){
	return true;
}

bool LogEngine::destory(){
	_terminate = true;
	_thread.join();

	for(auto* loggerItor : _loggers){
		DEL loggerItor;
	}

	for(auto queueItor : _runningQueues){
		DEL queueItor.second;
	}

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

ILogger* LogEngine::createLogger(){
	Logger* logger  = NEW Logger(this);
	_loggers.push_back(logger);
	return logger;
}

void LogEngine::logAsync(LogNode* node){
	ThreadID threadId = pthread_self();
	auto queueItor = _runningQueues.find(threadId);
	if(queueItor == _runningQueues.end()){
		_runningQueues[threadId] = NEW sl::CycleQueue<LogNode*>(LOG_QUEUE_SIZE);
	}
	_runningQueues[threadId]->push(node);
}

int64 LogEngine::loop(int64 overTime){
	return 0;
}

bool LogEngine::queuesIsEmpty(){
	LOG_QUEUES runningQueues = _runningQueues;
	for(auto queueItor : runningQueues){
		if(!queueItor.second->isEmpty())
			return false;
	}
	return true;
}

void LogEngine::threadRun(){
	while (!_terminate || !queuesIsEmpty()){
		int32 readCount = 0;
		do{
			if (queuesIsEmpty())
				break;

			LOG_QUEUES runningQueues = _runningQueues;
			for(auto queueItor : _runningQueues){
				LogNode* currNode = NULL;
				if(queueItor.second->read(currNode)){
					ILogger* logger = currNode->getLogger();
					if( strcmp(logger->fileHandler(), "") != 0 ){
						_asyncLogFile.Init(sl::ENamed, logger->logPath(), logger->fileHandler());
						_asyncLogFile.RemoveFormat(sl::ELogFormatTag::ENoFile);
						_asyncLogFile.RemoveFormat(sl::ELogFormatTag::EStdOut);
					}
					else{
						_asyncLogFile.AddFormat(sl::ELogFormatTag::ENoFile);
						_asyncLogFile.AddFormat(sl::ELogFormatTag::EStdOut);
					}
					_asyncLogFile.SetFilter(currNode->getFilter());
					_asyncLogFile.m_pFile = currNode->getFile();
					_asyncLogFile.m_uiLine = currNode->getLine();
					_asyncLogFile.ResetLogTick(currNode->getTick());
					_asyncLogFile.Log(currNode->getLog());

					DEL currNode;
				}

				readCount++;
			}

			if (_terminate)
				break;

		} while (readCount < MAX_READ_COUNT);

		CSLEEP(1);
	}
}

}
}
