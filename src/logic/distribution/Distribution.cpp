#include "Distribution.h"

bool Distribution::initialize(sl::api::IKernel * pKernel){
	_kernel = pKernel;
	return true;
}

bool Distribution::launched(sl::api::IKernel * pKernel){
	return true;
}

bool Distribution::destory(sl::api::IKernel * pKernel){
	DEL this;
	return true;
}



