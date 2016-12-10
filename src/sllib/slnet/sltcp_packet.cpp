#include "sltcp_packet.h"
#include "slendpoint.h"
#include "sladdress.h"
namespace sl
{
namespace network
{
TCPPacket::TCPPacket(size_t res /* = 0 */)
	:Packet(true, res)
{
	data_resize(maxBufferSize());
	wpos(0);
}

TCPPacket::~TCPPacket()
{
	data_resize(maxBufferSize());
}

size_t TCPPacket::maxBufferSize()
{
	return PACKET_MAX_SIZE_TCP;
}

int TCPPacket::recvFromEndPoint(EndPoint& ep, Address* pAddr /* = NULL */)
{
	SLASSERT(maxBufferSize() > wpos(), "wtf");

	int len = ep.recv(data() + wpos(), (int)(size() - wpos()));

	if(len > 0)
		wpos((int)wpos() + len);

	return len;

}

}
}