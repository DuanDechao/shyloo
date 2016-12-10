#ifndef _SL_SOCKETTCPPACKET_H_
#define _SL_SOCKETTCPPACKET_H_
#include "slpacket.h"

namespace sl
{
namespace network
{
class EndPoint;
class Address;
class TCPPacket: public Packet
{
public:

	static size_t maxBufferSize();

	TCPPacket(size_t res = 0);
	virtual ~TCPPacket(void);

	int recvFromEndPoint(EndPoint& ep, Address* pAddr = NULL);

};
CREATE_OBJECT_POOL(TCPPacket);

}
}
#endif