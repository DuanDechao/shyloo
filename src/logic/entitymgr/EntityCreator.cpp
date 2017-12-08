#include "EntityMgr.h"
bool EntityMgr::initialize(sl::api::IKernel * pKernel){
	return true;
}

bool EntityMgr::launched(sl::api::IKernel * pKernel){
	return true;
}

bool EntityMgr::destory(sl::api::IKernel * pKernel){
	DEL this;
	return true;
}

