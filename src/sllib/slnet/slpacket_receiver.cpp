#include "slpacket_receiver.h"
#include "slchannel.h"
#include "slnetwork_interface.h"
namespace sl{
namespace network{

PacketReceiver::PacketReceiver(EndPoint* endpoint, NetworkInterface* networkInterface)
	:_pEndPoint(endpoint),
	 _pNetworkInterface(networkInterface)
{}

PacketReceiver::~PacketReceiver(){
	_pEndPoint = NULL;
	_pNetworkInterface = NULL;
}

int PacketReceiver::handleInputNotification(int fd){
	Channel *activeChannel = getChannel();
	SLASSERT(activeChannel != NULL, "wtf");

	if(activeChannel->isCondemn()){
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

Channel* PacketReceiver::getChannel(){
	return _pNetworkInterface->findChannel(_pEndPoint->addr());
}

}
}