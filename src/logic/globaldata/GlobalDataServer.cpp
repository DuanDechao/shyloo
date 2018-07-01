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
    
	RGS_NODE_ARGS_HANDLER(SLMODULE(Harbor), NodeProtocol::GLOBAL_DATA_CLIENT_CHANGED, GlobalDataServer::onGlobalDataChangedFromClient);
	return true;
}

bool GlobalDataServer::destory(sl::api::IKernel * pKernel){
	DEL this;
	return true;
}

void GlobalDataServer::addGlobalDataListener(IGlobalDataListener* listener){
	_globalData->addListener(listener);
}

void GlobalDataServer::onGlobalDataChanged(const char* key, const int16 dataType, const void* data, const int32 dataSize, bool isDelete){
    IArgs<20, 10240> args;
	args << isDelete;
    args << key;
	
	if(!isDelete){
		args << dataType;
		args << dataSize;
		args.addStruct(data, dataSize);
	}
    args.fix();

	std::unordered_map<int32, std::set<int32>> excludeNodes;
	excludeNodes[_changedFromNodeType].insert(_changedFromNodeId);
	SLMODULE(Harbor)->broadcast(NodeProtocol::GLOBAL_DATA_SERVER_CHANGED, args.out(), excludeNodes);
}

void GlobalDataServer::onGlobalDataChangedFromClient(sl::api::IKernel* pKernel, int32 nodeType, int32 nodeId, const OArgs& args){
	_changedFromNodeType = nodeType;
	_changedFromNodeId = nodeId;

	bool isDelete = args.getBool(0);
	const char* key = args.getString(1);
	if(isDelete){
		_globalData->del(key);
	}
	else{
		const int16 dataType = args.getInt16(2);
		const int32 dataSize = args.getInt32(3);
		int32 size = 0;
		const void* data = args.getStruct(4, size);
		_globalData->write(key, dataType, data, dataSize);
	}
}


