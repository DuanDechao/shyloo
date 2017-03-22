#include "RoleMgr.h"
#include "IHarbor.h"

bool RoleMgr::initialize(sl::api::IKernel * pKernel){
	_self = this;
	return true;
}

bool RoleMgr::launched(sl::api::IKernel * pKernel){
	FIND_MODULE(_harbor, Harbor);
	return true;
}
bool RoleMgr::destory(sl::api::IKernel * pKernel){
	DEL this;
	return true;
}

bool RoleMgr::getRoleList(int64 account, const std::function <void(sl::api::IKernel* pKernel, const int64 actorId, IRole* role)>& f){
	return true;
}