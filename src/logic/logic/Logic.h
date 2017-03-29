#ifndef __SL_FRAMEWORK_LOGIC_H__
#define __SL_FRAMEWORK_LOGIC_H__
#include "slikernel.h"
#include "slimodule.h"
#include "ILogic.h"
#include "slsingleton.h"
#include <unordered_map>
#include <unordered_set>

class IHarbor;
class IObjectMgr;
class Logic :public ILogic, public sl::SLHolder<Logic>{
public:
	virtual bool initialize(sl::api::IKernel * pKernel);
	virtual bool launched(sl::api::IKernel * pKernel);
	virtual bool destory(sl::api::IKernel * pKernel);

	void onGateBindPlayerOnLogic(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId, const OArgs& args);
	void onGateUnBindPlayerOnLogic(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId, const OArgs& args);

private:

private:
	Logic*		_self;
	IHarbor*	_harbor;
	IObjectMgr* _objectMgr;
};
#endif