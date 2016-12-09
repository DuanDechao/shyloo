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

	PacketReader(Channel* pChannel, ISLPacketParser* poPacketParser);
	virtual ~PacketReader();

	virtual void reset();

	virtual void processMessages(Packet* pPacket);

	virtual PacketReader::PACKET_READER_TYPE type() const {return PACKET_READER_TYPE_SOCKET;}

protected:

	virtual int32 mergeFragmentMessage(Packet* pPacket);
protected:
	MemoryStream*			m_pFragmentStream;
	uint32					m_pFragmentStreamLength;
	Channel*				m_pChannel;
	ISLPacketParser*		m_pPacketParser;
};

}
}//namespace sl
#endif