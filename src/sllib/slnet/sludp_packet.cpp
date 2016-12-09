#include "sludp_packet.h"
#include "slendpoint.h"
#include "sladdress.h"
namespace sl
{
namespace network
{
static CObjectPool<UDPPacket> g_objPool("UDPPacket");
CObjectPool<UDPPacket>& UDPPacket::ObjPool()
{
	return g_objPool;
}

UDPPacket* UDPPacket::createPoolObject()
{
	return g_objPool.FetchObj();
}

void UDPPacket::reclaimPoolObject(UDPPacket* obj)
{
	g_objPool.ReleaseObj(obj);
}

void UDPPacket::destroyObjPool()
{
	g_objPool.Destroy();
}

UDPPacket::SmartPoolObjectPtr UDPPacket::createSmartPoolObj()
{
	return SmartPoolObjectPtr(new SmartPoolObject<UDPPacket>(ObjPool().FetchObj(), g_objPool));
}

UDPPacket::UDPPacket(size_t res /* = 0 */)
	:Packet(false, res)
{
	data_resize(maxBufferSize());
	wpos(0);
}

UDPPacket::~UDPPacket(){}

size_t UDPPacket::maxBufferSize()
{
	return PACKET_MAX_SIZE_UDP;
}

void UDPPacket::onReclaimObject()
{
	Packet::onReclaimObject();
	data_resize(maxBufferSize());
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