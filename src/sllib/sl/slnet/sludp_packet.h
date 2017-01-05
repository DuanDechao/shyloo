#ifndef _SL_SOCKETUDPPACKET_H_
#define _SL_SOCKETUDPPACKET_H_
#include "slpacket.h"
namespace sl
{
namespace network
{
class EndPoint;
class Address;
class UDPPacket: public Packet
{
public:
	static uint32 maxBufferSize();

	UDPPacket(uint32 res = 0);
	virtual ~UDPPacket(void);

	int recvFromEndPoint(EndPoint& ep, Address* pAddr = NULL);
};

CREATE_OBJECT_POOL(UDPPacket);

}
}
#endif