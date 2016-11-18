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
		return rstate = PacketReceiver::RECV_STATE_CONTINUE;
	}

	Channel* pSrcChannel = m_pNetworkInterface->findChannel(srcAddr);
	if(pSrcChannel == NULL)
	{
		EndPoint* pNewEndPoint = EndPoint::createPoolObject();
		pNewEndPoint->addr(srcAddr.m_port, srcAddr.m_ip);

		pSrcChannel = network::Channel::createPoolObject();
		bool ret = pSrcChannel->initialize(*m_pNetworkInterface, pNewEndPoint, Channel::EXTERNAL, PROTOCOL_UDP);
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
	SL_ASSERT(pSrcChannel != NULL);

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



}

}