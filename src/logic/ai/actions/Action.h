#ifndef __SL_AI_ACTIONS_ACTION_H__
#define __SL_AI_ACTIONS_ACTION_H__
#include "../AINode.h"
class Action : public AINode{
public:
	virtual bool onEnter(sl::api::IKernel* pKernel, IObject* object, int32 level);
	virtual AINode::AIState onTick(sl::api::IKernel* pKernel, IObject* object, int32 level);
	virtual void onLeave(sl::api::IKernel* pKernel, IObject* object, int32 level);
};
#endif