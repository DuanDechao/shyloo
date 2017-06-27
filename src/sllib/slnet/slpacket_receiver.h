#ifndef _SL_LIB_NET_PACKET_RECEIVER_H_
#define _SL_LIB_NET_PACKET_RECEIVER_H_
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
class PacketReceiver: public InputNotificationHandler{
public:
	enum RecvState{
		RECV_STATE_INTERRUPT = -1,
		RECV_STATE_BREAK= 0,
		RECV_STATE_CONTINUE = 1
	};

	enum PACKET_RECEIVER_TYPE{
		TCP_PACKET_RECEIVER = 0,
		UDP_PACKET_RECEIVER = 1
	};

public:
	PacketReceiver(EndPoint* endpoint, NetworkInterface* networkInterface);
	virtual ~PacketReceiver();

	virtual Reason processPacket(Channel* pChannel, int32 packetLen);
	virtual Reason processRecievePacket(Channel* pChannel) = 0;
	EventDispatcher& dispatcher();

	virtual PacketReceiver::PACKET_RECEIVER_TYPE type() const{
		return TCP_PACKET_RECEIVER;
	}

	inline void SetEndPoint(EndPoint* pEndPoint){ _pEndPoint = pEndPoint;}
	inline EndPoint* GetEndPoint() const {return _pEndPoint;}

	virtual int handleInputNotification(int fd);
	virtual Channel* getChannel();

protected:
	virtual bool processRecv(bool expectingPacket) = 0;
	virtual RecvState checkSocketErrors(int len, bool expectingPacket) = 0;

protected:
	EndPoint*			_pEndPoint;
	NetworkInterface*	_pNetworkInterface;

};
}
}
#endif