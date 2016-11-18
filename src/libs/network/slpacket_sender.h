#ifndef _SL_PACKET_SENDER_H_
#define _SL_PACKET_SENDER_H_
#include "slcommon.h"
#include "../common/slobjectpool.h"
#include "slinterfaces.h"
namespace sl
{
namespace network
{
class Packet;
class EndPoint;
class Channel;
class Address;
class NetworkInterface;
class EventDispatcher;

class PacketSender: public PoolObject, public OutputNotificationHandler
{
public:
	PacketSender();
	PacketSender(EndPoint& endpoint, NetworkInterface& networkInterface);
	virtual ~PacketSender();

	EventDispatcher& dispatcher();

	void onReclaimObject()
	{
		m_pEndPoint = NULL;
		m_pNetworkInterface = NULL;
	}

	void SetEndPoint(EndPoint* pEndPoint)
	{
		m_pEndPoint = pEndPoint;
	}

	EndPoint* GetEndPoint() const {return m_pEndPoint;}

	virtual int handleOutputNotification(int fd);

	virtual Reason processPacket(Channel* pChannel, Packet* pPacket);
	virtual Reason processFilterPacket(Channel* pChannel, Packet* pPacket) = 0;

	static Reason checkSocketErrors(const EndPoint* pEndPoint);

	virtual Channel* getChannel();

	virtual bool processSend(Channel* pChannel) = 0;


protected:
	EndPoint*				m_pEndPoint;
	NetworkInterface*		m_pNetworkInterface;
};
}
}
#endif