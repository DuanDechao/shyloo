#ifndef __SL_FRAMEWORK_AI_NODE_H__
#define __SL_FRAMEWORK_AI_NODE_H__
#include "slikernel.h"
class IObject;
class AICondition;
class AINode{
public:
	enum AIState{
		AINODE_COMPLETE = 0,
		AINODE_RUNNING,
		AINODE_FAILTURE
	};
	AINode(AICondition* condition);
	~AINode();

	bool enter(sl::api::IKernel* pKernel, IObject* object, int32 level = 0);
	AIState tick(sl::api::IKernel* pKernel, IObject* object, int32 level = 0);
	void leave(sl::api::IKernel* pKernel, IObject* object, int32 level = 0);

	bool checkCondition(sl::api::IKernel* pKernel, IObject* object);

	AIState getState(IObject* object, int32 level = 0) const;

protected:
	virtual bool onEnter(sl::api::IKernel* pKernel, IObject* object, int32 level) = 0;
	virtual AIState onTick(sl::api::IKernel* pKernel, IObject* object, int32 level) = 0;
	virtual void onLeave(sl::api::IKernel* pKernel, IObject* object, int32 level) = 0;

	void setState(IObject* object, int32 level, int8 state) const;
	void setParam(IObject* object, int32 level, int32 index, int64 param) const;
	int64 getParam(IObject* object, int32 level, int32 index) const;

	void addLevel(IObject* object, int32 level) const;
	void removeLevel(IObject* object, int32 level) const;
private:
	AICondition* _condition;
};
#endif