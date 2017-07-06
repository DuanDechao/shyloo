#include "sllistener_receiver.h"
#include "sladdress.h"
#include "slendpoint.h"
#include "slevent_dispatcher.h"
#include "slnetwork_interface.h"
#include "slpacket_receiver.h"

namespace sl{
namespace network{

ListenerReceiver::ListenerReceiver(EndPoint* endpoint, NetworkInterface* networkInterface)
	:_endpoint(endpoint),
	 _networkInterface(networkInterface),
	 _pSessionFactory(nullptr),
	 _pPacketParser(nullptr),
	 _recvSize(0),
	 _sendSize(0)
{}

ListenerReceiver::~ListenerReceiver(){
	_endpoint = nullptr;
	_networkInterface = nullptr;
	_pSessionFactory = nullptr;
	_pPacketParser = nullptr;
	_sendSize = 0;
	_recvSize = 0;
}

int ListenerReceiver::handleInputNotification(int fd){
	int tickcount = 0;
	while(tickcount++ <256){
		EndPoint* pNewEndPoint = _endpoint->accept();
		if(pNewEndPoint == NULL){
			break;
		}
		else{
			Channel* pChannel = Channel::create(_networkInterface, pNewEndPoint, _pPacketParser, _recvSize, _sendSize);
			if (!pChannel){
				pChannel->destroy();
				pChannel->release();
				return 0;
			}

			if(!_networkInterface->registerChannel(pChannel)){
				pChannel->destroy();
				pChannel->release();
			}

			//
			if(_pSessionFactory == NULL){
				SLASSERT(false, "wtf");
				ECHO_ERROR("network inferface have no sessionfactory");
				return -1;
			}

			ISLSession* poSession = _pSessionFactory->createSession(pChannel);
			if(NULL == poSession){
				ECHO_ERROR("create session failed");
				pChannel->destroy();
				pChannel->release();
				return -2;
			}
			if(!pChannel->isConnected()){
				pChannel->setSession(poSession);
				poSession->setChannel(pChannel);
				poSession->onEstablish();
				pChannel->setConnected();
			}
		}
	}
	return 0;
}
void ListenerReceiver::setSessionFactory(ISLSessionFactory* poSessionFactory){
	_pSessionFactory = poSessionFactory;
}

void ListenerReceiver::setPacketParser(ISLPacketParser* poPacketParser){
	_pPacketParser = poPacketParser;
}

}
}
