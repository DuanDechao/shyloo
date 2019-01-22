#include "GlobalDataServer.h"
#include "IDCCenter.h"
#include "IHarbor.h"
#include "NodeDefine.h"
#include "NodeProtocol.h"
bool GlobalDataServer::initialize(sl::api::IKernel * pKernel){
	_self = this;
	_kernel = pKernel;
	_changedFromNodeId = 0;
	_changedFromNodeType = 0;
	
	if(SLMODULE(Harbor)->getNodeType() != NodeType::DATABASE)
		return true;
	
	_globalData = NEW GlobalData();
	_globalData->addListener(this);

	return true;
}

bool GlobalDataServer::launched(sl::api::IKernel * pKernel){
	if(SLMODULE(Harbor)->getNodeType() != NodeType::DATABASE)
		return true;
    
	RGS_NODE_HANDLER(SLMODULE(Harbor), NodeProtocol::GLOBAL_DATA_CLIENT_CHANGED, GlobalDataServer::onGlobalDataChangedFromClient);
	return true;
}

bool GlobalDataServer::destory(sl::api::IKernel * pKernel){
	DEL _globalData;
	DEL this;
	return true;
}

void GlobalDataServer::addGlobalDataListener(IGlobalDataListener* listener){
	_globalData->addListener(listener);
}

void GlobalDataServer::onGlobalDataChanged(const char* key, const int16 dataType, const void* data, const int32 dataSize, bool isDelete){
	sl::BStream<10240> args;
	args << isDelete;
    args << key;
	
	if(!isDelete){
		args << dataType;
		args << dataSize;
		args.addBlob(data, dataSize);
	}

	std::unordered_map<int32, std::set<int32>> excludeNodes;
	excludeNodes[_changedFromNodeType].insert(_changedFromNodeId);
	SLMODULE(Harbor)->broadcast(NodeProtocol::GLOBAL_DATA_SERVER_CHANGED, args.out(), excludeNodes);
}

void GlobalDataServer::onGlobalDataChangedFromClient(sl::api::IKernel* pKernel, int32 nodeType, int32 nodeId, const sl::OBStream& args){
	_changedFromNodeType = nodeType;
	_changedFromNodeId = nodeId;

	bool isDelete = false;
	args >> isDelete;
	const char* key = nullptr;
	args >> key;
	if(isDelete){
		_globalData->del(key);
	}
	else{
		int16 dataType = 0;
		args >> dataType;
		int32 dataSize = 0;
		args >> dataSize;
		int32 size = 0;
		const void* data = args.readBlob(size);
		_globalData->write(key, dataType, data, dataSize);
	}
}


