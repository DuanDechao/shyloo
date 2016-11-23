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

UDPPacket::UDPPacket(MessageID msgID /* = 0 */, size_t res /* = 0 */)
	:Packet(msgID, false, res)
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
	SL_ASSERT(maxBufferSize() > wpos());

	//���������Ĵ�С���ڽ��ܻ�������ʱ��recvfrom����-1
	int len = ep.recvfrom(data() + wpos(), size() - wpos(), 
		(uint16*)&pAddr->m_port, (uint32*)&pAddr->m_ip);

	if(len > 0)
		wpos(wpos() + len);

	return len;

}

}
}