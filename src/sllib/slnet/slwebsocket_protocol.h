#ifndef SL_WEBSOCKET_PROTOCOL_H
#define SL_WEBSOCKET_PROTOCOL_H
#include "slmulti_sys.h"
#include "slchannel.h"
namespace sl{

class MemoryStream;

namespace network
{
//	class Channel;
//	class Packet;

namespace websocket{


/*	WebSocket FRC6544
*/

class WebSocketProtocol{
public:
	enum FrameType {
		// 下一帧与结束
		NEXT_FRAME = 0x0,
		END_FRAME = 0x80,

		ERROR_FRAME = 0xFF00,
		INCOMPLETE_FRAME = 0xFE00,

		OPENING_FRAME = 0x3300,
		CLOSING_FRAME = 0x3400,

		// 未完成的帧
		INCOMPLETE_TEXT_FRAME = 0x01,
		INCOMPLETE_BINARY_FRAME = 0x02,

		// 文本帧与二进制帧
		TEXT_FRAME = 0x81,
		BINARY_FRAME = 0x82,

		PING_FRAME = 0x19,
		PONG_FRAME = 0x1A,

		// 关闭连接
		CLOSE_FRAME = 0x08
	};

	/**
		是否是websocket协议
	*/
	static bool isWebSocketProtocol(const char* data, const int32 datalen);
	
	/**
		websocket协议握手
	*/
	static int32 handshake(network::Channel* pChannel, const char* data, const int32 datalen);

	/**
		帧解析相关
	*/
	static int makeFrame(FrameType frame_type, uint64 size , MemoryStream* pOutStream);
	static int getFrame(char* data, uint64 datalen, uint8& msg_opcode, uint8& msg_fin, uint8& msg_masked, uint32& msg_mask, 
		int32& msg_length_field, uint64& msg_payload_length, FrameType& frameType);

	static bool decodingDatas(char* data, int32 datalen, uint8 msg_masked, uint32 msg_mask);
};

}
}
}

#endif // SL_WEBSOCKET_PROTOCOL_H

