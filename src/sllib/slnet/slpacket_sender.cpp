#include "slpacket_sender.h"
#include "sladdress.h"
#include "slbundle.h"
#include "slchannel.h"
#include "slendpoint.h"
#include "slevent_dispatcher.h"
#include "slnetwork_interface.h"
#include "slevent_poller.h"

namespace sl
{
namespace  network
{
PacketSender::PacketSender()
	:m_pEndPoint(NULL),
	 m_pNetworkInterface(NULL)
{}

PacketSender::PacketSender(EndPoint& endpoint, NetworkInterface& networkInterface)
	:m_pEndPoint(&endpoint),
	 m_pNetworkInterface(&networkInterface)
{}

PacketSender::~PacketSender()
{
	m_pEndPoint = NULL;
	m_pNetworkInterface = NULL;
}

Channel* PacketSender::getChannel()
{
	return m_pNetworkInterface->findChannel(m_pEndPoint->addr());
}

int PacketSender::handleOutputNotification(int fd)
{
	Channel *activeChannel = getChannel();
	SLASSERT(activeChannel != NULL, "wtf");

	if(activeChannel->isCondemn())
	{
		return -1;
	}

	if(!activeChannel->isConnected()){
		ISLSession* poSession = activeChannel->getSession();
		poSession->onEstablish();
		activeChannel->setConnected();
		return 0;
	}

	processSend(NULL);
	return 0;
}

Reason PacketSender::processPacket(Channel* pChannel, Packet* pPacket)
{
	if(pChannel != NULL)
	{
		pChannel->onPacketSent((int32)pPacket->length(), false);
	}

	return this->processSendPacket(pChannel, pPacket);
}

EventDispatcher& PacketSender::dispatcher()
{
	return m_pNetworkInterface->getDispatcher();
}

Reason PacketSender::checkSocketErrors(const EndPoint* pEndPoint)
{
	int err;
	Reason reason;

#ifdef SL_OS_WINDOWS
	err = WSAGetLastError();
	if(err == WSAEWOULDBLOCK || err == WSAEINTR)
	{
		reason = REASON_RESOURCE_UNAVAILABLE;
	}
	else
	{
		switch (err)
		{
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