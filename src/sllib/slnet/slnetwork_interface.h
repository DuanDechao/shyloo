#ifndef _SL_LIB_NET_NETWROK_INTERFACE_H_
#define _SL_LIB_NET_NETWROK_INTERFACE_H_

#include "slnetbase.h"
#include "slendpoint.h"
#include "slnet.h"
#include <set>
namespace sl{
namespace network{
class Address;
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
class NetworkInterface{
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

	ChannelTimeOutHandler* getChannelTimeOutHandler() const {return _pChannelTimeOutHandler;}
	void setChannelTimeOutHandler(ChannelTimeOutHandler* pHandler) {_pChannelTimeOutHandler = pHandler;}

	ChannelDeregisterHandler* getChannelDeregisterHandler() const {return _pChannelDeregisterHandler;}
	void setChannelDeregisterHandler(ChannelDeregisterHandler* pHandler) {_pChannelDeregisterHandler = pHandler;}

	EventDispatcher& getDispatcher() {return *_pDispatcher;}

	bool deregisterSocket(int32 fd);

	const ChannelMap& channels(void) {return _channelMap;}

	//∑¢ÀÕœ‡πÿ
	void sendIfDelayed(Channel& channel);

	void onChannelTimeOut(Channel* pChannel);

	inline int32 numExtChannels() const;

	void recoverDestroyChannel();

private:

	ChannelMap						_channelMap;
	std::set<Channel*>				_destoryChannel;

	EventDispatcher*				_pDispatcher;

	DelayedChannels*				_pDelayedChannels;

	ChannelTimeOutHandler*			_pChannelTimeOutHandler;
	ChannelDeregisterHandler*		_pChannelDeregisterHandler;

	int32							_numExtChannels;

	int64							_lastCheckDestroyChannelTime;
};
}
}
#endif