#include "Cluster.h"
#include "NodeDefine.h"
#include "NodeProtocol.h"
#include "ICluster.h"
bool Cluster::initialize(sl::api::IKernel * pKernel){
	_self = this;
	_kernel = pKernel;
	_lastUpdateTime = 0;
	_currNodeInfo = {0.f, ServerState::S_INIT};
	return true;
}

bool Cluster::launched(sl::api::IKernel * pKernel){
	if(SLMODULE(Harbor)->getNodeType() == NodeType::MASTER){
		RGS_NODE_HANDLER(SLMODULE(Harbor), NodeProtocol::CLUSTER_MSG_UPDATE_NODE_INFO, Cluster::nodeInfoReport);
		_currNodeInfo.state = ServerState::S_READY;
	}
	SLMODULE(Harbor)->addNodeListener(this);
	
	_lastUpdateTime = sl::getTimeMilliSecond();
	START_TIMER(_self, 0, -1, 2000);
	
	return true;
}

bool Cluster::destory(sl::api::IKernel * pKernel){
	DEL this;
	return true;
}

void Cluster::onOpen(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId, const char* ip, const int32 port){
	if(nodeType == NodeType::MASTER && _currNodeInfo.state == ServerState::S_INIT){
		_currNodeInfo.state = ServerState::S_READY;
	}

	if(SLMODULE(Harbor)->getNodeType() == NodeType::MASTER){
		_allNodeInfo[nodeType][nodeId] = {0.f, ServerState::S_INIT};
	}
}

void Cluster::onClose(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId){
	auto itor = _allNodeInfo.find(nodeType);
	if(itor == _allNodeInfo.end())
		return;
	
	auto node = itor->second.find(nodeId);
	if(node == itor->second.end())
		return;
	
	itor->second.erase(node);
	if(itor->second.size() <= 0){
		_allNodeInfo.erase(itor);
	}
}

void Cluster::addServerProcessHandler(IServerProcessHandler* handler){
	_handlers.push_back(handler);
}

bool Cluster::serverReady(){
	for(auto handler : _handlers){
		if(!handler->onServerReady(_kernel))
			return false;
	}
	return true;
}

bool Cluster::serverReadyForLogin(){
	for(auto handler : _handlers){
		if(!handler->onServerReadyForLogin(_kernel))
			return false;
	}
	return true;
}

bool Cluster::serverReadyForShutDown(){
	for(auto handler : _handlers){
		if(!handler->onServerReadyForShutDown(_kernel))
			return false;
	}
	return true;
}

bool Cluster::serverShutDown(){
	for(auto handler : _handlers){
		if(!handler->onServerShutDown(_kernel))
			return false;
	}
	return true;
}

void Cluster::onTime(sl::api::IKernel* pKernel, int64 timetick){
	uint64 passTime = timetick - _lastUpdateTime;
	_lastUpdateTime = timetick;
	if(passTime <= 0)
		return;
	
	checkServerState();
	updateLoad(pKernel, timetick, passTime);
	onUpdateNodeInfo();
}

void Cluster::checkServerState(){
	switch(_currNodeInfo.state){
	case ServerState::S_READY:{
		if(serverReady())
			_currNodeInfo.state = ServerState::S_READY_FOR_LOGIN;
		break;
	}
	case ServerState::S_READY_FOR_LOGIN:{
		if(serverReadyForLogin())
			_currNodeInfo.state = ServerState::S_RUNNING;
		break;
	}
	case ServerState::S_PRE_SHUTDOWN:{
		if(serverReadyForShutDown())
			_currNodeInfo.state = ServerState::S_SHUTDOWN;
		break;
	}
	case ServerState::S_SHUTDOWN:{
		if(serverShutDown())
			_currNodeInfo.state = ServerState::S_NONE;
		break;
	}
	}
}

void Cluster::nodeInfoReport(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId, const sl::OBStream& args){
	float nodeLoad = 0.0f;
	int32 state = 0;
	args >> nodeLoad >> state;
	_allNodeInfo[nodeType][nodeId] = { nodeLoad, state};
	bestNodeId(nodeType);

	for(auto itor : _allNodeInfo){
		for(auto node : itor.second){
			printf("-------------- %d %d %d %f---------------------\n", itor.first, node.first, node.second.state, node.second.load);
		}
	}
}

void Cluster::updateLoad(sl::api::IKernel* pKernel, int64 timetick, int64 passTime){
	double spareTimePercent = double(pKernel->getSpareTime()) / double(passTime);
	pKernel->clearSpareTime();

	if(spareTimePercent < 0.f || spareTimePercent > 1.f){
		SLASSERT(false, "spareTime calc error!!!!");
	}

	if(spareTimePercent < 0.f)
		spareTimePercent = 0.f;
	if(spareTimePercent > 1.f)
		spareTimePercent = 1.f;

	float load = 1.f - spareTimePercent;

	//smooth the load changes
	static float alpha = 0.5;
	_currNodeInfo.load = (1 - alpha) * _currNodeInfo.load + alpha * load;
}

void Cluster::onUpdateNodeInfo(){
	int32 nodeType = SLMODULE(Harbor)->getNodeType();
	if(nodeType == NodeType::MASTER){
		int32 nodeId = SLMODULE(Harbor)->getNodeId();
		_allNodeInfo[nodeType][nodeId] = { _currNodeInfo.load, _currNodeInfo.state};
		return;
	}

	sl::BStream<256> args;
	args << (float)_currNodeInfo.load;
	args << (int32)_currNodeInfo.state;
	SLMODULE(Harbor)->send(NodeType::MASTER, 1, NodeProtocol::CLUSTER_MSG_UPDATE_NODE_INFO, args.out());
}

int32 Cluster::findFreeNodeId(int32 nodeType){
	auto itor = _allNodeInfo.find(nodeType);
	if(itor == _allNodeInfo.end())
		return 0;

	float minLoad = 1.f;
	int32 freeNodeId = 0;
	for(auto node : itor->second){
		if(minLoad > node.second.load && node.second.state != ServerState::S_NONE){
			freeNodeId = node.first;
			minLoad = node.second.load;
		}
	}
	return freeNodeId;
}

int32 Cluster::getBestNodeId(int32 nodeType){
	auto itor = _bestNodeId.find(nodeType);
	if(itor == _bestNodeId.end())
		return 0;
	return itor->second;
}

void Cluster::bestNodeId(int32 nodeType){
	int32 bestNodeId = findFreeNodeId(nodeType);
	_bestNodeId[nodeType] = bestNodeId;
}

int32 Cluster::getNodeState(int32 nodeType, int32 nodeId){
	auto itor = _allNodeInfo.find(nodeType);
	if(itor == _allNodeInfo.end())
		return ServerState::S_NONE;
	auto node = itor->second.find(nodeId);
	if(node == itor->second.end())
		return ServerState::S_NONE;

	return node->second.state;
}
