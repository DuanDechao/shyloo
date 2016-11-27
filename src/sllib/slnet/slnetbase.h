#ifndef _SL_NETWORK_COMMON_H_
#define _SL_NETWORK_COMMON_H_
#include "slmulti_sys.h"
namespace sl
{
namespace network
{
const uint32 BROADCAST		=	0xFFFFFFFF;
const uint32 LOCALCAST		=	0x0100007F;

//消息的ID
typedef uint16				MessageID;
typedef uint16				MessageLength;
typedef uint32				MessageLength1;


typedef int32				ChannelID;
const ChannelID				CHANNEL_ID_NULL = 0;


//通道超時時間
extern float g_channelInternalTimeout;
extern float g_channelExternalTimeout;


//外部通道加密類別
extern int8 g_channelExternalEncryptType;

//listen监听队列最大值
extern uint32 g_SOMAXCONN;

//加密額外存儲的信息佔用的字節（長度+填充）
#define ENCRYPTTION_WASTAGE_SIZE		(1 + 7)

#define PACKET_MAX_SIZE					1500
#ifndef PACKET_MAX_SIZE_TCP
#define PACKET_MAX_SIZE_TCP				1460
#endif // !PACKET_MAX_SIZE_TCP
#define PACKET_MAX_SIZE_UDP				1472

typedef uint16							PacketLength;			///< 最大65535
#define	PACKET_LENGTH_SIZE				sizeof(PacketLength)

#define NETWORK_MESSAGE_ID_SIZE			sizeof(network::MessageID)
#define NETWORK_MESSAGE_LENGTH_SIZE		sizeof(network::MessageLength)
#define NETWORK_MESSAGE_LENGTH1_SIZE	sizeof(network::MessageLength1)
#define NETWORK_MESSAGE_MAX_SIZE		65535
#define NETWORK_MESSAGE_MAX_SIZE1		4294967295

//sl machine端口
#define SL_PORT_START					20000
#define SL_MACHINE_BROADCAST_SEND_PORT	SL_PORT_START + 86		///< machine接收广播的端口
#define SL_PORT_BROADCAST_DISCOVERY		SL_PORT_START + 87		///
#define SL_MACHINE_TCP_PORT				SL_PORT_START + 88

/*
 网络消息类型，定长或者变长
 如果需要自定义长度则在NETWORK_INTERFACE_DECLARE_BEGIN中声明时填入长度即可
*/
#ifndef NETWORK_FIXED_MESSAGE
#define NETWORK_FIXED_MESSAGE 0
#endif

#ifndef NETWORK_VARIABLE_MESSAGE
#define NETWORK_VARIABLE_MESSAGE -1
#endif

//網絡消息類別
enum NETWORK_MESSAGE_TYPE
{
	NETWORK_MESSAGE_TYPE_COMPONENT = 0,		///< 組件消息
	NETWORK_MESSAGE_TYPE_ENTITY	=1,			///< entity消息
};

//消息长度
typedef uint16				MessageLength;		///最大65535
typedef uint32				MessageLength1;		///最大4294967295


#ifndef IFF_UP
enum
{
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
enum ProtocolType
{
	PROTOCOL_TCP = 0,
	PROTOCOL_UDP = 1,
};

enum Reason
{
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
	REASON_CHANNEL_CONDEMN		= -14,				///< condemn error

};

typedef UINT_PTR			SLSOCKET;

#define SEND_BUNDLE_COMMON(SND_FUNC, BUNDLE)																\
	BUNDLE.finiMessage();																					\
																											\
	network::Bundle::Packets::iterator iter = BUNDLE.packets().begin();										\
	for (; iter != BUNDLE.packets().end(); ++iter)															\
	{																										\
		Packet* pPacket = (*iter);																			\
		int retries = 0;																					\
		Reason reason;																						\
		pPacket->m_sentSize = 0;																				\
																											\
		while(true)																							\
		{																									\
			++retries;																						\
			int slen = SND_FUNC;																			\
																											\
			if(slen > 0)																					\
				pPacket->m_sentSize += slen;																	\
																											\
			if(pPacket->m_sentSize != pPacket->length())														\
			{																								\
				reason = PacketSender::checkSocketErrors(&ep);												\
				/* 如果发送出现错误那么我们可以继续尝试一次， 超过60次退出	*/								\
			if (reason == REASON_NO_SUCH_PORT && retries <= 3)												\
			{																								\
				continue;																					\
			}																								\
																											\
			/* 如果系统发送缓冲已经满了，则我们等待10ms	*/													\
			if ((reason == REASON_RESOURCE_UNAVAILABLE || reason == REASON_GENERAL_NETWORK)					\
					&& retries <= 60)																		\
			{																								\
				ep.waitSend();																				\
				continue;																					\
			}																								\
																											\
			if(retries > 60 && reason != REASON_SUCCESS)													\
			{																								\
			}																								\
			}																								\
			else																							\
			{																								\
				break;																						\
			}																								\
		}																									\
																											\
	}																										\
																											\
	BUNDLE.clearPackets();																					\
																											\

#define SEND_BUNDLE(ENDPOINT, BUNDLE)																		\
{																											\
	EndPoint& ep = ENDPOINT;																				\
	SEND_BUNDLE_COMMON(ENDPOINT.send(pPacket->data() + pPacket->m_sentSize,									\
	(int32)(pPacket->length() - pPacket->m_sentSize)), BUNDLE);														\
}																											\

#define SENDTO_BUNDLE(ENDPOINT, ADDR, PORT, BUNDLE)															\
{																											\
	EndPoint& ep = ENDPOINT;																				\
	SEND_BUNDLE_COMMON(ENDPOINT.sendto(pPacket->data() + pPacket->m_sentSize,								\
	(int32)(pPacket->length() - pPacket->m_sentSize), PORT, ADDR), BUNDLE);											\
}																											\

#define MALLOC_PACKET(outputPacket, isTCPPacket)															\
{																											\
	if(isTCPPacket)																							\
	outputPacket = TCPPacket::createPoolObject();															\
	else																									\
	outputPacket = UDPPacket::createPoolObject();															\
}																											\

#define RECLAIM_PACKET(isTCPPacket, pPacket)																\
{																											\
	if(isTCPPacket)																							\
	TCPPacket::reclaimPoolObject(static_cast<TCPPacket*>(pPacket));											\
	else																									\
	UDPPacket::reclaimPoolObject(static_cast<UDPPacket*>(pPacket));											\
}																											\

//network stats
extern uint64				g_numPacketsSent;
extern uint64				g_numPacketsReceived;
extern uint64				g_numBytesSent;
extern uint64				g_numBytesReceived;



}
}//namespace sl
#endif