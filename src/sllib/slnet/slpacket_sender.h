#ifndef _SL_LIB_NET_PACKET_SENDER_H_
#define _SL_LIB_NET_PACKET_SENDER_H_
#include "slnetbase.h"
#include "slobjectpool.h"
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
	PacketSender(EndPoint* endpoint, NetworkInterface* networkInterface);
	virtual ~PacketSender();

	EventDispatcher& dispatcher();

	inline void SetEndPoint(EndPoint* pEndPoint){ _pEndPoint = pEndPoint;}
	inline EndPoint* GetEndPoint() const {return _pEndPoint;}

	virtual int handleOutputNotification(int fd);
	virtual Reason processPacket(Channel* pChannel);
	virtual Reason processSendPacket(Channel* pChannel) = 0;
	virtual Channel* getChannel();
	virtual bool processSend(Channel* pChannel) = 0;

	static Reason checkSocketErrors(const EndPoint* pEndPoint);

protected:
	EndPoint*				_pEndPoint;
	NetworkInterface*		_pNetworkInterface;
};
}
}
#endif