#include "Logger.h"
#include "IResMgr.h"
#include "slstring_utils.h"
#include "log4cxx/propertyconfigurator.h"
#include <sys/timeb.h>
#include "LogSyncHandler.h"
#include "IHarbor.h"
#include "NodeDefine.h"
#include "NodeProtocol.h"
#include <iomanip>

bool Logger::initialize(sl::api::IKernel * pKernel){
	_self = this;
	_kernel = pKernel;
	_mainThreadId = pthread_self();
	_logger = log4cxx::Logger::getLogger("");
	_tickMaxBufferedLogs = 1000;
	_hasBufferedLogPackets = 0;
	_bufferedLogPackets.reserve(_tickMaxBufferedLogs);
	for(int32 idx = 0; idx < _tickMaxBufferedLogs; idx++){
		 sl::BStream<1024>* buf = NEW sl::BStream<1024>;
		_bufferedLogPackets.push_back(*buf);
	}

	_syncHandler = NEW LogSyncHandler(this);

	std::string xmlPath = SLMODULE(ResMgr)->matchRes("server/shyloo_defs.xml");
	if(xmlPath != "server/shyloo_defs.xml"){
		sl::CStringUtils::RepleaceAll(xmlPath, "shyloo_defs.xml", "shyloo.properties");
	}
	log4cxx::PropertyConfigurator::configure(xmlPath);
	return true;
}

bool Logger::launched(sl::api::IKernel * pKernel){
	if(SLMODULE(Harbor)->getNodeType() == NodeType::LOGGER){
		RGS_NODE_HANDLER(SLMODULE(Harbor), NodeProtocol::CLUSTER_MSG_SYNC_LOG, Logger::onClusterMsgSyncLog);
	}

	return true;
}

bool Logger::destory(sl::api::IKernel * pKernel){
	DEL this;
	return true;
}

void Logger::changeLogger(const std::string& name){
	_logger = log4cxx::Logger::getLogger(name);
}

void Logger::printMsg(const char* format, ...){
	va_list argList;
	va_start(argList, format);
	char buf[2048];
	onMessage(LOG_LEVEL::TRACE, format, argList);
	va_end(argList);
}

void Logger::errorMsg(const char* format, ...){
	va_list argList;
	va_start(argList, format);
	onMessage(LOG_LEVEL::ERROR, format, argList);
	va_end(argList);
}

void Logger::infoMsg(const char* format, ...){
	va_list argList;
	va_start(argList, format);
	onMessage(LOG_LEVEL::INFO, format, argList);
	va_end(argList);
}

void Logger::debugMsg(const char* format, ...){
	va_list argList;
	va_start(argList, format);
	onMessage(LOG_LEVEL::DEBUG, format, argList);
	va_end(argList);
}

void Logger::warningMsg(const char* format, ...){
	va_list argList;
	va_start(argList, format);
	onMessage(LOG_LEVEL::WARNING, format, argList);
	va_end(argList);
}

void Logger::fatalMsg(const char* format, ...){
	va_list argList;
	va_start(argList, format);
	onMessage(LOG_LEVEL::FATAL, format, argList);
	va_end(argList);
}

bool Logger::canSyncLog(){
	if(SLMODULE(Harbor)->getNodeType() == NodeType::LOGGER)
		return SLMODULE(Harbor)->isNodeConnected(NodeType::MASTER, 1);
	else if(SLMODULE(Harbor)->getNodeType() == NodeType::MASTER)
		return SLMODULE(Harbor)->isNodeConnected(NodeType::LOGGER, 1);
	else
		return SLMODULE(Harbor)->isNodeConnected(NodeType::MASTER, 1) && SLMODULE(Harbor)->isNodeConnected(NodeType::LOGGER, 1);
}

void Logger::activeSyncHandler(){
	if(!canSyncLog())
		return;

	if(!_isSyncActive){
		sl::api::IKernel* pKernel = _kernel;
		START_TIMER(_syncHandler, 0, 1, 10 * SECOND); 
		_isSyncActive = true;
	}
}

void Logger::syncLog(){
	if(_hasBufferedLogPackets <= 0)
		return;
	
	for(int32 idx = 0; idx < _hasBufferedLogPackets; idx++){
		SLMODULE(Harbor)->send(NodeType::LOGGER, 1, NodeProtocol::CLUSTER_MSG_SYNC_LOG, _bufferedLogPackets[idx].out());
	}
	_hasBufferedLogPackets = 0;
}

void Logger::onMessage(const int8 logType, const char* format, va_list argp){
	if(!canSyncLog()){
		char logBuf[2048];
		vsnprintf(logBuf, 2048, format, argp);
		
		switch(logType){
		case LOG_LEVEL::TRACE: printf("[TRACE] %s\n", logBuf); break;
		case LOG_LEVEL::DEBUG: printf("[DEBUG] %s\n", logBuf); break;
		case LOG_LEVEL::INFO: printf("[INFO] %s\n", logBuf); break;
		case LOG_LEVEL::WARNING: printf("[WARN] %s\n", logBuf); break;
		case LOG_LEVEL::ERROR:	printf("[ERROR] %s\n", logBuf); break;
		case LOG_LEVEL::FATAL:	printf("[ERROR] %s\n", logBuf); break;
		default: printf("[INFO] %s\n", logBuf); break;
		}
		return;
	}

	struct timeb tp;
	ftime(&tp);
	int64 t = tp.time;
	uint32 millitm = tp.millitm;

	if(SLMODULE(Harbor)->getNodeType() == NodeType::LOGGER){
		char logBuf[2048];
		vsnprintf(logBuf, 2048, format, argp);
		LOG_ITEM item {logType, t, millitm, SLMODULE(Harbor)->getNodeType(), SLMODULE(Harbor)->getNodeId(), logBuf};
		writeLog(&item);
		return;
	}
	
	if(_hasBufferedLogPackets >= _tickMaxBufferedLogs){
		//清空buffer
		_hasBufferedLogPackets = 0;
		printf("Logger::onMessage: discard logs\n");
		return;
	}

	sl::IBStream& buf = _bufferedLogPackets[_hasBufferedLogPackets++];
	buf.reset();
	SLASSERT(t > 0, "wtf");

	buf << logType;
	buf << t;
	buf << millitm;
	int32* msgSize = buf.reserveInt32();
	vsnprintf((char*)buf.getContext(), buf.getSize(), format, argp);
	*msgSize = strlen((const char*)buf.getContext());
	buf.skip(*msgSize + 1);

	activeSyncHandler();
}

void Logger::onClusterMsgSyncLog(sl::api::IKernel* pKernel, int32 nodeType, int32 nodeId, const sl::OBStream& args){
	int8 logType = 0;
	int64 tick = 0;
	uint32 time = 0;
	const char* logStr = nullptr; 
	args >> logType >> tick >> time >> logStr;
	LOG_ITEM item {logType, tick, time, nodeType, nodeId, logStr};
	writeLog(&item);
}

void Logger::writeLog(LOG_ITEM* pLogItem){
	std::stringstream logStream;

	time_t tt = static_cast<time_t>(pLogItem->t);
	tm* aTm = localtime(&tt);
	if(!aTm){
		printf("Logger::writeLog: log error\n");
		return;
	}
	char timeBuf[256];
	SafeSprintf(timeBuf, 256, "%-4d-%02d-%02d %02d:%02d:%02d,%03d", aTm->tm_year+1900, aTm->tm_mon+1, aTm->tm_mday, 
			aTm->tm_hour, aTm->tm_min, aTm->tm_sec, pLogItem->time);

	logStream << timeBuf;
	logStream << setw(8) << setiosflags(ios::right) << SLMODULE(Harbor)->getNodeName(pLogItem->nodeType);
	logStream << setw(3) << setiosflags(ios::right) << pLogItem->nodeId;
	logStream << setw(8) << setiosflags(ios::right) << LOG_LEVEL_NAME[pLogItem->logType];
	logStream << " - ";
	logStream << pLogItem->logStr;
	logStream << "\n";

	switch(pLogItem->logType){
	case LOG_LEVEL::TRACE:	 LOG4CXX_TRACE(_logger, logStream.str()); break;
	case LOG_LEVEL::DEBUG:	 LOG4CXX_DEBUG(_logger, logStream.str()); break;
	case LOG_LEVEL::INFO:	 LOG4CXX_INFO(_logger, logStream.str()); break;
	case LOG_LEVEL::WARNING: LOG4CXX_WARN(_logger, logStream.str()); break;
	case LOG_LEVEL::ERROR:	 LOG4CXX_ERROR(_logger, logStream.str()); break;
	case LOG_LEVEL::FATAL:	 LOG4CXX_FATAL(_logger, logStream.str()); break;
	default: LOG4CXX_INFO(_logger, logStream.str()); break;
	}
}
