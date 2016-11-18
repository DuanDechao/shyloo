#ifndef _SL_SOCKETTCPPACKET_H_
#define _SL_SOCKETTCPPACKET_H_
#include "slpacket.h"
#include "../common/slobjectpool.h"

namespace sl
{
namespace network
{
class EndPoint;
class Address;
class TCPPacket: public Packet
{
public:
	typedef SLShared_ptr<SmartPoolObject<TCPPacket>> SmartPoolObjectPtr;
	static SmartPoolObjectPtr createSmartPoolObj();
	static CObjectPool<TCPPacket>& ObjPool();
	static TCPPacket* createPoolObject();
	static void reclaimPoolObject(TCPPacket* obj);
	static void destroyObjPool();

	static size_t maxBufferSize();

	TCPPacket(MessageID msgID = 0, size_t res = 0);
	virtual ~TCPPacket(void);

	int recvFromEndPoint(EndPoint& ep, Address* pAddr = NULL);

	virtual void onReclaimObject();
};

typedef SmartPointer<TCPPacket> TCPPacketPtr;
}
}
#endif