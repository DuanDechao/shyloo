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

PacketReceiver::~PacketReceiver()
{
	m_pEndPoint = NULL;
	m_pNetworkInterface = NULL;
}

int PacketReceiver::handleInputNotification(int fd)
{
	Channel *activeChannel = getChannel();
	SLASSERT(activeChannel != NULL, "wtf");

	if(activeChannel->isCondemn())
	{
		return -1;
	}

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
	if(pChannel)
	{
		pChannel->onPacketReceived((int)pPacket->length());
	}
	return processRecievePacket(pChannel, pPacket);
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