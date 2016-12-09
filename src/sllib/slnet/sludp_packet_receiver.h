#ifndef _SL_NETWORKUDPPACKET_RECEIVER_H_
#define _SL_NETWORKUDPPACKET_RECEIVER_H_
#include "sltimer.h"
#include "slobjectpool.h"
#include "slnetbase.h"
#include "slinterfaces.h"
#include "slpacket_receiver.h"
#include "sludp_packet.h"

namespace sl
{
namespace network
{
class Channel;
class Address;
class NetworkInterface;
class EventDispatcher;

class UDPPacketReceiver: public PacketReceiver
{
public:
	typedef SLShared_ptr<SmartPoolObject<UDPPacketReceiver>> SmartPoolObjectPtr;
	static SmartPoolObjectPtr createSmartPoolObj();
	static CObjectPool<UDPPacketReceiver>& ObjPool();
	static UDPPacketReceiver* createPoolObject();
	static void reclaimPoolObject(UDPPacketReceiver* obj);
	static void destroyObjPool();

	UDPPacketReceiver():PacketReceiver(){}
	UDPPacketReceiver(EndPoint& endpoint, NetworkInterface& networkInterface);
	~UDPPacketReceiver();

	Reason processRecievePacket(Channel* pChannel, Packet* pPacket);

	virtual PacketReceiver::PACKET_RECEIVER_TYPE type() const
	{
		return UDP_PACKET_RECEIVER;
	}

protected:
	bool processRecv(bool expectingPacket);
	PacketReceiver::RecvState checkSocketErrors(int len, bool expectingPacket);
};

}
}
#endif