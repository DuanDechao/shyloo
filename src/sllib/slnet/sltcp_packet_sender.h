#ifndef _SL_LIB_NET_TCPPACKET_SENDER_H_
#define _SL_LIB_NET_TCPPACKET_SENDER_H_
#include "slpacket_sender.h"
#include "slnetbase.h"
#include "slpool.h"

namespace sl{
namespace network{

class Channel;
class NetworkInterface;
class EventDispatcher;
class TCPPacketSender: public PacketSender{
public:
	inline static TCPPacketSender* create(Channel* channel, NetworkInterface* networkInferface){
		return CREATE_FROM_POOL(s_pool, channel, networkInferface);
	}

	virtual void release(){
		s_pool.recover(this);
	}

	virtual void onGetError(Channel* pChannel);
	virtual bool processSend(Channel* pChannel);

protected:
	friend SLPool<TCPPacketSender>;
	TCPPacketSender(Channel* channel, NetworkInterface* networkInferface);
	~TCPPacketSender();

	virtual Reason processSendPacket(Channel* pChannel);

private:
	static SLPool<TCPPacketSender> s_pool;
};
}
}
#endif