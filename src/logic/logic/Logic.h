#ifndef __SL_FRAMEWORK_LOGIC_H__
#define __SL_FRAMEWORK_LOGIC_H__
#include "slikernel.h"
#include "slimodule.h"
#include "ILogic.h"
#include "slsingleton.h"
#include <unordered_map>
#include <unordered_set>
#include "IHarbor.h"
class IObjectMgr;
class IRoleMgr;
class IEventEngine;
class IObject;
class Logic :public ILogic, public INodeListener, public sl::SLHolder<Logic>{
public:
	virtual bool initialize(sl::api::IKernel * pKernel);
	virtual bool launched(sl::api::IKernel * pKernel);
	virtual bool destory(sl::api::IKernel * pKernel);

	virtual void onOpen(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId, const char* ip, const int32 port){}
	virtual void onClose(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId);

	void onGateBindPlayerOnLogic(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId, const OArgs& args);
	void onGateUnBindPlayerOnLogic(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId, const OArgs& args);

	void recoverObject(sl::api::IKernel* pKernel, const int64 id);
private:

private:
	Logic*		_self;
	IHarbor*	_harbor;
	IObjectMgr* _objectMgr;
	IRoleMgr*	_roleMgr;
	IEventEngine* _eventEngine;

	std::unordered_map<int32, std::unordered_set<int64>> _gateActors;
	int64	_recoverInterval;
};

#endif