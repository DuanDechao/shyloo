#include "slpacket_filter.h"
#include "slchannel.h"
namespace sl { 
namespace network{
//-------------------------------------------------------------------------------------
PacketFilter::PacketFilter()
{}

//-------------------------------------------------------------------------------------
PacketFilter::~PacketFilter()
{}

void PacketFilter::send(Channel * pChannel, const char* data, const int32 datalen){
    pChannel->putSendBuffer(data, datalen);
}
//-------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------
} 
}
