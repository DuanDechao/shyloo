#include "AIMachine.h"

void AIMachine::run(sl::api::IKernel* pKernel, IObject* object){
	if (!_root){
		SLASSERT(false, "has no ai root");
		return;
	}

	if (_root->getState(object) == AINode::AINODE_RUNNING){
		auto ret = _root->tick(pKernel, object);
		if (ret != AINode::AINODE_RUNNING){
			_root->leave(pKernel, object);
			if (ret == AINode::AINODE_COMPLETE)
				return;
		}
		else
			return;
	}
	
	if (!_root->enter(pKernel, object)){
		_root->leave(pKernel, object);
		return;
	}

	if (_root->tick(pKernel, object) != AINode::AINODE_RUNNING){
		_root->leave(pKernel, object);
		return;
	}
}