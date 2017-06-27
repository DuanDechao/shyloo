#ifndef _SL_PACKET_READER_H_
#define _SL_PACKET_READER_H_
#include "slnetbase.h"
#include "slpacket.h"
#include "slmemorystream.h"
namespace sl
{
namespace network
{
class Channel;
class MessageHandlers;
class ISLPacketParser;
class PacketReader
{
public:
	enum PACKET_READER_TYPE
	{
		PACKET_READER_TYPE_SOCKET = 0,
		PACKET_READER_TYPE_WEBSOCKET = 1
	};
	PacketReader();
	PacketReader(Channel* pChannel, ISLPacketParser* poPacketParser);
	virtual ~PacketReader();

	virtual void reset();

	virtual void processMessages(const char* msgBuf, const int32 size);

	virtual PacketReader::PACKET_READER_TYPE type() const {return PACKET_READER_TYPE_SOCKET;}

protected:

	virtual int32 mergeFragmentMessage(Packet* pPacket);
protected:
	MemoryStream*			m_pFragmentStream;
	uint32					m_pFragmentStreamLength;
	uint32					m_lastFragmentStreamLength;
	Channel*				m_pChannel;
	ISLPacketParser*		m_pPacketParser;
};
CREATE_OBJECT_POOL(PacketReader);

}
}//namespace sl
#endif