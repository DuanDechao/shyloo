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
	typedef SLShared_ptr<SmartPoolObject<TCPPacketReceiver>> SmartPoolObjectPtr;

	static SmartPoolObjectPtr createSmartPoolObj();
	static CObjectPool<TCPPacketReceiver>& ObjPool();
	static TCPPacketReceiver* createPoolObject();
	static void reclaimPoolObject(TCPPacketReceiver* obj);
	static void destroyObjPool();

	TCPPacketReceiver():PacketReceiver() {}

	TCPPacketReceiver(EndPoint& endpoint, NetworkInterface& networkInterface);
	~TCPPacketReceiver();

	Reason processFilteredPacket(Channel* pChannel, Packet* pPacket);

protected:
	virtual bool processRecv(bool expectingPacket);
	PacketReceiver::RecvState checkSocketErrors(int len, bool expectingPacket);
	virtual void onGetError(Channel* pChannel);
};
}
}
#endif