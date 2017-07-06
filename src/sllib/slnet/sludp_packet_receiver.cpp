#include "sludp_packet_receiver.h"
#include "sladdress.h"
#include "slchannel.h"
#include "slendpoint.h"
#include "slevent_dispatcher.h"
#include "slnetwork_interface.h"
#include "slevent_poller.h"
namespace sl{
namespace network{

SLPool<UDPPacketReceiver> UDPPacketReceiver::s_pool;
UDPPacketReceiver::UDPPacketReceiver(Channel* channel, NetworkInterface* networkInterface)
	:PacketReceiver(channel, networkInterface)
{}

UDPPacketReceiver::~UDPPacketReceiver(){}

bool UDPPacketReceiver::processRecv(bool expectingPacket){
	/*Address srcAddr;
	UDPPacket* pChannelReceiveWindow = CREATE_POOL_OBJECT(UDPPacket);
	int len = pChannelReceiveWindow->recvFromEndPoint(*m_pEndPoint, &srcAddr);

	if(len <= 0)
	{
	RELEASE_POOL_OBJECT(UDPPacket, pChannelReceiveWindow);
	PacketReceiver::RecvState rstate = this->checkSocketErrors(len, expectingPacket);
	return rstate == PacketReceiver::RECV_STATE_CONTINUE;
	}

	Channel* pSrcChannel = m_pNetworkInterface->findChannel(srcAddr);
	if(pSrcChannel == NULL)
	{
	EndPoint* pNewEndPoint = CREATE_POOL_OBJECT(EndPoint);

	pNewEndPoint->addr(srcAddr._port, srcAddr._ip);

	pSrcChannel = Channel::create(m_pNetworkInterface, pNewEndPoint, nullptr, PROTOCOL_UDP);
	if (!pSrcChannel)
	{
	pSrcChannel->destroy();
	pSrcChannel->release();
	RELEASE_POOL_OBJECT(UDPPacket, pChannelReceiveWindow);
	return false;
	}

	if(!m_pNetworkInterface->registerChannel(pSrcChannel))
	{
	RELEASE_POOL_OBJECT(UDPPacket, pChannelReceiveWindow);
	pSrcChannel->destroy();
	pSrcChannel->release();
	return false;
	}
	}
	SLASSERT(pSrcChannel != NULL, "wtf");

	if(pSrcChannel->isCondemn())
	{
	RELEASE_POOL_OBJECT(UDPPacket, pChannelReceiveWindow);
	m_pNetworkInterface->deregisterChannel(pSrcChannel);
	pSrcChannel->destroy();
	pSrcChannel->release();
	return false;
	}

	Reason ret = this->processPacket(pSrcChannel, 0);

	if(ret != REASON_SUCCESS)
	{

	}*/

	return true;
}

Reason UDPPacketReceiver::processRecievePacket(Channel* pChannel){
	// 如果为None， 则可能是被过滤器过滤掉了(过滤器正在按照自己的规则组包解密)
	/*if(pPacket)
	{
	pChannel->addReceiveWindow(pPacket);
	}
	*/
	return REASON_SUCCESS;
}

PacketReceiver::RecvState UDPPacketReceiver::checkSocketErrors(int len, bool expectingPacket){
	if (len == 0)
	{
		/*SL_ELOG(fmt::format("PacketReceiver::processPendingEvents: "
			"Throwing REASON_GENERAL_NETWORK (1)- {}\n",
			strerror( errno )));*/

		/*this->dispatcher().errorReporter().reportException(
			REASON_GENERAL_NETWORK );*/

		return RECV_STATE_CONTINUE;
	}

#ifdef _WIN32
	DWORD wsaErr = WSAGetLastError();
#endif //def _WIN32

	if (
#ifdef _WIN32
		wsaErr == WSAEWOULDBLOCK && !expectingPacket
#else
		errno == EAGAIN && !expectingPacket
#endif
		)
	{
		return RECV_STATE_BREAK;
	}

#ifdef unix
	/*if (errno == EAGAIN ||
		errno == ECONNREFUSED ||
		errno == EHOSTUNREACH)
	{
		Network::Address offender;

		if (pEndpoint_->getClosedPort(offender))
		{
			// If we got a NO_SUCH_PORT error and there is an internal
			// channel to this address, mark it as remote failed.  The logic
			// for dropping external channels that get NO_SUCH_PORT
			// exceptions is built into BaseApp::onClientNoSuchPort().
			if (errno == ECONNREFUSED)
			{
				// 未实现
			}

			this->dispatcher().errorReporter().reportException(
				REASON_NO_SUCH_PORT, offender);

			return RECV_STATE_CONTINUE;
		}
		else
		{
			WARNING_MSG("UDPPacketReceiver::processPendingEvents: "
				"getClosedPort() failed\n");
		}
	}*/
#else
/*	if (wsaErr == WSAECONNRESET)
	{
		return RECV_STATE_CONTINUE;
	}*/
#endif // unix

#ifdef _WIN32
#else
#endif
	/*this->dispatcher().errorReporter().reportException(
		REASON_GENERAL_NETWORK);*/

	return RECV_STATE_CONTINUE;
}

}

}
