#include "slpacket_sender.h"
#include "slchannel.h"
#include "slevent_dispatcher.h"
#include "slnetwork_interface.h"
#include "slevent_poller.h"
#include "slendpoint.h"

namespace sl{
namespace  network{

PacketSender::PacketSender(Channel* channel, NetworkInterface* networkInterface)
	:_channel(channel),
	 _pNetworkInterface(networkInterface)
{}

PacketSender::~PacketSender(){
	_channel = NULL;
	_pNetworkInterface = NULL;
}

int PacketSender::handleOutputNotification(int fd){
	SLASSERT(_channel != NULL, "wtf");

	if (_channel->isDestroyed())
		return -1;

	if (!_channel->isConnected()){
		int error = -1, slen = sizeof(int);
		getsockopt((int32)(*(_channel->getEndPoint())), SOL_SOCKET, SO_ERROR, (char*)&error, (socklen_t *)&slen);
		if (error == 0){
			if (_channel->sendBufEmpty()){
				_pNetworkInterface->getDispatcher().deregisterWriteFileDescriptor((int32)(*(_channel->getEndPoint())));
			}
			ISLSession* poSession = _channel->getSession();
			poSession->onEstablish();
			_channel->setConnected();
		}
		else{
			SLASSERT(false, "wtf");
			ISLSession* poSession = _channel->getSession();
			poSession->onTerminate();
			_channel->destroy();
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
#else
   err = errno;
   switch (err){
   case ECONNREFUSED:	reason = REASON_NO_SUCH_PORT; break;
   case EAGAIN:		reason = REASON_RESOURCE_UNAVAILABLE; break;
   case EPIPE:			reason = REASON_CLIENT_DISCONNECTED; break;
   case ECONNRESET:	reason = REASON_CLIENT_DISCONNECTED; break;
   case ENOBUFS:		reason = REASON_TRANSMIT_QUEUE_FULL; break;
   default:			reason = REASON_GENERAL_NETWORK; break;
   }
#endif

	return reason;
}
}
}
