#include "ShutDown.h"

bool ShutDown::initialize(sl::api::IKernel * pKernel){
	return true;
}

bool ShutDown::launched(sl::api::IKernel * pKernel){
	return true;
}

bool ShutDown::destory(sl::api::IKernel * pKernel){
	DEL this;
	return true;
}
