#include "AI.h"

bool AI::initialize(sl::api::IKernel * pKernel){
	_self = this;
	_kernel = pKernel;

	return true;
}

bool AI::launched(sl::api::IKernel * pKernel){
	return true;
}

bool AI::destory(sl::api::IKernel * pKernel){
	DEL this;
	return true;
}

void AI::startAI(IObject* object){

}
