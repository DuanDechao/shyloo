#include "Selector.h"

Selector::Selector()
	:AINode(nullptr)
{}

Selector::Selector(AICondition* condition)
	:AINode(condition)
{}

Selector::~Selector(){
	for (auto& node : _childNodes){
		if (node)
			DEL node;
	}
	_childNodes.clear();
}

bool Selector::onEnter(sl::api::IKernel* pKernel, IObject* object, int32 level){
	if (_childNodes.empty()){
		SLASSERT(false, "sequence must has children");
		return false;
	}
	
	int32 nodeSize = _childNodes.size();
	int32 i = 0;
	for (; i < nodeSize; i++){
		if (!_childNodes[i]->enter(pKernel, object, level + 1))
			_childNodes[i]->leave(pKernel, object, level + 1);
		else{
			break;
		}
	}
	if (i >= nodeSize){
		setParam(object, level, 0, 0);
		return false;
	}

	setParam(object, level, 0, i + 1);
	return true;
}

AINode::AIState Selector::onTick(sl::api::IKernel* pKernel, IObject* object, int32 level){
	int32 index = getParam(object, level, 0) - 1;
	if (index < 0 || index >= _childNodes.size()){
		SLASSERT(false, "params index is invailed");
		return AIState::AINODE_FAILTURE;
	}

	return _childNodes[index]->tick(pKernel, object, level + 1);
}

void Selector::onLeave(sl::api::IKernel* pKernel, IObject* object, int32 level){
	int32 index = getParam(object, level, 0) - 1;
	SLASSERT(index >= 0 && index < _childNodes.size(), "index is invailed");
	if (index >= 0 && index < _childNodes.size())
		_childNodes[index]->leave(pKernel, object, level + 1);
}