#include "sltcp_packet_receiver.h"
#include "slchannel.h"
#include "sltcp_packet.h"
namespace sl
{
namespace network
{

TCPPacketReceiver::TCPPacketReceiver(EndPoint* endpoint,
									 NetworkInterface* networkInterface)
									 :PacketReceiver(endpoint, networkInterface)
{}

TCPPacketReceiver::~TCPPacketReceiver(){}

bool TCPPacketReceiver::processRecv(bool expectingPacket)
{
	Channel* pChannel = getChannel();
	SLASSERT(pChannel != NULL, "wtf");

	if(pChannel->isCondemn())
	{
		return false;
	}

	TCPPacket* pReceiveWindow = CREATE_POOL_OBJECT(TCPPacket);
	int len = pReceiveWindow->recvFromEndPoint(*m_pEndPoint);

	if(len < 0)
	{
		RELEASE_POOL_OBJECT(TCPPacket, pReceiveWindow);

		PacketReceiver::RecvState rstate = this->checkSocketErrors(len, expectingPacket);

		if(rstate == PacketReceiver::RECV_STATE_INTERRUPT){
			onGetError(pChannel);
			return false;
		}
		return rstate == PacketReceiver::RECV_STATE_CONTINUE;
	}
	else if(len == 0)	///�����˳�
	{
		RELEASE_POOL_OBJECT(TCPPacket, pReceiveWindow);
		onGetError(pChannel);
		return false;
	}

	Reason ret = this->processPacket(pChannel, pReceiveWindow);
	if(ret != REASON_SUCCESS)
	{

	}
	return true;
}

void TCPPacketReceiver::onGetError(Channel* pChannel)
{
	pChannel->condemn();
}

Reason TCPPacketReceiver::processRecievePacket(Channel* pChannel, Packet* pPacket){
	
	if(pPacket)
	{
		pChannel->addReceiveWindow(pPacket);
		pChannel->processPackets();
	}

	return REASON_SUCCESS;
}


PacketReceiver::RecvState TCPPacketReceiver::checkSocketErrors(int len, bool expectingPacket)
{
#ifdef SL_OS_WINDOWS
	DWORD wsaErr = WSAGetLastError();
#endif // SL_OS_WINDOWS

	if(
#ifdef SL_OS_WINDOWS
		wsaErr = WSAEWOULDBLOCK && !expectingPacket		///< Send�������ǻ��������ˣ� recv�����Ѿ������ݿɶ���
#else
		errno = EAGAIN && !expectingPacket
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