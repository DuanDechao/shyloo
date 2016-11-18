#include "slpacket_filter.h"
#include "slpacket_sender.h"
#include "slpacket_receiver.h"
namespace sl
{
namespace network
{
Reason PacketFilter::send(Channel* pChannel, PacketSender& sender, Packet* pPacket)
{
	return sender.processFilterPacket(pChannel, pPacket);
}
Reason PacketFilter::recv(Channel* pChannel, PacketReceiver& receiver, Packet* pPacket)
{
	return receiver.processFilteredPacket(pChannel,pPacket);
}
}
}