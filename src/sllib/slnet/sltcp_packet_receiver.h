#ifndef _SL_LIB_NET_TCPPACKET_RECEIVER_H_
#define _SL_LIB_NET_TCPPACKET_RECEIVER_H_

#include "slpacket_receiver.h"
#include "slpool.h"

namespace sl{
namespace  network{
class EndPoint;
class Channel;
class Address;
class NetworkInterface;
class EventDispatcher;

class TCPPacketReceiver: public PacketReceiver{
public:
	inline static TCPPacketReceiver* create(Channel* channel, NetworkInterface* networkInterface){
		return CREATE_FROM_POOL(s_pool, channel, networkInterface);
	}

	virtual void release(){
		s_pool.recover(this);
	}

	Reason processRecievePacket(Channel* pChannel);

protected:
	friend sl::SLPool<TCPPacketReceiver>;
	TCPPacketReceiver(Channel* channel, NetworkInterface* networkInterface);
	~TCPPacketReceiver();

	virtual bool processRecv(bool expectingPacket);
	PacketReceiver::RecvState checkSocketErrors(int len, bool expectingPacket);
	virtual void onGetError(Channel* pChannel);

private:
	static sl::SLPool<TCPPacketReceiver> s_pool;
};

}
}
#endif