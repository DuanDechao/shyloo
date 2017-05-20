#ifndef __SL_FRAMEWORK__OBJECTLOCATOR_H__
#define __SL_FRAMEWORK__OBJECTLOCATOR_H__
#include "IObjectLocator.h"
#include "slsingleton.h"
#include <unordered_map>

class IEventEngine;
class IHarbor;
class ObjectLocator : public IObjectLocator, public sl::SLHolder<ObjectLocator>{
	struct ObjectLocation{
		int32 _logic;
		int32 _gate;
	};
public:
	virtual bool initialize(sl::api::IKernel * pKernel);
	virtual bool launched(sl::api::IKernel * pKernel);
	virtual bool destory(sl::api::IKernel * pKernel);

	virtual int32 findObjectGate(int64 id);
	virtual int32 findObjectLogic(int64 id);

	void onSyncLocation(sl::api::IKernel* pKernel, const void* context, const int32 size);
	void onRemoveLocation(sl::api::IKernel* pKernel, const void* context, const int32 size);
	
	void onLogicSyncObjectLocation(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId, const OArgs& args);
	void onLogicRemoveObjectLocation(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId, const OArgs& args);

private:
	ObjectLocator* _self;
	sl::api::IKernel* _kernel;
	IHarbor*		_harbor;
	IEventEngine*	_eventEngine;

	std::unordered_map<int64, ObjectLocation> _allObjects;

};
#endif