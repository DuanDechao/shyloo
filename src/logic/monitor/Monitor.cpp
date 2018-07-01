#include "Monitor.h"
#include "IHarbor.h"
#include "NodeDefine.h"
#include "StartUpHandler.h"

class MonitorCBMessageHandler : public IMonitorMessageHandler{
public:
	MonitorCBMessageHandler(const MONITOR_CB cb) : _cb(cb){}
	virtual ~MonitorCBMessageHandler() {}

	virtual void DealMonitorMessage(sl::api::IKernel* pKernel, const int64 id, const char* pContext, const int32 size){
		_cb(pKernel, id, pContext, size);
	}
private:
	MONITOR_CB		_cb;
};


class MonitorArgsCBMessageHandler : public IMonitorMessageHandler{
public:
	MonitorArgsCBMessageHandler(const MONITOR_ARGS_CB cb) : _cb(cb){}
	virtual ~MonitorArgsCBMessageHandler() {}

	virtual void DealMonitorMessage(sl::api::IKernel* pKernel, const int64 id, const char* pContext, const int32 size){
		sl::OBStream args(pContext, size);
		_cb(pKernel, id, args);
	}
private:
	MONITOR_ARGS_CB _cb;
};

bool Monitor::initialize(sl::api::IKernel * pKernel){
	_kernel = pKernel;
	return true;
}

bool Monitor::launched(sl::api::IKernel * pKernel){
	FIND_MODULE(_harbor, Harbor);
	if (_harbor->getNodeType() == NodeType::MASTER){
		FIND_MODULE(_agent, Agent);
		_agent->setListener(this);
	}
	else{
		if(!_startUpHandler)
			_startUpHandler = NEW StartUpHandler();
	}
	
	return true;
}

bool Monitor::destory(sl::api::IKernel * pKernel){
	DEL this;
	return true;
}

void Monitor::rgsMonitorMessageHandler(int32 messageId, const MONITOR_CB& handler, const char* debug){
	SLASSERT(_monitorProtos.find(messageId) == _monitorProtos.end(), "duplicate agent msg %d", messageId);
	_monitorProtos[messageId] = NEW MonitorCBMessageHandler(handler);
}

void Monitor::rgsMonitorArgsMessageHandler(int32 messageId, const MONITOR_ARGS_CB& handler, const char* debug){
	SLASSERT(_monitorProtos.find(messageId) == _monitorProtos.end(), "duplicate agent msg %d", messageId);
	_monitorProtos[messageId] = NEW MonitorArgsCBMessageHandler(handler);
}

void Monitor::addListener(IMonitorListener* listener){
	_listeners.push_back(listener);
}

void Monitor::onAgentOpen(sl::api::IKernel* pKernel, const int64 id){

}

void Monitor::onAgentClose(sl::api::IKernel* pKernel, const int64 id){

}

int32 Monitor::onAgentRecv(sl::api::IKernel* pKernel, const int64 id, const void* context, const int32 size){
	if (size < sizeof(int32)* 2){
		return 0;
	}

	int32 len = *((int32*)((const char*)context + sizeof(int32)));
	if (size < len){
		return 0;
	}

	int32 msgId = *((int32*)context);
	if (_monitorProtos.find(msgId) != _monitorProtos.end()){
		_monitorProtos[msgId]->DealMonitorMessage(pKernel, id, (const char*)context + sizeof(int32)* 2, len);
	}
	return len;
}

bool Monitor::serverReady(){
	for(auto listener : _listeners){
		if(!listener->onServerReady(_kernel))
			return false;
	}
	return true;
}

bool Monitor::serverReadyForLogin(){
	for(auto listener : _listeners){
		if(!listener->onServerReadyForLogin(_kernel))
			return false;
	}
	return true;
}

bool Monitor::serverReadyForShutDown(){
	for(auto listener : _listeners){
		if(!listener->onServerReadyForShutDown(_kernel))
			return false;
	}
	return true;
}

bool Monitor::serverShutDown(){
	for(auto listener : _listeners){
		if(!listener->onServerShutDown(_kernel))
			return false;
	}
	return true;
}
