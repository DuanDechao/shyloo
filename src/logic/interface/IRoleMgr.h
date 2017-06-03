#ifndef _SL_INTERFACE_ROLEMGR_H__
#define _SL_INTERFACE_ROLEMGR_H__
#include "slimodule.h"
#include <functional>
#include "slbinary_stream.h"

class IRole{
public:
	virtual ~IRole(){}

	virtual void pack() = 0;
	virtual int64 getRoleId() = 0;
};
class IObject;
class IRoleMgr : public sl::api::IModule{
public:
	virtual ~IRoleMgr() {}

	virtual bool getRoleList(int64 account, const std::function <void(sl::api::IKernel* pKernel, const int64 actorId, IRole* role)>& f) = 0;
	virtual IRole* createRole(int64 accountId, const sl::OBStream& buf) = 0;
	virtual bool loadRole(const int64 actorId, IObject* object) = 0;
	virtual void recoverPlayer(IObject* player) = 0;
};

#endif