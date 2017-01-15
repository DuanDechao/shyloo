#include "slkernel.h"
#include "slnet_engine.h"
#include "sltimer_engine.h"
#include "sllogic_engine.h"
#include "slconfig_engine.h"
#include <time.h>
namespace sl
{
namespace core
{
api::IKernel* Kernel::getInstance(){
	static api::IKernel* p = nullptr;
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
		LogicEngine::getInstance();
}

bool Kernel::initialize(int32 argc, char ** argv){
	parse(argc, argv);

	return ConfigEngine::getInstance()->initialize() &&
		TimerEngine::getInstance()->initialize() && 
		NetEngine::getInstance()->initialize() &&
		LogicEngine::getInstance()->initialize();
}

bool Kernel::destory(){
	ConfigEngine::getInstance()->destory();
	TimerEngine::getInstance()->destory();
	NetEngine::getInstance()->destory();
	LogicEngine::getInstance()->destory();
	DEL this;
	return true;
}

void Kernel::loop() {
	srand((uint32)time(nullptr));
	m_bShutDown = false;
	while(!m_bShutDown){
		int64 startTick = sl::getTimeMilliSecond();
		int64 netTick = NetEngine::getInstance()->processing(ConfigEngine::getInstance()->getCoreConfig()->sNetlooptick);
		int64 timerTick = TimerEngine::getInstance()->processing(ConfigEngine::getInstance()->getCoreConfig()->sTimerlooptick);

		int64 useTick = sl::getTimeMilliSecond() - startTick;
		if (useTick > ConfigEngine::getInstance()->getCoreConfig()->sLoopduration ||
			netTick > ConfigEngine::getInstance()->getCoreConfig()->sNetlooptick ||
			timerTick > ConfigEngine::getInstance()->getCoreConfig()->sTimerlooptick){
			//ECHO_ERROR("Loop use %d(%d, %d)", useTick, netTick, timerTick);
		}
		else{
			Sleep(1);
		}
	}
}

const char* Kernel::getCmdArg(const char* name){
	auto iter = _args.find(name);
	if (iter != _args.end())
		return iter->second.c_str();
	return nullptr;
}

const char* Kernel::getCoreFile(){
	return ConfigEngine::getInstance()->getCoreFile();
}

const char* Kernel::getConfigFile(){
	return ConfigEngine::getInstance()->getConfigFile();
}

const char* Kernel::getEnvirPath(){
	return ConfigEngine::getInstance()->getEnvirPath();
}

bool Kernel::startTcpServer(api::ITcpServer * server, const char* ip, const int32 port, int32 sendSize, int32 recvSize){
	return NetEngine::getInstance()->addTcpServer(server, ip, port, sendSize, recvSize);
}

bool Kernel::startTcpClient(api::ITcpSession * client, const char* ip, const int32 port, int32 sendSize, int32 recvSize){
	return NetEngine::getInstance()->addTcpClient(client, ip, port, sendSize, recvSize);
}

api::IModule* Kernel::findModule(const char * name){
	return LogicEngine::getInstance()->findModule(name);
}

bool Kernel::startTimer(api::ITimer* timer, int64 delay, int32 count, int64 interval){
	return TimerEngine::getInstance()->startTimer(timer, delay, count, interval);
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