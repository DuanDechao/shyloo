#include "slpacket_receiver.h"
#include "slpacket.h"
#include "slchannel.h"
#include "slnetwork_interface.h"
namespace sl
{
namespace network
{
PacketReceiver::PacketReceiver()
	:m_pEndPoint(NULL),
	 m_pNetworkInterface(NULL)
{}

PacketReceiver::PacketReceiver(EndPoint& endpoint, NetworkInterface& networkInterface)
	:m_pEndPoint(&endpoint),
	 m_pNetworkInterface(&networkInterface)
{}

PacketReceiver::~PacketReceiver(){}

int PacketReceiver::handleInputNotification(int fd)
{
	if(this->processRecv(true))
	{
		while(this->processRecv(false))
		{

		}
	}
	return 0;
}

Reason PacketReceiver::processPacket(Channel* pChannel, Packet* pPacket)
{
	if(pChannel != NULL)
	{
		pChannel->onPacketReceived((int)pPacket->length());

		if(pChannel->getFilter())
		{
			return pChannel->getFilter()->recv(pChannel, *this, pPacket);
		}
	}

	return this->processFilteredPacket(pChannel, pPacket);
}

EventDispatcher& PacketReceiver::dispatcher()
{
	return this->m_pNetworkInterface->getDispatcher();
}

Channel* PacketReceiver::getChannel()
{
	return m_pNetworkInterface->findChannel(m_pEndPoint->addr());
}

}
}