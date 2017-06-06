#include "Client.h"
#include "ClientSession.h"
#include "slxml_reader.h"
#include "slstring_utils.h"
#define RECONNECT_INTERVAL 1 * SECOND

sl::api::ITcpSession* ClientSessionPool::mallocTcpSession(sl::api::IKernel* pKernel){
	return CREATE_POOL_OBJECT(ClientSession, m_pClient);
}

bool Client::initialize(sl::api::IKernel * pKernel){
	_kernel = pKernel;
	_clientNextId = 0;
	return true;
}

bool Client::launched(sl::api::IKernel * pKernel){
	sl::XmlReader server_conf;
	if (!server_conf.loadXml(pKernel->getCoreFile())){
		SLASSERT(false, "can not load core file %s", pKernel->getCoreFile());
		return false;
	}
	const sl::ISLXmlNode& clientConf = server_conf.root()["client"][0];
	_clientRecvSize = clientConf.getAttributeInt32("recv");
	_clientSendSize = clientConf.getAttributeInt32("send");

	_sessionPool = NEW ClientSessionPool(this);

	return true;
}

bool Client::destory(sl::api::IKernel * pKernel){
	_listener = nullptr;
	_clientNextId = 0;
	
	DEL this;
	return true;
}

int64 Client::onOpen(ClientSession* pSession){
	if (_clientNextId <= 0)
		_clientNextId = 1;
	
	int64 ret = _clientNextId++;
	_clientSessions[ret] = pSession;
	
	if (_listener)
		_listener->onAgentOpen(_kernel, ret);

	return ret;
}

int32 Client::onRecv(int64 id, const char* pContext, const int32 size){
	SLASSERT(_clientSessions.find(id) != _clientSessions.end(), "where is agent %lld", id);
	if (_listener == nullptr)
		return 0;

	return _listener->onAgentRecv(_kernel, id, pContext, size);
}

void Client::onClose(int64 id){
	SLASSERT(_clientSessions.find(id) != _clientSessions.end(), "where is agent %lld", id);
	_clientSessions.erase(id);
	
	if (_listener)
		_listener->onAgentClose(_kernel, id);
}

void Client::setListener(IClientListener* pListener){
	_listener = pListener;
}

void Client::connect(const char* ip, const int32 port){
	ClientSession* pSession = (ClientSession*)_sessionPool->mallocTcpSession(_kernel);
	sl::api::IKernel* pKernel = _kernel;
	pSession->setConnect(ip, port);
	if (!pKernel->startTcpClient(pSession, ip, port, _clientSendSize, _clientRecvSize)){
		START_TIMER(pSession, 0, TIMER_BEAT_FOREVER, RECONNECT_INTERVAL);
		ERROR_LOG("connect [%s:%d] failed!", ip, port);
	}
	else{
		TRACE_LOG("connect [%s:%d] success!", ip, port);
	}
}

void Client::close(int64 id){
	auto itor = _clientSessions.find(id);
	if (itor == _clientSessions.end()){
		SLASSERT(false, "where is agent %lld", id);
		return;
	}

	itor->second->close();
}

void Client::send(const int64 id, const void* pBuf, const int32 size){
	auto itor = _clientSessions.find(id);
	if (itor == _clientSessions.end()){
		SLASSERT(false, "where is agent %lld", id);
		return;
	}

	itor->second->send(pBuf, size);
}