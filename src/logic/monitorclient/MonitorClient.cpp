#include "MonitorClient.h"
#include "slstring_utils.h"
#include "MonitorProtocol.h"

bool MonitorClient::initialize(sl::api::IKernel * pKernel){
	_client = nullptr;
	return true;
}

bool MonitorClient::launched(sl::api::IKernel * pKernel){
	FIND_MODULE(_client, Client);

	const char* svrIp = pKernel->getCmdArg("server_ip");
	const int32 svrPort = sl::CStringUtils::StringAsInt32(pKernel->getCmdArg("server_port"));

	_client->setListener(this);

	_client->connect(svrIp, svrPort);

	RSG_MONITOR_FUNC(this, MONITOR_FUNC_SVR_SHUTDOWN, MonitorClient::shutDownServer);

	return true;
}

bool MonitorClient::destory(sl::api::IKernel * pKernel){
	DEL this;
	return true;
}

void MonitorClient::rgsMonitorFunc(int32 funcId, const MONITOR_FUNC& func){
	SLASSERT(_monitorFunc.find(funcId) == _monitorFunc.end(), "duplicate monitor func %d", funcId);
	_monitorFunc[funcId] = func;
}

void MonitorClient::rgsSvrMessageHandler(int32 messageId, const MONITOR_CB& handler){
	SLASSERT(_svrProtos.find(messageId) == _svrProtos.end(), "duplicate svr msg %d", messageId);
	_svrProtos[messageId] = handler;
}

void MonitorClient::onServerConnected(sl::api::IKernel* pKernel){
	int32 funcId = sl::CStringUtils::StringAsInt32(pKernel->getCmdArg("func"));
	switch (funcId){
	case MONITOR_FUNC_SVR_SHUTDOWN: shutDownServer(pKernel); break;
	default:
		break;
	}
}

void MonitorClient::onServerDisConnected(sl::api::IKernel* pKernel){
	pKernel->shutdown();
}

void MonitorClient::shutDownServer(sl::api::IKernel* pKernel){
	sl::IBStream<32> args;
	sendToSvr(pKernel, MonitorProtocol::CLIENT_SHUTDOWN_SERVER_REQ, args.out());
}

int32 MonitorClient::onServerMsg(sl::api::IKernel* pKernel, const void* context, const int32 size){
	if (size < sizeof(int32)* 2){
		return 0;
	}

	int32 len = *((int32*)((const char*)context + sizeof(int32)));
	if (size < len){
		return 0;
	}

	int32 msgId = *((int32*)context);
	if (_svrProtos.find(msgId) != _svrProtos.end()){
		sl::OBStream buf((const char*)context + sizeof(int32)* 2, len);
		_svrProtos[msgId](pKernel, buf);
	}
	else{
		//SLASSERT(false, "can not find message id[%d]", msgId);
	}
	return len;
}

void MonitorClient::sendToSvr(sl::api::IKernel* pKernel, const int32 msgId, const sl::OBStream& buf){
	int32 header[2];
	header[0] = msgId;
	header[1] = buf.getSize() + sizeof(int32)* 2;

	_client->send(header, sizeof(header));
	_client->send(buf.getContext(), buf.getSize());
}
