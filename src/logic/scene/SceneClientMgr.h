#ifndef __SL_FRAMEWORK_SCENECLINETMGR_H__
#define __SL_FRAMEWORK_SCENECLINETMGR_H__
#include "slsingleton.h"
#include "ISceneClientMgr.h"
#include "slbinary_stream.h"
#include "slstring.h"
#include "GameDefine.h"
class ILogic;
class IHarbor;
class IObject;
class IEventEngine;
class IObjectTimer;
class IProp;
class SceneClientMgr : public ISceneClientMgr, public sl::SLHolder<SceneClientMgr>{
public:
	virtual bool initialize(sl::api::IKernel * pKernel);
	virtual bool launched(sl::api::IKernel * pKernel);
	virtual bool destory(sl::api::IKernel * pKernel);

	bool onClientEnterScene(sl::api::IKernel* pKernel, IObject* object, const sl::OBStream& args);

	void onPlayerDataLoadCompleted(sl::api::IKernel* pKernel, const void* context, const int32 size);
	void onPlayerDestroy(sl::api::IKernel* pKernel, const void* context, const int32 size);
	void onStartSyncTimer(sl::api::IKernel* pKernel, const void* context, const int32 size);
	void onStopSyncTimer(sl::api::IKernel* pKernel, const void* context, const int32 size);
	void onObjectAppear(sl::api::IKernel* pKernel, const void* context, const int32 size);
	void onObjectDisappear(sl::api::IKernel* pKernel, const void* context, const int32 size);

	void onSyncToSceneStart(sl::api::IKernel*, IObject*, int64){}
	void onSyncToSceneTime(sl::api::IKernel*, IObject*, int64);
	void onSyncToSceneTerminate(sl::api::IKernel*, IObject*, bool, int64);

	void syncToScene(sl::api::IKernel* pKernel, IObject* object, const char* name, const IProp* prop, const bool sync);

private:
	void notifySceneMgrAppearScene(sl::api::IKernel* pKernel, IObject* object);
	void notifySceneMgrEnterScene(sl::api::IKernel* pKernel, IObject* object);
	void notifySceneMgrLeaveScene(sl::api::IKernel* pKernel, IObject* object);
	void playerAppearOnScene(sl::api::IKernel* pKernel, IObject* object, bool distribute);
	bool distributeScene(const char* scene, sl::SLString<game::MAX_SCENE_LEN>& sceneId);
	void sendSceneInfo(sl::api::IKernel* pKernel, IObject* object);

private:
	sl::api::IKernel* _kernel;
	SceneClientMgr*		_self;
	ILogic*				_logic;
	IHarbor*			_harbor;
	IEventEngine*		_eventEngine;
	IObjectTimer*		_objectTimer;

	int64				_syncToSceneInterval;
};
#endif