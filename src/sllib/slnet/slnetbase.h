#ifndef _SL_LIB_NET_NETWORK_BASE_H_
#define _SL_LIB_NET_NETWORK_BASE_H_
#include "slmulti_sys.h"
#ifndef WIN32
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/tcp.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>
#endif
namespace sl{
namespace network{

const uint32 BROADCAST		=	0xFFFFFFFF;
const uint32 LOCALCAST		=	0x0100007F;

//��Ϣ��ID
typedef uint16				MessageID;
typedef uint16				MessageLength;
typedef uint32				MessageLength1;


//ͨ�����r�r�g
extern float g_channelInternalTimeout;
extern float g_channelExternalTimeout;


//�ⲿͨ������e
extern int8 g_channelExternalEncryptType;

//listen�����������ֵ
extern uint32 g_SOMAXCONN;

//�����~��惦����Ϣ���õ��ֹ����L��+��䣩
#define ENCRYPTTION_WASTAGE_SIZE		(1 + 7)

#define PACKET_MAX_SIZE					1500
#ifndef PACKET_MAX_SIZE_TCP
#define PACKET_MAX_SIZE_TCP				1460
#endif // !PACKET_MAX_SIZE_TCP
#define PACKET_MAX_SIZE_UDP				1472

typedef uint16							PacketLength;			///< ���65535
#define	PACKET_LENGTH_SIZE				sizeof(PacketLength)

#define NETWORK_MESSAGE_ID_SIZE			sizeof(network::MessageID)
#define NETWORK_MESSAGE_LENGTH_SIZE		sizeof(network::MessageLength)
#define NETWORK_MESSAGE_LENGTH1_SIZE	sizeof(network::MessageLength1)
#define NETWORK_MESSAGE_MAX_SIZE		65535
#define NETWORK_MESSAGE_MAX_SIZE1		4294967295

//sl machine�˿�
#define SL_PORT_START					20000
#define SL_MACHINE_BROADCAST_SEND_PORT	SL_PORT_START + 86		///< machine���չ㲥�Ķ˿�
#define SL_PORT_BROADCAST_DISCOVERY		SL_PORT_START + 87		///
#define SL_MACHINE_TCP_PORT				SL_PORT_START + 88

/*
 ������Ϣ���ͣ��������߱䳤
 �����Ҫ�Զ��峤������NETWORK_INTERFACE_DECLARE_BEGIN������ʱ���볤�ȼ���
*/
#ifndef NETWORK_FIXED_MESSAGE
#define NETWORK_FIXED_MESSAGE 0
#endif

#ifndef NETWORK_VARIABLE_MESSAGE
#define NETWORK_VARIABLE_MESSAGE -1
#endif

//�W�j��Ϣe
enum NETWORK_MESSAGE_TYPE
{
	NETWORK_MESSAGE_TYPE_COMPONENT = 0,		///< �M����Ϣ
	NETWORK_MESSAGE_TYPE_ENTITY	=1,			///< entity��Ϣ
};

//��Ϣ����
typedef uint16				MessageLength;		///���65535
typedef uint32				MessageLength1;		///���4294967295


#ifndef IFF_UP
enum{
	IFF_UP					=	0x1,
	IFF_BROADCAST			=	0x2,
	IFF_DEBUG				=	0x4,
	IFF_LOOPBACK			=   0x8,
	IFF_POINTOPINT			=	0x10,
	IFF_NOTRAILERS			=	0x20,
	IFF_RUNNING				=	0x40,
	IFF_NOARP				=	0x80,
	IFF_PROMISC				=	0x100,
	IFF_MULTICAST			=	0x1000
};
#endif // !IFF_UP


#define PACKET_MAX_SIZE_UDP			1472
enum ProtocolType{
	PROTOCOL_TCP = 0,
	PROTOCOL_UDP = 1,
};

enum Reason{
	REASON_SUCCESS				= 0,				///< No reason
	REASON_TIMER_EXPIRED		= -1,				///< Time expired
	REASON_NO_SUCH_PORT			= -2,				///< Destination port is not open
	REASON_GENERAL_NETWORK		= -3,				///< the network is stuffed
	REASON_CORRUPTED_PACKET		= -4,				///< got a bad socket
	REASON_NONEXISTENT_ENTRY	= -5,				///< wanted to call a null function
	REASON_WINDOW_OVERFLOW		= -6,				///< Channel send window overflowed
	REASON_INACTIVITY			= -7,				///< channel inactivity timeout
	REASON_RESOURCE_UNAVAILABLE = -8,				///< Corresponds to EAGAIN
	REASON_CLIENT_DISCONNECTED	= -9,				///< CLient disconnected voluntarily
	REASON_TRANSMIT_QUEUE_FULL  = -10,				///< Corresponds to ENOBUFS
	REASON_CHANNEL_LOST			= -11,				///< Corresponds to channel lost
	REASON_SHUTTING_DOWN		= -12,				///< Corresponds to shutting down up
	REASON_WEBSOCKET_ERROR		= -13,				///< html5 error
	REASON_CHANNEL_DESTROYED	= -14,				///< destroy error

};

#ifdef WIN32
typedef UINT_PTR			SLSOCKET;
#else
typedef uintptr_t			SLSOCKET;
#endif

#ifdef SL_OS_WINDOWS
//Windows socket�Ĵ�����ͺ����linux��һ����ͨ����ͳһ
#define SL_WSA_ERRNO		WSAGetLastError()
#define SL_EWOULDBLOCK		WSAEWOULDBLOCK		//	10035
#define SL_EINPROGRESS		WSAEINPROGRESS		//	10036
#define SL_ETIME			WSAETIMEDOUT		//	10060
#define SL_EINTR			WSAEINTR			//	10000
#define SL_ECONNRESET		WSAECONNRESET		//	10054	������ʾԶ�̹ر�������

typedef int socklen_t;

#endif

#ifdef SL_OS_LINUX
#define SL_WSA_ERRNO		errno
#define SL_EWOULDBLOCK		EWOULDBLOCK
#define SL_EINPROGRESS		EINPROGRESS
#define SL_ETIME			ETIME
#define SL_EINTR			EINTR
#define SL_ECONNRESET		ECONNRESET

#define closesocket			close

#endif

#ifndef SL_INVALID_SOCKET
#define	SL_INVALID_SOCKET	(SLSOCKET)(~0)
#endif

//network stats
extern uint64				g_numPacketsSent;
extern uint64				g_numPacketsReceived;
extern uint64				g_numBytesSent;
extern uint64				g_numBytesReceived;



}
}//namespace sl
#endif