#ifndef SL_LOGIC_AI_H
#define SL_LOGIC_AI_H
#include "slikernel.h"
#include "IAI.h"
#include "slsingleton.h"
#include <unordered_map>
#include "slstring.h"
#include "GameDefine.h"

class IHarbor;
class IObject;
class IScriptEngine;
class AIMachine;
class IObjectTimer;
#define MAX_AI_CONFIG_NAME_LEN 32
class AI :public IAI, public sl::SLHolder<AI>{
public:
	struct AIConfig{
		int32 interval;
		AIMachine* machine;
	};

	virtual bool initialize(sl::api::IKernel * pKernel);
	virtual bool launched(sl::api::IKernel * pKernel);
	virtual bool destory(sl::api::IKernel * pKernel);

	virtual void startAI(IObject* object);
	virtual void stopAI(IObject* object);

	IScriptEngine* GetScriptEngine(){ return _scriptEngine; }

	void onAIStart(sl::api::IKernel* pKernel, IObject* object, int64);
	void onAITick(sl::api::IKernel* pKernel, IObject* object, int64);
	void onAIEnd(sl::api::IKernel* pKernel, IObject* object, bool, int64);

private:
	void startAI(sl::api::IKernel* pKernel, IObject* object);
	void stopAI(sl::api::IKernel* pKernel, IObject* object);

private:
	typedef std::unordered_map<sl::SLString<MAX_AI_CONFIG_NAME_LEN>, AIConfig, sl::HashFunc<MAX_AI_CONFIG_NAME_LEN>, sl::EqualFunc<MAX_AI_CONFIG_NAME_LEN>> AI_CONFIG_MAP;

	sl::api::IKernel*		_kernel;
	AI*						_self;
	IObjectTimer*			_objectTimer;
	IScriptEngine*			_scriptEngine;

	AI_CONFIG_MAP	        _ais;
};

#endif