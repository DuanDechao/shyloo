#ifndef __SL_FRAMEWORK_AOI_H__
#define __SL_FRAMEWORK_AOI_H__
#include "slsingleton.h"
#include "IAOI.h"

class IHarbor;
class IObjectMgr;
class IShadowMgr;
class IPacketSender;
class IEventEngine;
class AOI : public IAOI, public sl::SLHolder<AOI>{
	
public:
	virtual bool initialize(sl::api::IKernel * pKernel);
	virtual bool launched(sl::api::IKernel * pKernel);
	virtual bool destory(sl::api::IKernel * pKernel);

	virtual void broadcast(IObject* object, int32 messageId, const sl::OBStream& args, bool self = false);
	virtual void foreachNeighbor(IObject* object, const std::function<void(sl::api::IKernel* pKernel, IObject* object)>& f);
	virtual bool isNeighbor(IObject* object, const int64 id);

	void onSceneAddInterester(sl::api::IKernel* pKernel, int32 nodeType, int32 nodeId, const OArgs& args);
	void onSceneAddWatcher(sl::api::IKernel* pKernel, int32 nodeType, int32 nodeId, const OArgs& args);
	void onSceneRemoveInterester(sl::api::IKernel* pKernel, int32 nodeType, int32 nodeId, const OArgs& args);
	void onSceneRemoveWatcher(sl::api::IKernel* pKernel, int32 nodeType, int32 nodeId, const OArgs& args);

private:
	void addWatcher(sl::api::IKernel* pKernel, IObject* object, int64 watcherId, int32 logic, int32 gate);
	void removeWatcher(sl::api::IKernel* pKernel, IObject* object, int64 watcherId);

	void addInterester(sl::api::IKernel* pKernel, IObject* object, int64 interester, int32 type);
	void removeInterester(sl::api::IKernel* pKernel, IObject* object, int64 interester);

	void notifyWatcherObjectAppear(sl::api::IKernel* pKernel, int32 gate, int64 watcherId, IObject* object);
	void notifyWatcherObjectDisappear(sl::api::IKernel* pKernel, int32 gate, int64 watcherId, IObject* object);

private:
	sl::api::IKernel* _kernel;
	AOI*			  _self;
	IHarbor*		  _harbor;
	IObjectMgr*		  _objectMgr;
	IShadowMgr*		  _shadowMgr;
	IPacketSender*	  _packetSender;
	IEventEngine*	  _eventEngine;
};
#endif