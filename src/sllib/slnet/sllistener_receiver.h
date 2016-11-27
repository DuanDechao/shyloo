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

class ListenerReceiver: public InputNotificationHandler
{
public:
	ListenerReceiver(EndPoint& endpoint, Channel::Traits traits, NetworkInterface& networkInterface);
	~ListenerReceiver();

private:
	virtual int handleInputNotification(int fd);
	EventDispatcher& dispatcher();

private:
	EndPoint&			m_endpoint;
	Channel::Traits		m_traits;
	NetworkInterface&	m_networkInterface;
};
}
}
#endif