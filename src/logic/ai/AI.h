#ifndef SL_LOGIC_AI_H
#define SL_LOGIC_AI_H
#include "slikernel.h"
#include "IAI.h"
#include "slsingleton.h"
class IHarbor;
class IObject;
class IScriptEngine;
class AI :public IAI, public sl::SLHolder<AI>{
public:
	virtual bool initialize(sl::api::IKernel * pKernel);
	virtual bool launched(sl::api::IKernel * pKernel);
	virtual bool destory(sl::api::IKernel * pKernel);

	virtual void startAI(IObject* object);
	virtual void stopAI(IObject* object);

	static IScriptEngine* GetScriptEngine(){ return s_scriptEngine; }

private:
	sl::api::IKernel*   _kernel;
	AI*					_self;

	static IScriptEngine* s_scriptEngine;
};

#endif