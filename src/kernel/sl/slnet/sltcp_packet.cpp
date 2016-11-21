#include "sltcp_packet.h"
#include "slendpoint.h"
#include "sladdress.h"
namespace sl
{
	namespace network
	{
		static CObjectPool<TCPPacket> g_objPool("TCPPacket");
		CObjectPool<TCPPacket>& TCPPacket::ObjPool()
		{
			return g_objPool;
		}

		TCPPacket* TCPPacket::createPoolObject()
		{
			return g_objPool.FetchObj();
		}

		void TCPPacket::reclaimPoolObject(TCPPacket* obj)
		{
			g_objPool.ReleaseObj(obj);
		}

		void TCPPacket::destroyObjPool()
		{
			g_objPool.Destroy();
		}

		TCPPacket::SmartPoolObjectPtr TCPPacket::createSmartPoolObj()
		{
			return SmartPoolObjectPtr(new SmartPoolObject<TCPPacket>(ObjPool().FetchObj(), g_objPool));
		}

		TCPPacket::TCPPacket(MessageID msgID /* = 0 */, size_t res /* = 0 */)
			:Packet(msgID, true, res)
		{
			data_resize(maxBufferSize());
			wpos(0);
		}

		TCPPacket::~TCPPacket(){}

		size_t TCPPacket::maxBufferSize()
		{
			return PACKET_MAX_SIZE_TCP;
		}

		void TCPPacket::onReclaimObject()
		{
			Packet::onReclaimObject();
			data_resize(maxBufferSize());
		}

		int TCPPacket::recvFromEndPoint(EndPoint& ep, Address* pAddr /* = NULL */)
		{
			SL_ASSERT(maxBufferSize() > wpos());

			int len = ep.recv(data() + wpos(), (int)(size() - wpos()));

			if(len > 0)
				wpos((int)wpos() + len);

			return len;

		}

	}
}