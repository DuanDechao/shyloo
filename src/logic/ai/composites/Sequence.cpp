#include "Sequence.h"
Sequence::Sequence()
	: AINode(nullptr)
{}

Sequence::Sequence(AICondition* condition)
	: AINode(condition)
{}

Sequence::~Sequence(){}

bool Sequence::onEnter(sl::api::IKernel* pKernel, IObject* object, int32 level){
	if (_childNodes.empty()){
		SLASSERT(false, "sequence must has children");
		return false;
	}

	if (!_childNodes[0]->enter(pKernel, object, level + 1)){
		_childNodes[0]->leave(pKernel, object, level + 1);
		return false;
	}

	setParam(object, level, 0, 1);
	return true;
}
AINode::AIState Sequence::onTick(sl::api::IKernel* pKernel, IObject* object, int32 level){
	int32 index = getParam(object, level, 0) - 1;
	if (index < 0 || index >= _childNodes.size()){
		SLASSERT(false, "index is invailed");
		return AINode::AIState::AINODE_FAILTURE;
	}

	auto ret = _childNodes[index]->tick(pKernel, object, level + 1);
	if (ret == AINode::AIState::AINODE_FAILTURE)
		return AINode::AIState::AINODE_FAILTURE;
	else if (ret == AINode::AIState::AINODE_COMPLETE){
		_childNodes[index]->leave(pKernel, object, level + 1);
		++index;
		if (index >= _childNodes.size()){
			setParam(object, level, 0, 0);
			return AINode::AIState::AINODE_COMPLETE;
		}

		if (!_childNodes[index]->enter(pKernel, object, level + 1)){
			_childNodes[index]->leave(pKernel, object, level + 1);
			setParam(object, level, 0, 0);
			return AINode::AIState::AINODE_FAILTURE;
		}
		setParam(object, level, 0, index + 1);
	}
	
	return AINode::AIState::AINODE_RUNNING;
}
void Sequence::onLeave(sl::api::IKernel* pKernel, IObject* object, int32 level){
	int32 index = getParam(object, level, 0) - 1;
	SLASSERT(index >= 0 && index < _childNodes.size(), "index is invailed");
	if (index >= 0 && index < _childNodes.size())
		_childNodes[index]->leave(pKernel, object, level + 1);
}