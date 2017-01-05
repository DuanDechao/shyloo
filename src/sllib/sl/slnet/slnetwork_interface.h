#ifndef _SL_NETWROK_INTERFACE_H_
#define _SL_NETWROK_INTERFACE_H_

#include "slmemorystream.h"
#include "slnetbase.h"
#include "sltimer.h"
#include "slendpoint.h"
#include "slnet.h"
namespace sl
{
namespace network
{
class Address;
class Bundle;
class Channel;
class ChannelTimeOutHandler;
class ChannelDeregisterHandler;
class DelayedChannels;
class ListenerReceiver;
class Packet;
class EventDispatcher;
class MessageHandlers;
class TCPPacketReceiver;

#define CHECK_DESTROY_CHANNEL_TIME   2 * 60 * 1000
class NetworkInterface
{
public:
	typedef std::map<Address, Channel*>			ChannelMap;

	NetworkInterface(EventDispatcher* pEventDispatcher);

	~NetworkInterface();

	bool createListeningSocket(const char* listeningInterface, uint16 listeningPort, 
		EndPoint* pLEP, ListenerReceiver* pLR, uint32 rbuffer = 0, uint32 wbuffer = 0);

	bool createConnectingSocket(const char* serverIp, uint16 serverPort,
		ISLSession* pSession, ISLPacketParser* poPacketParser, uint32 rbuffer = 0, uint32 wbuffer = 0);

	bool registerChannel(Channel* pChannel);
	bool deregisterChannel(Channel* pChannel);
	bool deregisterAllChannels();
	Channel* findChannel(const Address& addr);
	Channel* findChannel(int fd);

	ChannelTimeOutHandler* getChannelTimeOutHandler() const {return m_pChannelTimeOutHandler;}
	void setChannelTimeOutHandler(ChannelTimeOutHandler* pHandler) {m_pChannelTimeOutHandler = pHandler;}

	ChannelDeregisterHandler* getChannelDeregisterHandler() const {return m_pChannelDeregisterHandler;}
	void setChannelDeregisterHandler(ChannelDeregisterHandler* pHandler) {m_pChannelDeregisterHandler = pHandler;}

	EventDispatcher& getDispatcher() {return *m_pDispatcher;}

	bool deregisterSocket(int32 fd);

	const ChannelMap& channels(void) {return m_channelMap;}

	//∑¢ÀÕœ‡πÿ
	void sendIfDelayed(Channel& channel);

	void onChannelTimeOut(Channel* pChannel);

	inline int32 numExtChannels() const;

	int32 checkDestroyChannel();

private:

	ChannelMap						m_channelMap;

	EventDispatcher*				m_pDispatcher;

	DelayedChannels*				m_pDelayedChannels;

	ChannelTimeOutHandler*			m_pChannelTimeOutHandler;
	ChannelDeregisterHandler*		m_pChannelDeregisterHandler;

	int32							m_numExtChannels;

	int64							m_lastCheckDestroyChannelTime;
};
}
}
#endif