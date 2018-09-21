#include "DebugHelper.h"
bool DebugHelper::initialize(sl::api::IKernel * pKernel){
	_self = this;
	return true;
}

bool DebugHelper::launched(sl::api::IKernel * pKernel){
	return true;
}

bool DebugHelper::destory(sl::api::IKernel * pKernel){
	DEL this;
	return true;
}
