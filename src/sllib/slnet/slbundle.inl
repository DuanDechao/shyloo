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

inline int32 Bundle::packetMaxSize() const
{
	return m_packetMaxSize;
}

inline int32 Bundle::lastPacketSpace()
{
	if(m_packets.size() > 0)
		return (int32)(packetMaxSize() - (int32)m_packets.back()->wpos());

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


}
}