#include "sludp_packet.h"
#include "slendpoint.h"
#include "sladdress.h"
namespace sl
{
namespace network
{

UDPPacket::UDPPacket(size_t res /* = 0 */)
	:Packet(false, res)
{
	data_resize(maxBufferSize());
	wpos(0);
}

UDPPacket::~UDPPacket(){
	data_resize(maxBufferSize());
}

size_t UDPPacket::maxBufferSize()
{
	return PACKET_MAX_SIZE_UDP;
}

int UDPPacket::recvFromEndPoint(EndPoint& ep, Address* pAddr /* = NULL */)
{
	SLASSERT(maxBufferSize() > wpos(), "wtf");

	//当接收来的大小大于接受缓冲区的时候，recvfrom返回-1
	int len = ep.recvfrom(data() + wpos(), (int32)(size() - wpos()), 
		(uint16*)&pAddr->m_port, (uint32*)&pAddr->m_ip);

	if(len > 0)
		wpos((int32)(wpos() + len));

	return len;

}

}
}