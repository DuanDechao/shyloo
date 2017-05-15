#ifndef __SL_FRAMEWORK_PLAYERMGR_H__
#define __SL_FRAMEWORK_PLAYERMGR_H__
#include "slikernel.h"
#include "IPlayerMgr.h"
#include "slsingleton.h"
#include <unordered_map>
#include <unordered_set>

class IHarbor;
class IObjectMgr;
class IRoleMgr;
class IEventEngine;
class IProp;
class ICacheDB;
class PlayerMgr :public IPlayerMgr, public sl::SLHolder<PlayerMgr>{
public:
	virtual bool initialize(sl::api::IKernel * pKernel);
	virtual bool launched(sl::api::IKernel * pKernel);
	virtual bool destory(sl::api::IKernel * pKernel);

	virtual bool active(int64 actorId, int32 nodeId, int64 accountId, const std::function<void(sl::api::IKernel* pKernel, IObject* object, bool isReconnect)>& f);
	virtual bool deActive(int64 actorId, int32 nodeId, bool isPlayerOpt);

	void recoverObject(sl::api::IKernel* pKernel, const int64 id);

	void onProcessPlayerOnline(sl::api::IKernel* pKernel, IObject* object);
	void allDataLoadComplete(sl::api::IKernel* pKernel, IObject* object);
	void propSync(sl::api::IKernel* pKernel, IObject* object, const char* name, const IProp* prop, const bool sync);
	bool savePlayer(sl::api::IKernel* pKernel, IObject* player);

	static void onSavePlayerTime(sl::api::IKernel*, IObject*, int64);
	static void onSavePlayerTerminate(sl::api::IKernel*, IObject*, bool, int64);

private:
	static PlayerMgr* s_self;
	sl::api::IKernel* _kernel;
	IHarbor* _harbor;
	IObjectMgr* _objectMgr;
	IEventEngine* _eventEngine;
	IRoleMgr* _roleMgr;
	ICacheDB* _cacheDB;

	int64	_recoverInterval;
	int64	_savePlayerInterval;
};
#endif