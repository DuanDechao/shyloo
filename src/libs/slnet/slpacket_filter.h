#ifndef _SL_PACKET_FILTER_H_
#define _SL_PACKET_FILTER_H_

#include "slnetbase.h"
#include "slrefcountable.h"
#include "slsmartpointer.h"
namespace sl
{
namespace network
{
class Channel;
class NetworkInterface;
class Packet;
class Address;
//class PacketFilter;
class PacketReceiver;
class PacketSender;

class PacketFilter: public RefCountable
{
public:
	virtual ~PacketFilter(){}
	virtual Reason send(Channel* pChannel, PacketSender& sender, Packet* pPacket);
	virtual Reason recv(Channel* pChannel, PacketReceiver& receiver, Packet* pPacket);
};
typedef SmartPointer<PacketFilter> PacketFilterPtr;

}
}
#endif