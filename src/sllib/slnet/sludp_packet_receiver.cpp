#include "sludp_packet_receiver.h"

#include "sladdress.h"
#include "slbundle.h"
#include "slchannel.h"
#include "slendpoint.h"
#include "slevent_dispatcher.h"
#include "slnetwork_interface.h"
#include "slevent_poller.h"
namespace sl
{
namespace network
{
static CObjectPool<UDPPacketReceiver> g_objPool("UDPPacketReceiver");
CObjectPool<UDPPacketReceiver>& UDPPacketReceiver::ObjPool()
{
	return g_objPool;
}

UDPPacketReceiver* UDPPacketReceiver::createPoolObject()
{
	return g_objPool.FetchObj();
}

void UDPPacketReceiver::reclaimPoolObject(UDPPacketReceiver* obj)
{
	g_objPool.ReleaseObj(obj);
}

void UDPPacketReceiver::destroyObjPool()
{
	g_objPool.Destroy();
}

UDPPacketReceiver::SmartPoolObjectPtr UDPPacketReceiver::createSmartPoolObj()
{
	return SmartPoolObjectPtr(new SmartPoolObject<UDPPacketReceiver>(ObjPool().FetchObj(), g_objPool));
}

UDPPacketReceiver::UDPPacketReceiver(EndPoint& endpoint,
									 NetworkInterface& networkInterface)
									 :PacketReceiver(endpoint, networkInterface)
{}

UDPPacketReceiver::~UDPPacketReceiver(){}

bool UDPPacketReceiver::processRecv(bool expectingPacket)
{
	Address srcAddr;
	UDPPacket* pChannelReceiveWindow = UDPPacket::createPoolObject();
	int len = pChannelReceiveWindow->recvFromEndPoint(*m_pEndPoint, &srcAddr);

	if(len <= 0)
	{
		UDPPacket::reclaimPoolObject(pChannelReceiveWindow);
		PacketReceiver::RecvState rstate = this->checkSocketErrors(len, expectingPacket);
		return rstate == PacketReceiver::RECV_STATE_CONTINUE;
	}

	Channel* pSrcChannel = m_pNetworkInterface->findChannel(srcAddr);
	if(pSrcChannel == NULL)
	{
		EndPoint* pNewEndPoint = EndPoint::createPoolObject();
		pNewEndPoint->addr(srcAddr.m_port, srcAddr.m_ip);

		pSrcChannel = network::Channel::createPoolObject();
		bool ret = pSrcChannel->initialize(*m_pNetworkInterface, pNewEndPoint, nullptr, PROTOCOL_UDP);
		if(!ret)
		{
			pSrcChannel->destroy();
			Channel::reclaimPoolObject(pSrcChannel);
			UDPPacket::reclaimPoolObject(pChannelReceiveWindow);
			return false;
		}

		if(!m_pNetworkInterface->registerChannel(pSrcChannel))
		{
			UDPPacket::reclaimPoolObject(pChannelReceiveWindow);
			pSrcChannel->destroy();
			Channel::reclaimPoolObject(pSrcChannel);
			return false;
		}
	}
	SLASSERT(pSrcChannel != NULL, "wtf");

	if(pSrcChannel->isCondemn())
	{
		UDPPacket::reclaimPoolObject(pChannelReceiveWindow);
		m_pNetworkInterface->deregisterChannel(pSrcChannel);
		pSrcChannel->destroy();
		Channel::reclaimPoolObject(pSrcChannel);
		return false;
	}

	Reason ret = this->processPacket(pSrcChannel, pChannelReceiveWindow);

	if(ret != REASON_SUCCESS)
	{

	}

	return true;
}

Reason UDPPacketReceiver::processRecievePacket(Channel* pChannel, Packet * pPacket)
{
	// 如果为None， 则可能是被过滤器过滤掉了(过滤器正在按照自己的规则组包解密)
	if(pPacket)
	{
		pChannel->addReceiveWindow(pPacket);
	}

	return REASON_SUCCESS;
}

PacketReceiver::RecvState UDPPacketReceiver::checkSocketErrors(int len, bool expectingPacket)
{
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
	if (errno == EAGAIN ||
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
	}
#else
	if (wsaErr == WSAECONNRESET)
	{
		return RECV_STATE_CONTINUE;
	}
#endif // unix

#ifdef _WIN32
	/*WARNING_MSG(fmt::format("UDPPacketReceiver::processPendingEvents: "
		"Throwing REASON_GENERAL_NETWORK - {}\n",
		wsaErr));*/
#else
	WARNING_MSG(fmt::format("UDPPacketReceiver::processPendingEvents: "
		"Throwing REASON_GENERAL_NETWORK - {}\n",
		kbe_strerror()));
#endif
	/*this->dispatcher().errorReporter().reportException(
		REASON_GENERAL_NETWORK);*/

	return RECV_STATE_CONTINUE;
}

}

}