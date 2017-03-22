#ifndef _SL_INTERFACE_ROLEMGR_H__
#define _SL_INTERFACE_ROLEMGR_H__
#include "slimodule.h"
#include <functional>

class IRole{
public:
	virtual ~IRole(){}

	//virtual void pack(IBStream)
};

class IRoleMgr : public sl::api::IModule{
public:
	virtual ~IRoleMgr() {}

	virtual bool getRoleList(int64 account, const std::function <void(sl::api::IKernel* pKernel, const int64 actorId, IRole* role)>& f) = 0;
};

#endif