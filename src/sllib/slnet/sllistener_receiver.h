#ifndef _SL_LIB_NET_LISTENER_RECEIVER_H_
#define _SL_LIB_NET_LISTENER_RECEIVER_H_

#include "slnetbase.h"
#include "slinterfaces.h"
#include "slchannel.h"

namespace sl{
namespace network{
class EndPoint;
class Address;
class NetworkInferace;
class EventDispatcher;
class ISLPacketParser;

class ListenerReceiver: public InputNotificationHandler{
public:
	ListenerReceiver();
	ListenerReceiver(EndPoint* endpoint, NetworkInterface* networkInterface);
	~ListenerReceiver();

	void setSessionFactory(ISLSessionFactory* poSessionFactory);
	void setPacketParser(ISLPacketParser* poPacketParser);

private:
	virtual int handleInputNotification(int fd);
	
private:
	EndPoint*			_endpoint;
	NetworkInterface*	_networkInterface;
	ISLSessionFactory*	_pSessionFactory;
	ISLPacketParser*	_pPacketParser;
};
}
}
#endif