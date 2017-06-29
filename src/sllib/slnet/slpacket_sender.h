#ifndef _SL_LIB_NET_PACKET_SENDER_H_
#define _SL_LIB_NET_PACKET_SENDER_H_
#include "slnetbase.h"
#include "slinterfaces.h"

namespace sl{
namespace network{

class EndPoint;
class Channel;
class Address;
class NetworkInterface;
class EventDispatcher;
class PacketSender: public OutputNotificationHandler{
public:
	PacketSender(Channel* channel, NetworkInterface* networkInterface);
	virtual ~PacketSender();

	EventDispatcher& dispatcher();

	virtual int handleOutputNotification(int fd);
	virtual Reason processPacket(Channel* pChannel);

	virtual Reason processSendPacket(Channel* pChannel) = 0;
	virtual bool processSend(Channel* pChannel) = 0;
	virtual void release() = 0;

	static Reason checkSocketErrors(const EndPoint* pEndPoint);

protected:
	Channel*				_channel;
	NetworkInterface*		_pNetworkInterface;
};
}
}
#endif