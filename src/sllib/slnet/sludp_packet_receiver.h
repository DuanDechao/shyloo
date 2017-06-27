#ifndef _SL_NETWORKUDPPACKET_RECEIVER_H_
#define _SL_NETWORKUDPPACKET_RECEIVER_H_
#include "sltimer.h"
#include "slobjectpool.h"
#include "slnetbase.h"
#include "slinterfaces.h"
#include "slpacket_receiver.h"

namespace sl
{
namespace network
{
class Channel;
class Address;
class NetworkInterface;
class EventDispatcher;

class UDPPacketReceiver: public PacketReceiver{
public:
	UDPPacketReceiver(EndPoint* endpoint, NetworkInterface* networkInterface);
	~UDPPacketReceiver();

	Reason processRecievePacket(Channel* pChannel);

	virtual PacketReceiver::PACKET_RECEIVER_TYPE type() const{
		return UDP_PACKET_RECEIVER;
	}

protected:
	bool processRecv(bool expectingPacket);
	PacketReceiver::RecvState checkSocketErrors(int len, bool expectingPacket);
};

CREATE_OBJECT_POOL(UDPPacketReceiver);
}
}
#endif