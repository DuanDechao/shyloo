#include "AIMachine.h"

void AIMachine::run(sl::api::IKernel* pKernel, IObject* object){
	
	auto ret = _root->tick(pKernel, object);
	if (ret != AINode::AINODE_RUNNING){
		_root->leave(pKernel, object);
		if (ret == AINode::AINODE_COMPLETE)
			return;
	}
	else
		return;

	if (!_root->enter(pKernel, object)){
		_root->leave(pKernel, object);
		return;
	}

	if (_root->tick(pKernel, object) != AINode::AINODE_RUNNING){
		_root->leave(pKernel, object);
		return;
	}
}