#include "Parallel.h"

Parallel::Parallel()
	:AINode(nullptr)
{}

Parallel::Parallel(AICondition* condition)
	: AINode(condition)
{}

Parallel::~Parallel(){}

bool Parallel::onEnter(sl::api::IKernel* pKernel, IObject* object, int32 level){
	return true;
}

AINode::AIState Parallel::onTick(sl::api::IKernel* pKernel, IObject* object, int32 level){
	return AINode::AIState::AINODE_COMPLETE;
}

void Parallel::onLeave(sl::api::IKernel* pKernel, IObject* object, int32 level){

}