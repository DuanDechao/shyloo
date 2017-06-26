#include "sllistener.h"
#include "slnet_module.h"
#include "sllistener_receiver.h"
namespace sl
{
CSLListener::CSLListener()
	:_dwRecvBufSize(0),
	 _dwSendBufSize(0),
	 _pListenEndPoint(),
	 _pNetworkInterface(nullptr),
	 _pListenerReceiver(nullptr)
{
	_pNetworkInterface = CSLNetModule::getSingletonPtr()->getNetworkInterface();
	_pListenerReceiver = new ListenerReceiver(&_pListenEndPoint, _pNetworkInterface);
}

CSLListener::~CSLListener(){
	if (nullptr != _pListenerReceiver)
		delete _pListenerReceiver;

	_pListenerReceiver = nullptr;

	stop();
	_dwRecvBufSize = 0;
	_dwSendBufSize = 0;
}

void CSLListener::setSessionFactory(ISLSessionFactory* poSessionFactory){
	_pListenerReceiver->setSessionFactory(poSessionFactory);
}

void CSLListener::setBufferSize(uint32 dwRecvBufSize, uint32 dwSendBufSize){
	_dwRecvBufSize = dwRecvBufSize;
	_dwSendBufSize = dwSendBufSize;
}

bool CSLListener::start(const char* pszIP, uint16 wPort, bool bReUseAddr /* = true */){
	SLASSERT(_pNetworkInterface, "wtf");
	if (_pNetworkInterface == nullptr || _pListenerReceiver == nullptr || 
		_dwRecvBufSize <= 0 || _dwSendBufSize <= 0){
		return false;
	}

	return _pNetworkInterface->createListeningSocket(pszIP, wPort, &_pListenEndPoint, _pListenerReceiver,
		_dwRecvBufSize, _dwSendBufSize);
}

bool CSLListener::stop(){
	if (_pListenEndPoint.good()){
		_pNetworkInterface->deregisterSocket((int32)_pListenEndPoint);
		_pListenEndPoint.close();
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