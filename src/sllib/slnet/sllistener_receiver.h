#ifndef _SL_LISTENER_RECEIVER_H_
#define _SL_LISTENER_RECEIVER_H_

#include "sltimer.h"
#include "slnetbase.h"
#include "slinterfaces.h"
#include "slpacket.h"
#include "slchannel.h"

namespace sl
{
namespace network
{
class EndPoint;
class Address;
class NetworkInferace;
class EventDispatcher;
class ISLPacketParser;

class ListenerReceiver: public InputNotificationHandler
{
public:
	ListenerReceiver();
	ListenerReceiver(EndPoint* endpoint, NetworkInterface* networkInterface);
	~ListenerReceiver();

	void setSessionFactory(ISLSessionFactory* poSessionFactory);
	void setPacketParser(ISLPacketParser* poPacketParser);

private:
	virtual int handleInputNotification(int fd);
	EventDispatcher& dispatcher();
	
private:
	EndPoint*			m_endpoint;
	NetworkInterface*	m_networkInterface;
	ISLSessionFactory*	m_pSessionFactory;
	ISLPacketParser*	m_pPacketParser;
};
}
}
#endif