namespace sl{
namespace network{

inline PacketReceiver* Channel::getPacketReceiver() const
{
	return m_pPacketReceiver;
}

inline PacketSender* Channel::getPacketSender() const
{
	return m_pPacketSender;
}

inline void Channel::setPacketSender(PacketSender* pPacketSender)
{
	m_pPacketSender = pPacketSender;
}

inline void Channel::pushBundle(Bundle* pBundle)
{
	m_bundles.push_back(pBundle);
}

}
}