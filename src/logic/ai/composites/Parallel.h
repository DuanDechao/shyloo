#ifndef __SL_AI_PARALLEL_H__
#define __SL_AI_PARALLEL_H__
#include "../AINode.h"
#include "../AICondition.h"
#include <vector>

class Parallel : public AINode{
public:
	Parallel();
	Parallel(AICondition* condition);
	~Parallel();

	inline void addChild(AINode* node) { _childNodes.push_back(node); }

	virtual bool onEnter(sl::api::IKernel* pKernel, IObject* object, int32 level);
	virtual AINode::AIState onTick(sl::api::IKernel* pKernel, IObject* object, int32 level);
	virtual void onLeave(sl::api::IKernel* pKernel, IObject* object, int32 level);

private:
	std::vector<AINode*> _childNodes;
};
#endif