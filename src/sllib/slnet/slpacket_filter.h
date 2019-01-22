#ifndef SL_PACKET_FILTER_H
#define SL_PACKET_FILTER_H
#include "slmulti_sys.h"
namespace sl { 
namespace network{
class Channel;
class PacketFilter{
public:
	PacketFilter();
	virtual ~PacketFilter();

	virtual void send(Channel * pChannel, const char* data, const int32 dataLen);
    virtual int32 recv(Channel * pChannel, char* data, int32 readLen){}
};

}
}

#endif // KBE_WEBSOCKET_PACKET_FILTER_H
