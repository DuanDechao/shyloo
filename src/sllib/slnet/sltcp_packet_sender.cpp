#include "sltcp_packet_sender.h"
#include "slchannel.h"
#include "slbundle.h"
#include "sltcp_packet.h"
#include "sludp_packet.h"
namespace sl
{
namespace network
{
static CObjectPool<TCPPacketSender> g_objPool("TCPPacketSender");
CObjectPool<TCPPacketSender>& TCPPacketSender::objPool()
{
	return g_objPool;
}

TCPPacketSender* TCPPacketSender::createPoolObject()
{
	return g_objPool.FetchObj();
}

void TCPPacketSender::reclaimPoolObject(TCPPacketSender* obj)
{
	g_objPool.ReleaseObj(obj);
}

void TCPPacketSender::destroyObjPool()
{
	g_objPool.Destroy();
}

TCPPacketSender::SmartPoolObjectPtr TCPPacketSender::createSmartPoolObj()
{
	return SmartPoolObjectPtr(new SmartPoolObject<TCPPacketSender>(objPool().FetchObj(), g_objPool));
}

TCPPacketSender::TCPPacketSender(EndPoint& endpoint, NetworkInterface& networkInferface)
	:PacketSender(endpoint, networkInferface)
{}

TCPPacketSender::~TCPPacketSender()
{
}

void TCPPacketSender::onGetError(Channel* pChannel)
{
	pChannel->condemn();
}

bool TCPPacketSender::processSend(Channel* pChannel)
{
	bool noticed = pChannel == NULL;

	//如果由poller通知的，我們需要通過地址找到channel
	if(noticed)
	{
		pChannel = getChannel();
	}

	SLASSERT(pChannel != NULL, "wtf");

	if(pChannel->isCondemn())
	{
		return false;
	}

	Channel::Bundles& bundles = pChannel->bundles();
	Reason reason = REASON_SUCCESS;

	Channel::Bundles::iterator iter = bundles.begin();
	for (; iter != bundles.end(); ++iter)
	{
		Bundle::Packets& packets = (*iter)->packets();
		Bundle::Packets::iterator iter1 = packets.begin();
		for (; iter1 != packets.end(); ++iter1)
		{
			reason = processPacket(pChannel, (*iter1));
			if(reason != REASON_SUCCESS)
				break;
			else
			{
				RECLAIM_PACKET((*iter)->isTCPPacket(), (*iter1));
			}
		}

		if(reason == REASON_SUCCESS)
		{
			packets.clear();
			network::Bundle::reclaimPoolObject((*iter));
		}
		else
		{
			packets.erase(packets.begin(), iter1);
			bundles.erase(bundles.begin(), iter);

			if(reason == REASON_RESOURCE_UNAVAILABLE)
			{

			}
			else
			{
				onGetError(pChannel);
			}
			return false;
		}
	}
	bundles.clear();

	if(noticed)
		pChannel->onSendCompleted();

	return true;

}

Reason TCPPacketSender::processFilterPacket(Channel* pChannel, Packet* pPacket)
{
	if(pChannel->isCondemn())
	{
		return REASON_CHANNEL_CONDEMN;
	}

	EndPoint* pEndPoint = pChannel->getEndPoint();
	int32 len = pEndPoint->send(pPacket->data() + pPacket->m_sentSize, (int32)(pPacket->length() - pPacket->m_sentSize));

	if(len > 0)
	{
		pPacket->m_sentSize += len;
	}

	bool sentCompleted = pPacket->m_sentSize == pPacket->length();

	pChannel->onPacketSent(len, sentCompleted);

	if(sentCompleted)
		return REASON_SUCCESS;

	return checkSocketErrors(pEndPoint);
}


}
}