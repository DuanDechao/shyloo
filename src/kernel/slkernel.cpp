#include "slkernel.h"
#include "slnet_engine.h"
#include "sltimer_engine.h"
#include "sllogic_engine.h"
#include "slconfig_engine.h"
#include "slasync_engine.h"
#include "sllog_engine.h"
//#include "slipc_engine.h"
#include <time.h>

namespace sl
{
namespace core
{

extern "C" KERNEL_API sl::api::IKernel* getCore(){
	return Kernel::getInstance();
}

Kernel* Kernel::getInstance(){
	static Kernel* p = nullptr;
	if (!p){
		p = NEW Kernel;
		if (!p->ready()){
			SLASSERT(false, "Kernel not ready");
			DEL p;
			p = nullptr;
		}
	}
	return p;
}

bool Kernel::ready(){
	return ConfigEngine::getInstance() &&
		TimerEngine::getInstance() &&
		NetEngine::getInstance() &&
	//	IPCEngine::getInstance() &&
		AsyncEngine::getInstance() &&
		LogicEngine::getInstance() &&
		LogEngine::getInstance();
		
}

bool Kernel::initialize(int32 argc, char ** argv){
	_shutDown = false;
	_kernelLogger = NULL;
	parse(argc, argv);

	bool ret = ConfigEngine::getInstance()->initialize() &&
		LogEngine::getInstance()->initialize();

	_kernelLogger = LogEngine::getInstance()->createLogger();
	const char* serverName = getCmdArg("name");
	const char* nodeId = getCmdArg("node_id");
	_kernelLogger->pushHeader(serverName);
	_kernelLogger->pushHeader(nodeId);
	_kernelLogger->pushHeader(" - ");

	return TimerEngine::getInstance()->initialize() &&
		NetEngine::getInstance()->initialize() &&
	//	IPCEngine::getInstance()->initialize() &&
		AsyncEngine::getInstance()->initialize() &&
		LogicEngine::getInstance()->initialize();
}

bool Kernel::destory(){
	AsyncEngine::getInstance()->destory();
	LogicEngine::getInstance()->destory();
	TimerEngine::getInstance()->destory();
	NetEngine::getInstance()->destory();
//	IPCEngine::getInstance()->destory();
	LogEngine::getInstance()->destory();
	ConfigEngine::getInstance()->destory();

	DEL this;
	return true;
}

void Kernel::loop() {
	srand((uint32)time(nullptr));
	_shutDown = false;
	while (!_shutDown){
		int64 startTick = sl::getTimeMilliSecond();
		int64 asyncTick = AsyncEngine::getInstance()->loop(ConfigEngine::getInstance()->getCoreConfig()->gameUpdateTick);
		int64 timerTick = TimerEngine::getInstance()->loop(ConfigEngine::getInstance()->getCoreConfig()->gameUpdateTick);
		int64 nextTimerExp = TimerEngine::getInstance()->getNextExp(ConfigEngine::getInstance()->getCoreConfig()->gameUpdateTick);
		SLASSERT(nextTimerExp <= ConfigEngine::getInstance()->getCoreConfig()->gameUpdateTick, "wtf");
		int64 netTick = NetEngine::getInstance()->loop(nextTimerExp);
//		int64 ipcTick = IPCEngine::getInstance()->loop(ConfigEngine::getInstance()->getCoreConfig()->gameUpdateTick);
		LogEngine::getInstance()->loop(0);

		int64 useTick = sl::getTimeMilliSecond() - startTick;
		if (useTick > 2* ConfigEngine::getInstance()->getCoreConfig()->gameUpdateTick){
			ECHO_ERROR("Loop use %d(%d, %d, %d)", useTick, netTick, asyncTick, timerTick);
		}
		else{
			CSLEEP(1);
		}
	}
}

const char* Kernel::getCmdArg(const char* name){
	auto iter = _args.find(name);
	if (iter != _args.end())
		return iter->second.c_str();
	return nullptr;
}

bool Kernel::startTcpServer(api::ITcpServer * server, const char* ip, const int32 port, int32 sendSize, int32 recvSize){
	return NetEngine::getInstance()->addTcpServer(server, ip, port, sendSize, recvSize);
}

bool Kernel::startTelnetServer(api::ITcpServer * server, const char* ip, const int32 port){
	return NetEngine::getInstance()->addTelnetServer(server, ip, port);
}

bool Kernel::startTcpClient(api::ITcpSession * client, const char* ip, const int32 port, int32 sendSize, int32 recvSize){
	return NetEngine::getInstance()->addTcpClient(client, ip, port, sendSize, recvSize);
}

const char* Kernel::getInternetIp(){
	return NetEngine::getInstance()->getInternetIp();
}

const char* Kernel::getLocalIp(){
	return NetEngine::getInstance()->getLocalIp();
}

uint64 Kernel::getSpareTime(){
	return NetEngine::getInstance()->getSpareTime();
}

void Kernel::clearSpareTime(){
	return NetEngine::getInstance()->clearSpareTime();
}

api::IModule* Kernel::findModule(const char * name){
	return LogicEngine::getInstance()->findModule(name);
}

bool Kernel::startTimer(api::ITimer* timer, int64 delay, int32 count, int64 interval, const char* file, const int32 line){
	return TimerEngine::getInstance()->startTimer(timer, delay, count, interval, file, line);
}

bool Kernel::killTimer(api::ITimer* timer){
	return TimerEngine::getInstance()->killTimer(timer);
}

void Kernel::pauseTimer(api::ITimer* timer){
	TimerEngine::getInstance()->pauseTimer(timer);
}

void Kernel::resumeTimer(api::ITimer* timer){
	TimerEngine::getInstance()->resumeTimer(timer);
}

void Kernel::startAsync(const int64 threadId, api::IAsyncHandler* handler, const char* debug){
	AsyncEngine::getInstance()->start(threadId, handler, debug);
}

void Kernel::stopAsync(api::IAsyncHandler* handler){
	AsyncEngine::getInstance()->stop(handler);
}

void Kernel::syncLog(int32 filter, const char* log, const char* file, const int32 line){
	LogEngine::getInstance()->logSync(filter, log, file, line);
}

sl::api::ILogger* Kernel::createLogger(){
	LogEngine::getInstance()->createLogger();
}

bool Kernel::addIPCServer(sl::api::ITcpServer* server, const int64 serverId){
	return true;
	//return IPCEngine::getInstance()->addIPCServer(server, serverId);
}

bool Kernel::addIPCClient(sl::api::ITcpSession* session, const int64 clientId, const int64 serverId, const int32 sendSize, const int32 recvSize){
	return true;
	//return IPCEngine::getInstance()->addIPCClient(session, clientId, serverId, sendSize, recvSize);
}
	
const char* Kernel::getSysResPath(){
	return ConfigEngine::getInstance()->getSysResPath();
}

const char* Kernel::getUserResPath(){
	return ConfigEngine::getInstance()->getUserResPath();
}

const char* Kernel::getUserScriptsPath(){
	return ConfigEngine::getInstance()->getUserScriptsPath();
}

bool Kernel::hasRes(const char* res){
	return ConfigEngine::getInstance()->hasRes(res);
}

const char* Kernel::matchRes(const char* res){
	return ConfigEngine::getInstance()->matchRes(res);
}

int32 Kernel::getResValueInt32(const char* attr){
	return ConfigEngine::getInstance()->getResValueInt32(attr);
}

int64 Kernel::getResValueInt64(const char* attr){
	return ConfigEngine::getInstance()->getResValueInt64(attr);
}

const char* Kernel::getResValueString(const char* attr){
	return ConfigEngine::getInstance()->getResValueString(attr);
}

bool Kernel::getResValueBoolean(const char* attr){
	return ConfigEngine::getInstance()->getResValueBoolean(attr);
}

int32 Kernel::maxAsyncThreadNum(){
	return ConfigEngine::getInstance()->getCoreConfig()->maxAsyncThreadNum;
}

void Kernel::parse(int argc, char** argv){
	for (int32 i = 1; i < argc; i++){
		SLASSERT(strncmp(argv[i], "--", 2) == 0, "invalid argv %s", argv[i]);

		char* start = argv[i] + 2;
		char* equal = strstr(start, "=");
		SLASSERT(equal != nullptr, "invalid argv %s", argv[i]);
		string name(start, equal);
		string value(equal + 1);
		_args[name] = value;
	}
}

}
}
