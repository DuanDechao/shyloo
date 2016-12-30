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
	return true;
}

bool Kernel::initialize(int32 argc, char ** argv){
	parse(argc, argv);
	return true;
}

void Kernel::loop() {
	srand((uint32)time(nullptr));
	m_bShutDown = false;
	while(!m_bShutDown){
		//int64 netTick = NetEngine::getSingletonPtr()->processing();
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

void Kernel::parse(int argc, char** argv)
{

}

}
}