#include "Monitor.h"

bool Monitor::initialize(sl::api::IKernel * pKernel){
	return true;
}

bool Monitor::launched(sl::api::IKernel * pKernel){
	FIND_MODULE(_client, Client);
	return true;
}

bool Monitor::destory(sl::api::IKernel * pKernel){
	DEL this;
	return true;
}