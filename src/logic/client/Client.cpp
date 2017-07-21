#include "Client.h"
#include "ClientSession.h"
#include "slxml_reader.h"
#include "slstring_utils.h"
#define RECONNECT_INTERVAL 1 * SECOND

bool Client::initialize(sl::api::IKernel * pKernel){
	_kernel = pKernel;
	_listener = nullptr;
	_session = nullptr;
	return true;
}

bool Client::launched(sl::api::IKernel * pKernel){
	sl::XmlReader server_conf;
	if (!server_conf.loadXml(pKernel->getConfigFile())){
		SLASSERT(false, "can not load core file %s", pKernel->getConfigFile());
		return false;
	}
	const sl::ISLXmlNode& clientConf = server_conf.root()["client"][0];
	_clientRecvSize = clientConf.getAttributeInt32("recv");
	_clientSendSize = clientConf.getAttributeInt32("send");

	return true;
}

bool Client::destory(sl::api::IKernel * pKernel){
	_listener = nullptr;
	DEL this;
	return true;
}

void Client::onOpen(ClientSession* pSession){
	if (_listener)
		_listener->onServerConnected(_kernel);
}

int32 Client::onRecv(const char* pContext, const int32 size){
	if (_listener == nullptr)
		return 0;

	return _listener->onServerMsg(_kernel, pContext, size);
}

void Client::onClose(){
	if (_listener)
		_listener->onServerDisConnected(_kernel);
}

void Client::setListener(IClientListener* pListener){
	_listener = pListener;
}

void Client::connect(const char* ip, const int32 port){
	sl::api::IKernel* pKernel = _kernel;
	_session = NEW ClientSession(this);
	_session->setConnect(ip, port);
	if (!pKernel->startTcpClient(_session, ip, port, _clientSendSize, _clientRecvSize)){
		START_TIMER(_session, 0, TIMER_BEAT_FOREVER, RECONNECT_INTERVAL);
		ERROR_LOG("connect [%s:%d] failed!", ip, port);
	}
	else{
		TRACE_LOG("connect [%s:%d] success!", ip, port);
	}
}

void Client::close(){
	if (!_session){
		SLASSERT(false, "has no connect session");
		return;
	}
	_session->close();
	DEL _session;
	_session = nullptr;
}

void Client::send(const void* pBuf, const int32 size){
	_session->send(pBuf, size);
}