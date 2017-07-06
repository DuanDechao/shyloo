#include "sltcp_packet_receiver.h"
#include "slchannel.h"
#include "slnetwork_interface.h"
namespace sl{
namespace network{

sl::SLPool<TCPPacketReceiver> TCPPacketReceiver::s_pool;
TCPPacketReceiver::TCPPacketReceiver(Channel* channel, NetworkInterface* networkInterface)
	:PacketReceiver(channel, networkInterface)
{}

TCPPacketReceiver::~TCPPacketReceiver(){}

bool TCPPacketReceiver::processRecv(bool expectingPacket){
	SLASSERT(_channel != NULL, "wtf");

	if (_channel->isDestroyed())
		return false;

	int32 len = _channel->recvFromEndPoint();
	if(len < 0){
		PacketReceiver::RecvState rstate = this->checkSocketErrors(len, expectingPacket);
		if(rstate == PacketReceiver::RECV_STATE_INTERRUPT){
			onGetError(_channel);
			return false;
		}
		return rstate == PacketReceiver::RECV_STATE_CONTINUE;
	}
	else if(len == 0){
		///�����˳�
		onGetError(_channel);
		return false;
	}

	Reason ret = this->processPacket(_channel, len);
	if(ret != REASON_SUCCESS){
		return false;
	}
	return true;
}

void TCPPacketReceiver::onGetError(Channel* pChannel){
	pChannel->destroy();
}

Reason TCPPacketReceiver::processRecievePacket(Channel* pChannel){
	pChannel->processPackets();
	return REASON_SUCCESS;
}


PacketReceiver::RecvState TCPPacketReceiver::checkSocketErrors(int len, bool expectingPacket)
{
#ifdef SL_OS_WINDOWS
	DWORD wsaErr = WSAGetLastError();
#endif // SL_OS_WINDOWS

	if(
#ifdef SL_OS_WINDOWS
		wsaErr == WSAEWOULDBLOCK && !expectingPacket		///< Send�������ǻ��������ˣ� recv�����Ѿ������ݿɶ���
#else
		errno == EAGAIN && !expectingPacket
#endif
		)
	{
		return RECV_STATE_BREAK;
	}

#ifdef  SL_OS_LINUX
	if(errno == EAGAIN ||				///< �Ѿ������ݿɶ���
		errno == ECONNREFUSED ||		///< ���ӱ��������ܾ�
		errno == EHOSTUNREACH)			///< Ŀ�ĵ�ַ���ɵ���
	{
		return RECV_STATE_BREAK;
	}
#else
	if(wsaErr == WSAECONNRESET || wsaErr == WSAECONNABORTED)
	{
		return RECV_STATE_INTERRUPT;
	}
#endif //  SL_OS_LINUX

	return RECV_STATE_CONTINUE;


}


}
}
