#ifndef __SL_AI_SEQUENCE_H__
#define __SL_AI_SEQUENCE_H__
#include "../AINode.h"
#include "../AICondition.h"
#include <vector>

class Sequence: public AINode{
public:
	Sequence();
	Sequence(AICondition* condition);
	~Sequence();

	inline void addChild(AINode* node) { _childNodes.push_back(node); }

	virtual bool onEnter(sl::api::IKernel* pKernel, IObject* object, int32 level);
	virtual AINode::AIState onTick(sl::api::IKernel* pKernel, IObject* object, int32 level);
	virtual void onLeave(sl::api::IKernel* pKernel, IObject* object, int32 level);

private:
	std::vector<AINode* > _childNodes;
};
#endif