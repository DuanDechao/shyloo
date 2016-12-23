#include "slkernel.h"
#include <time.h>
namespace sl
{
SL_SINGLETON_INIT(core::Kernel);
namespace core
{
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

void Kernel::parse(int argc, char** argv)
{

}

}
}