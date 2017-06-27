#include "sludp_packet.h"
#include "slendpoint.h"
#include "sladdress.h"
namespace sl
{
namespace network
{

UDPPacket::UDPPacket(uint32 res /* = 0 */)
	:Packet(false, res)
{
	data_resize(maxBufferSize());
	wpos(0);
}

UDPPacket::~UDPPacket(){
	data_resize(maxBufferSize());
}

uint32 UDPPacket::maxBufferSize()
{
	return PACKET_MAX_SIZE_UDP;
}

int UDPPacket::recvFromEndPoint(EndPoint& ep, Address* pAddr /* = NULL */)
{
	SLASSERT((int32)maxBufferSize() > wpos(), "wtf");

	//当接收来的大小大于接受缓冲区的时候，recvfrom返回-1
	int len = ep.recvfrom(data() + wpos(), (int32)(size() - wpos()), 
		(uint16*)&pAddr->_port, (uint32*)&pAddr->_ip);

	if(len > 0)
		wpos((int32)(wpos() + len));

	return len;

}

}
}