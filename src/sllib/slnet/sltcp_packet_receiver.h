#ifndef _SL_NETWORKTCPPACKET_RECEIVER_H_
#define _SL_NETWORKTCPPACKET_RECEIVER_H_

#include "slobjectpool.h"
#include "slpacket_receiver.h"
namespace sl
{
namespace  network
{
class EndPoint;
class Channel;
class Address;
class NetworkInterface;
class EventDispatcher;

class TCPPacketReceiver: public PacketReceiver
{
public:
	TCPPacketReceiver():PacketReceiver() {}

	TCPPacketReceiver(EndPoint& endpoint, NetworkInterface& networkInterface);
	~TCPPacketReceiver();

	Reason processRecievePacket(Channel* pChannel, Packet* pPacket);

protected:
	virtual bool processRecv(bool expectingPacket);
	PacketReceiver::RecvState checkSocketErrors(int len, bool expectingPacket);
	virtual void onGetError(Channel* pChannel);
};
CREATE_OBJECT_POOL(TCPPacketReceiver);

}
}
#endif