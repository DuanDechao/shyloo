#include "Action.h"

bool Action::onEnter(sl::api::IKernel* pKernel, IObject* object, int32 level){
	return true;
}
AINode::AIState Action::onTick(sl::api::IKernel* pKernel, IObject* object, int32 level){
	ECHO_TRACE("pick item");
	return AINode::AIState::AINODE_COMPLETE;
}
void Action::onLeave(sl::api::IKernel* pKernel, IObject* object, int32 level){

}