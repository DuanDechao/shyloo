#include "MonitorClient.h"
#include "IClient.h"
#include "slstring_utils.h"

bool MonitorClient::initialize(sl::api::IKernel * pKernel){
	return true;
}

bool MonitorClient::launched(sl::api::IKernel * pKernel){
	FIND_MODULE(_client, Client);

	const char* svrIp = pKernel->getCmdArg("server_ip");
	const int32 svrPort = sl::CStringUtils::StringAsInt32(pKernel->getCmdArg("server_port"));

	_client->connect(svrIp, svrPort);

	return true;
}

bool MonitorClient::destory(sl::api::IKernel * pKernel){
	DEL this;
	return true;
}


void MonitorClient::sendToSvr(sl::api::IKernel* pKernel, const int64 id, const int32 msgId, const sl::OBStream& buf){
	int32 header[2];
	header[0] = msgId;
	header[1] = buf.getSize() + sizeof(int32)* 2;

	_client->send(id, header, sizeof(header));
	_client->send(id, buf.getContext(), buf.getSize());
}
