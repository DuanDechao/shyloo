#ifndef _SL_BUNDLE_BROADCAST_H_
#define _SL_BUNDLE_BROADCAST_H_
#include "slnetbase.h"
#include "slbundle.h"
#include "slendpoint.h"
#include <string>
namespace sl{
namespace network{
class NetworkInterface;
class EventDispatcher;
/*
	可以方便的处理如：向局域网内广播某些消息，并处理收集相关信息
*/
class BundleBroadcast: public Bundle
{
public:
	BundleBroadcast(NetworkInterface& networkInterface, uint16 bindPort = SL_PORT_BROADCAST_DISCOVERY,
		uint32 recvWindowSize = PACKET_MAX_SIZE_TCP);
	virtual ~BundleBroadcast();

	EventDispatcher& dispatcher();

	bool broadcast(uint16 port = 0);
	bool receive(/*MessageArgs* recvArgs,*/ sockaddr_in* psin = NULL, int32 timeout = 100000, bool showerr = true);

	network::EndPoint& epListen(){
		return m_epListen;
	}

	void close();

	bool good() const {
		return m_epListen.good() && m_good;
	}

	void itry(int8 i){
		m_itry = i;
	}

	void addBroadCastAddress(std::string addr);
protected:
	network::EndPoint m_epListen, m_epBroadcast;
	NetworkInterface& m_networkInterface;
	uint32 m_recvWindowSize;
	bool m_good;
	int8 m_itry;
	std::vector<std::string> m_machine_addresses;
};
}
}
#endif