#include "sltcp_packet_sender.h"
#include "slchannel.h"
namespace sl{
namespace network{

SLPool<TCPPacketSender> TCPPacketSender::s_pool;
TCPPacketSender::TCPPacketSender(Channel* channel, NetworkInterface* networkInferface)
	:PacketSender(channel, networkInferface)
{}

TCPPacketSender::~TCPPacketSender()
{}

void TCPPacketSender::onGetError(Channel* pChannel){
	pChannel->destroy();
}

bool TCPPacketSender::processSend(Channel* pChannel){
	bool noticed = pChannel == NULL;

	//如果由poller通知的，我們需要通過地址找到channel
	if(noticed){
		pChannel = _channel;
	}

	SLASSERT(pChannel != NULL, "wtf");

	if (pChannel->isDestroyed()){
		return false;
	}

	if(!pChannel->isConnected()){
		return false;
	}

	Reason reason = REASON_SUCCESS;
	reason = processPacket(pChannel);
	if (reason != REASON_SUCCESS){
		onGetError(pChannel);
		return false;
	}

	if(noticed)
		pChannel->onSendCompleted();

	return true;

}

Reason TCPPacketSender::processSendPacket(Channel* pChannel){
	if(pChannel->isDestroyed()){
		return REASON_CHANNEL_DESTROYED;
	}

	EndPoint* pEndPoint = pChannel->getEndPoint();
	int32 sentLen = pChannel->sendToEndPoint();

	pChannel->onPacketSent(sentLen, true);

	return REASON_SUCCESS;

	//return checkSocketErrors(pEndPoint);
}


}
}
