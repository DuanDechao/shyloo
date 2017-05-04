#ifndef __SL_FRAMEWORK_ROLEMGR_H__
#define __SL_FRAMEWORK_ROLEMGR_H__
#include "slikernel.h"
#include "IRoleMgr.h"
#include "slsingleton.h"
#include <unordered_map>
#include <unordered_set>

class IHarbor;
class ICacheDB;
class Role;
class RoleMgr :public IRoleMgr, public sl::SLHolder<RoleMgr>{
public:
	virtual bool initialize(sl::api::IKernel * pKernel);
	virtual bool launched(sl::api::IKernel * pKernel);
	virtual bool destory(sl::api::IKernel * pKernel);

	virtual bool getRoleList(int64 account, const std::function <void(sl::api::IKernel* pKernel, const int64 actorId, IRole* role)>& f);
	virtual IRole* createRole(int64 accountId, int64 actorId, const sl::OBStream& buf);
	virtual bool loadRole(const int64 actorId, IObject* object);
	virtual void recoverPlayer(IObject* player);

private:
	RoleMgr* _self;
	sl::api::IKernel* _kernel;
	IHarbor* _harbor;
	ICacheDB* _cacheDB;
	std::unordered_map<int64, Role*> _roleMap;
	std::unordered_map<int64, vector<int64>> _accountMap;
};
#endif