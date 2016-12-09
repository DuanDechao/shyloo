#ifndef _SL_NETWORKTCPPACKET_SENDER_H_
#define _SL_NETWORKTCPPACKET_SENDER_H_
#include "slobjectpool.h"
#include "slpacket_sender.h"
#include "slnetbase.h"
namespace sl
{
namespace network
{
class EndPoint;
class Channel;
class Address;
class NetworkInterface;
class EventDispatcher;

class TCPPacketSender: public PacketSender
{
public:
	typedef SLShared_ptr<SmartPoolObject<TCPPacketSender>> SmartPoolObjectPtr;
	static SmartPoolObjectPtr createSmartPoolObj();
	static CObjectPool<TCPPacketSender>& objPool();
	static TCPPacketSender* createPoolObject();
	static void reclaimPoolObject(TCPPacketSender* obj);
	static void destroyObjPool();

	TCPPacketSender():PacketSender(){}
	TCPPacketSender(EndPoint& endpoint, NetworkInterface& networkInferface);
	~TCPPacketSender();

	virtual void onGetError(Channel* pChannel);
	virtual bool processSend(Channel* pChannel);

protected:
	virtual Reason processSendPacket(Channel* pChannel, Packet* pPacket);
};
}
}
#endif