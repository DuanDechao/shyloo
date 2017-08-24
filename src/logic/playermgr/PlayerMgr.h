#ifndef __SL_FRAMEWORK_PLAYERMGR_H__
#define __SL_FRAMEWORK_PLAYERMGR_H__
#include "slikernel.h"
#include "IPlayerMgr.h"
#include "slsingleton.h"
#include <unordered_map>
#include <unordered_set>
#include "slbinary_stream.h"
#include "slstring.h"

class IHarbor;
class IObjectMgr;
class IRoleMgr;
class IEventEngine;
class IProp;
class ICacheDB;
class IObjectTimer;
class ILogic;
class IPropDelaySender;
class PlayerMgr :public IPlayerMgr, public sl::SLHolder<PlayerMgr>{
public:
	virtual bool initialize(sl::api::IKernel * pKernel);
	virtual bool launched(sl::api::IKernel * pKernel);
	virtual bool destory(sl::api::IKernel * pKernel);

	virtual bool active(int64 actorId, int32 nodeId, int64 accountId, const std::function<void(sl::api::IKernel* pKernel, IObject* object, bool isReconnect)>& f);
	virtual bool deActive(int64 actorId, int32 nodeId, bool isPlayerOpt);
	virtual void foreach(const IProp* prop, const std::function<void(sl::api::IKernel* pKernel, IObject* object, int64 tick)>& f);
	virtual IObject* findPlayer(const char* name);
	virtual IObject* findPlayer(const int64 id);
	virtual void save(IObject* object);

	void startSavePlayerTimer(sl::api::IKernel* pKernel, IObject* object);
	void recoverObject(sl::api::IKernel* pKernel, const int64 id);

	void onProcessPlayerOnline(sl::api::IKernel* pKernel, IObject* object);
	void allDataLoadComplete(sl::api::IKernel* pKernel, IObject* object);
	void propSync(sl::api::IKernel* pKernel, IObject* object, const char* name, const IProp* prop, const bool sync);
	bool savePlayer(sl::api::IKernel* pKernel, IObject* player);

	void onSavePlayerStart(sl::api::IKernel*, IObject*, int64){}
	void onSavePlayerTime(sl::api::IKernel*, IObject*, int64);
	void onSavePlayerTerminate(sl::api::IKernel*, IObject*, bool, int64){}

	void OnNewDayComing(sl::api::IKernel* pKernel, const void* context, const int32 size);
	void OnNewWeekComing(sl::api::IKernel* pKernel, const void* context, const int32 size);
	void OnNewMonthComing(sl::api::IKernel* pKernel, const void* context, const int32 size);
	static void OnDayChange(sl::api::IKernel* pKernel, IObject* object, int64 tick);
	static void OnWeekChange(sl::api::IKernel* pKernel, IObject* object, int64 tick);
	static void OnMonthChange(sl::api::IKernel* pKernel, IObject* object, int64 tick);

	bool onClientTestReq(sl::api::IKernel* pKernel, IObject* object, const sl::OBStream& args);

private:
	PlayerMgr* _self;
	sl::api::IKernel* _kernel;
	IHarbor* _harbor;
	IObjectMgr* _objectMgr;
	IRoleMgr* _roleMgr;
	ICacheDB* _cacheDB;
	IObjectTimer* _objectTimer;
	ILogic*		  _logic;
	IPropDelaySender* _propDelaySender;
	static IEventEngine* s_eventEngine;

	std::unordered_map<sl::SLString<64>, int64> _onlines;
	int64	_recoverInterval;
	int64	_savePlayerInterval;
	int32	_foreachCount;
};
#endif