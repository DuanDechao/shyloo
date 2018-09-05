#include "GlobalDataClient.h"
#include "IDCCenter.h"
#include "IHarbor.h"
#include "NodeDefine.h"
#include "NodeProtocol.h"
bool GlobalDataClient::initialize(sl::api::IKernel * pKernel){
	_self = this;
	_kernel = pKernel;
	
	if(SLMODULE(Harbor)->getNodeType() == NodeType::DATABASE)
		return true;
	
	_globalData = NEW GlobalData();
	return true;
}

bool GlobalDataClient::launched(sl::api::IKernel * pKernel){
	if(SLMODULE(Harbor)->getNodeType() == NodeType::DATABASE)
		return true;

	RGS_NODE_HANDLER(SLMODULE(Harbor), NodeProtocol::GLOBAL_DATA_SERVER_CHANGED, GlobalDataClient::onGlobalDataChangedFromServer);
	return true;
}

bool GlobalDataClient::destory(sl::api::IKernel * pKernel){
	DEL this;
	return true;
}

void GlobalDataClient::addGlobalDataListener(IGlobalDataListener* listener){
	_globalData->addListener(listener);
}

bool GlobalDataClient::write(const char* key, const int16 dataType, const void* data, const int32 dataSize){
	_globalData->write(key, dataType, data, dataSize);
	sendGlobalDataChanged(key, dataType, data, dataSize, false);
}

bool GlobalDataClient::del(const char* key){
	_globalData->del(key);
	sendGlobalDataChanged(key, 0, nullptr, 0, true);
}

void GlobalDataClient::sendGlobalDataChanged(const char* key, const int16 dataType, const void* data, const int32 dataSize, bool isDelete){
	sl::BStream<10240> args;
	args << isDelete;
    args << key;
	
	if(!isDelete){
		args << dataType;
		args << dataSize;
		args.addBlob(data, dataSize);
	}
	SLMODULE(Harbor)->send(NodeType::DATABASE, 1, NodeProtocol::GLOBAL_DATA_CLIENT_CHANGED, args.out());
}
	
void GlobalDataClient::onGlobalDataChangedFromServer(sl::api::IKernel* pKernel, int32 nodeType, int32 nodeId, const sl::OBStream& args){
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
		int32 dataSize;
		args >> dataSize;
		int32 size = 0;
		const void* data = args.readBlob(size);
		_globalData->write(key, dataType, data, dataSize);
	}
}
	
const void* GlobalDataClient::getData(const char* key, int16& dataType, int32& dataSize){
	return _globalData->getData(key, dataType, dataSize);
}

bool GlobalDataClient::hasData(const char* key){
	return _globalData->hasData(key);
}
