#ifndef _SL_PACKET_READER_H_
#define _SL_PACKET_READER_H_
#include "slcommon.h"
#include "slpacket.h"
#include "../common/slmemorystream.h"
namespace sl
{
namespace network
{
class Channel;
class MessageHandlers;

class PacketReader
{
public:
	enum PACKET_READER_TYPE
	{
		PACKET_READER_TYPE_SOCKET = 0,
		PACKET_READER_TYPE_WEBSOCKET = 1
	};

	PacketReader(Channel* pChannel);
	virtual ~PacketReader();

	virtual void reset();

	virtual void processMessages(sl::network::MessageHandlers* pMsgHandlers, Packet* pPacket);

	network::MessageID	currMsgID() const {return m_currMsgID;}
	network::MessageLength	currMsgLen() const {return m_currMsgLen;}

	void currMsgID(network::MessageID id) {m_currMsgID = id;}
	void currMsgLen(network::MessageLength len) {m_currMsgLen = len;}

	virtual PacketReader::PACKET_READER_TYPE type() const {return PACKET_READER_TYPE_SOCKET;}

protected:
	enum FragmentDataTypes
	{
		FRAGMENT_DATA_UNKNOW,
		FRAGMENT_DATA_MESSAGE_ID,
		FRAGMENT_DATA_MESSAGE_LENGTH,
		FRAGMENT_DATA_MESSAGE_LENGTH1,
		FRAGMENT_DATA_MESSAGE_BODY
	};

	virtual void writeFragmentMessage(FragmentDataTypes fragmentDataFlag, Packet* packet, uint32 datasize);
	virtual void mergeFragmentMessage(Packet* pPacket);
protected:
	uint8*					m_pFragmentDatas;
	uint32					m_pFragmentDatasWpos;
	uint32					m_pFragmentDatasRemain;
	FragmentDataTypes		m_pFragmentDatasType;
	MemoryStream*			m_pFragmentStream;
	network::MessageID		m_currMsgID;
	network::MessageLength1	m_currMsgLen;

	Channel*				m_pChannel;
};

}
}//namespace sl
#endif