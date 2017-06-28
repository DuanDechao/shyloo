#ifndef _SL_LIB_NET_UDPPACKET_RECEIVER_H_
#define _SL_LIB_NET_UDPPACKET_RECEIVER_H_
#include "sltimer.h"
#include "slnetbase.h"
#include "slinterfaces.h"
#include "slpacket_receiver.h"
#include "slpool.h"

namespace sl{
namespace network{
class Channel;
class Address;
class NetworkInterface;
class EventDispatcher;

class UDPPacketReceiver: public PacketReceiver{
public:
	
	inline static UDPPacketReceiver* create(Channel* channel, NetworkInterface* networkInterface){
		return CREATE_FROM_POOL(s_pool, channel, networkInterface);
	}

	virtual void release(){
		s_pool.recover(this);
	}

	Reason processRecievePacket(Channel* pChannel);

	virtual PacketReceiver::PACKET_RECEIVER_TYPE type() const{
		return UDP_PACKET_RECEIVER;
	}

protected:
	friend SLPool<UDPPacketReceiver>;
	UDPPacketReceiver(Channel* channel, NetworkInterface* networkInterface);
	~UDPPacketReceiver();

	bool processRecv(bool expectingPacket);
	PacketReceiver::RecvState checkSocketErrors(int len, bool expectingPacket);

private:
	static SLPool<UDPPacketReceiver> s_pool;
};

}
}
#endif