#include "slpacket_sender.h"
#include "sladdress.h"
#include "slbundle.h"
#include "slchannel.h"
#include "slendpoint.h"
#include "slevent_dispatcher.h"
#include "slnetwork_interface.h"
#include "slevent_poller.h"

namespace sl{
namespace  network{

PacketSender::PacketSender(EndPoint* endpoint, NetworkInterface* networkInterface)
	:_pEndPoint(endpoint),
	 _pNetworkInterface(networkInterface)
{}

PacketSender::~PacketSender(){
	_pEndPoint = NULL;
	_pNetworkInterface = NULL;
}

Channel* PacketSender::getChannel(){
	return _pNetworkInterface->findChannel(_pEndPoint->addr());
}

int PacketSender::handleOutputNotification(int fd){
	Channel *activeChannel = getChannel();
	SLASSERT(activeChannel != NULL, "wtf");

	if(activeChannel->isCondemn())
		return -1;

	if(!activeChannel->isConnected()){
		int error = -1, slen = sizeof(int);
		getsockopt((int32)*_pEndPoint, SOL_SOCKET, SO_ERROR, (char*)&error, (socklen_t *)&slen);
		if (error == 0){
			if (activeChannel->sendBufEmpty()){
				_pNetworkInterface->getDispatcher().deregisterWriteFileDescriptor((int32)*_pEndPoint);
			}
			ISLSession* poSession = activeChannel->getSession();
			poSession->onEstablish();
			activeChannel->setConnected();
			
		}
		else{
			SLASSERT(false, "wtf");
			activeChannel->destroy();
			return -2;
		}
		return 0;
	}

	processSend(NULL);
	return 0;
}

Reason PacketSender::processPacket(Channel* pChannel){
	return this->processSendPacket(pChannel);
}

EventDispatcher& PacketSender::dispatcher(){
	return _pNetworkInterface->getDispatcher();
}

Reason PacketSender::checkSocketErrors(const EndPoint* pEndPoint){
	int err;
	Reason reason;

#ifdef SL_OS_WINDOWS
	err = WSAGetLastError();
	if(err == WSAEWOULDBLOCK || err == WSAEINTR){
		reason = REASON_RESOURCE_UNAVAILABLE;
	}
	else{
		switch (err){
		case WSAECONNREFUSED: reason = REASON_NO_SUCH_PORT; break;
		case WSAECONNRESET: reason = REASON_CLIENT_DISCONNECTED; break;
		case WSAECONNABORTED: reason = REASON_CLIENT_DISCONNECTED; break;
		default: reason = REASON_GENERAL_NETWORK; break;
		}
	}
#endif
	return reason;
}
}
}