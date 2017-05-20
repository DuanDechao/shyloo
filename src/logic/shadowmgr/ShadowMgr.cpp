#include "ShadowMgr.h"
#include "IHarbor.h"
#include "NodeProtocol.h"

bool ShadowMgr::initialize(sl::api::IKernel * pKernel){
	_self = this;
	_kernel = pKernel;
	return true;
}

bool ShadowMgr::launched(sl::api::IKernel * pKernel){
	FIND_MODULE(_harbor, Harbor);
	
	RGS_NODE_ARGS_HANDLER(_harbor, NodeProtocol::SCENE_MSG_CREATE_SHADOW, ShadowMgr::onSceneCreateShadow);
	RGS_NODE_ARGS_HANDLER(_harbor, NodeProtocol::LOGIC_MSG_SYNC_SHADOW, ShadowMgr::onLogicSyncShadow);
	return true;
}
bool ShadowMgr::destory(sl::api::IKernel * pKernel){
	DEL this;
	return true;
}

void ShadowMgr::onSceneCreateShadow(sl::api::IKernel* pKernel, int32 nodeType, int32 nodeId, const OArgs& args){

}

void ShadowMgr::onLogicSyncShadow(sl::api::IKernel* pKernel, int32 nodeType, int32 nodeId, const OArgs& args){

}