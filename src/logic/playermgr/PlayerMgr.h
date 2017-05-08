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
class PlayerMgr :public IPlayerMgr, public sl::SLHolder<PlayerMgr>{
public:
	virtual bool initialize(sl::api::IKernel * pKernel);
	virtual bool launched(sl::api::IKernel * pKernel);
	virtual bool destory(sl::api::IKernel * pKernel);

	virtual bool active(int64 actorId, int32 nodeId, int64 accountId, const std::function<void(sl::api::IKernel* pKernel, IObject* object, bool isReconnect)>& f);
	virtual bool deActive(int64 actorId, int32 nodeId, bool isPlayerOpt);

	void recoverObject(sl::api::IKernel* pKernel, const int64 id);

private:
	PlayerMgr* _self;
	sl::api::IKernel* _kernel;
	IHarbor* _harbor;
	IObjectMgr* _objectMgr;
	IEventEngine* _eventEngine;
	IRoleMgr* _roleMgr;

	int64	_recoverInterval;
};
#endif