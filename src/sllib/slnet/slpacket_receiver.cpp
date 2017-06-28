#include "slpacket_receiver.h"
#include "slchannel.h"
#include "slnetwork_interface.h"
namespace sl{
namespace network{

PacketReceiver::PacketReceiver(Channel* channel, NetworkInterface* networkInterface)
	:_channel(channel),
	 _pNetworkInterface(networkInterface)
{}

PacketReceiver::~PacketReceiver(){
	_channel = NULL;
	_pNetworkInterface = NULL;
}

int PacketReceiver::handleInputNotification(int fd){
	SLASSERT(_channel != NULL, "wtf");

	if (_channel->isDestroyed()){
		return -1;
	}

	if(this->processRecv(true)){
		while(this->processRecv(false)){
		}
	}
	return 0;
}

Reason PacketReceiver::processPacket(Channel* pChannel, int32 packetLen){
	if(pChannel){
		pChannel->onPacketReceived(packetLen);
	}
	return processRecievePacket(pChannel);
}

EventDispatcher& PacketReceiver::dispatcher(){
	return this->_pNetworkInterface->getDispatcher();
}

}
}