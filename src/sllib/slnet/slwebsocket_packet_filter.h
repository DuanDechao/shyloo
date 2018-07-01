#ifndef SL_WEBSOCKET_PACKET_FILTER_H
#define SL_WEBSOCKET_PACKET_FILTER_H

#include "slpacket_filter.h"
#include "slwebsocket_protocol.h"

namespace sl { 
namespace network{
class WebSocketPacketFilter : public PacketFilter
{
public:
	WebSocketPacketFilter();
	virtual ~WebSocketPacketFilter();

	virtual void send(Channel * pChannel, const char* data, const int32 dataLen);
	virtual int32 recv(Channel * pChannel, char* dataBuf, int32 buflen);

protected:
	void reset();

protected:
	enum FragmentDataTypes
	{
		FRAGMENT_MESSAGE_HREAD,
		FRAGMENT_MESSAGE_DATAS
	};

	int32										pFragmentDatasRemain_;
	FragmentDataTypes							fragmentDatasFlag_;

	uint8										msg_opcode_;
	uint8										msg_fin_;
	uint8										msg_masked_;
	uint32										msg_mask_;
	int32										msg_length_field_;
	uint64										msg_payload_length_;
	websocket::WebSocketProtocol::FrameType		msg_frameType_;
	sl::SLRingBuffer*			                _recvFilterBuf;
	sl::SLRingBuffer*			                _sendFilterBuf;
};


}
}

#endif // KBE_WEBSOCKET_PACKET_FILTER_H
