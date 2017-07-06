#include "sllistener.h"
#include "slnet_module.h"
#include "sllistener_receiver.h"
namespace sl{
namespace network{
CSLListener::CSLListener()
	:_pNetworkInterface(nullptr),
	_pListenerReceiver(nullptr),
	_dwRecvBufSize(0),
    _dwSendBufSize(0)
{
	_pListenEndPoint = EndPoint::create(0, 0);
	_pNetworkInterface = CSLNetModule::getSingletonPtr()->getNetworkInterface();
	_pListenerReceiver = NEW ListenerReceiver(_pListenEndPoint, _pNetworkInterface);
}

CSLListener::~CSLListener(){
	stop();

	if (nullptr != _pListenerReceiver)
		DEL _pListenerReceiver;
	_pListenerReceiver = nullptr;

	if (_pListenEndPoint)
		_pListenEndPoint->release();
	_pListenEndPoint = nullptr;

	_dwRecvBufSize = 0;
	_dwSendBufSize = 0;
}

void CSLListener::setSessionFactory(ISLSessionFactory* poSessionFactory){
	_pListenerReceiver->setSessionFactory(poSessionFactory);
}

void CSLListener::setBufferSize(uint32 dwRecvBufSize, uint32 dwSendBufSize){
	_dwRecvBufSize = dwRecvBufSize;
	_dwSendBufSize = dwSendBufSize;
	_pListenerReceiver->setBufferSize(_dwRecvBufSize, _dwSendBufSize);
}

bool CSLListener::start(const char* pszIP, uint16 wPort, bool bReUseAddr /* = true */){
	SLASSERT(_pNetworkInterface, "wtf");
	if (_pNetworkInterface == nullptr || _pListenerReceiver == nullptr ||
		_dwRecvBufSize <= 0 || _dwSendBufSize <= 0){
		return false;
	}

	return _pNetworkInterface->createListeningSocket(pszIP, wPort, _pListenEndPoint, _pListenerReceiver,
		_dwRecvBufSize, _dwSendBufSize);
}

bool CSLListener::stop(){
	if (_pListenEndPoint->good()){
		_pNetworkInterface->deregisterSocket((int32)*_pListenEndPoint);
		_pListenEndPoint->closeEndPoint();
	}
	return true;
}

void CSLListener::setPacketParser(ISLPacketParser* poPacketParser){
	_pListenerReceiver->setPacketParser(poPacketParser);
}

void CSLListener::release(){
	DEL this;
}
}
}
