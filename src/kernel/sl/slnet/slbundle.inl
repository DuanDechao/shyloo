namespace sl{
namespace network{
inline bool Bundle::empty() const
{
	return packetsSize() == 0;
}

inline int Bundle::packetsSize() const
{
	size_t i =m_packets.size();
	if(m_pCurrPacket && !m_pCurrPacket->empty())
		++i;

	return (int)i;
}

inline void Bundle::setCurrMsgLength(MessageLength1 v)
{
	m_currMsgLength = v;
}

inline MessageLength1 Bundle::getCurrMsgLength() const
{
	return m_currMsgLength;
}

//inline void Bundle::setCurrMsgHandler(const network::MessageHandler* pMsgHandler)
//{
//	m_pCurrMsgHandler = pMsgHandler;
//    
//	if(m_pCurrMsgHandler)
//		m_currMsgID = pMsgHandler->msgID;
//	else
//		m_currMsgID = 0;
//}

//inline const network::MessageHandler* Bundle::getCurrMsgHandler() const
//{
//	return m_pCurrMsgHandler;
//}

inline Bundle::Packets& Bundle::packets()
{
	return m_packets;
}

inline Packet* Bundle::getCurrPacket() const
{
	return m_pCurrPacket;
}

inline void Bundle::setCurrPacket(Packet* p)
{
	m_pCurrPacket = p;
}

inline MessageID Bundle::getMessageID() const
{
	return m_currMsgID;
}

inline void Bundle::setMessageID(MessageID id)
{
	m_currMsgID = id;
}

inline int32 Bundle::packetMaxSize() const
{
	return m_packetMaxSize;
}

inline int32 Bundle::lastPacketSpace()
{
	if(m_packets.size() > 0)
		return packetMaxSize() - m_packets.back()->wpos();

	return 0;
}

inline bool Bundle::packetHaveSpace()
{
	return lastPacketSpace() > 8;
}

inline int32 Bundle::numMessages() const
{
	return m_numMessages;
}

inline void Bundle::setChannel(Channel* pChannel)
{
	m_pChannel = pChannel;
}

inline Channel* Bundle::getChannel() const
{
	return m_pChannel;
}

inline void Bundle::finiCurrPacket()
{
	if(!m_pCurrPacket)
		return;

	m_packets.push_back(m_pCurrPacket);
	setCurrMsgPacketCount(getCurrMsgPacketCount() + 1);
	m_pCurrPacket = NULL;
}

inline void Bundle::setCurrMsgPacketCount(uint32 v)
{
	m_currMsgPacketCount = v;
}

inline uint32 Bundle::getCurrMsgPacketCount() const
{
	return m_currMsgPacketCount;
}

inline void Bundle::setCurrMsgLengthPos(size_t v)
{
	m_currMsgLengthPos = v;
}

inline size_t Bundle::getCurrMsgLengthPos() const
{
	return m_currMsgLengthPos;
}
}
}