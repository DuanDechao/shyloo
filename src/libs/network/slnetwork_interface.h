#ifndef _SL_NETWROK_INTERFACE_H_
#define _SL_NETWROK_INTERFACE_H_

#include "../common/slmemorystream.h"
#include "slcommon.h"
#include "../common/slbase.h"
#include "../common/sltimer.h"
#include "slendpoint.h"

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

class NetworkInterface: public TimerHandler
{
public:
	typedef std::map<Address, Channel*>		ChannelMap;

	NetworkInterface(EventDispatcher* pDispatcher,
		int32 extlisteningPort_min = -1, int32 extlisteningPort_max = -1, const char* extlisteningInterface="",
		uint32 extrbuffer = 0, uint32 extwbuffer = 0,
		int32 intlisteningPort = 0, const char* intlisteningInterface = "",
		uint32 intrbuffer = 0, uint32 intwbuffer = 0);

	~NetworkInterface();

	inline const Address& extaddr() const;
	inline const Address& intaddr() const;

	bool recreateListeningSocket(const char* pEndPointName, uint16 listeningPort_min, uint16 listeningPort_max,
		const char* listeningInterface, EndPoint* pEP, ListenerReceiver* pLR, uint32 rbuffer = 0, uint32 wbuffer = 0);

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

	///外部网点和内部网点
	EndPoint& extEndPoint()		{return m_extEndPoint;}
	EndPoint& intEndPoint()		{return m_intEndPoint;}

	bool isExternal() const {return m_isExternal;}

	const char* c_str() const {return m_extEndPoint.c_str();}

	void* getExtensionData() const {return m_pExtensionData;}
	void setExtensionData(void* data) {m_pExtensionData = data;}

	const ChannelMap& channels(void) {return m_channelMap;}

	//发送相关
	void sendIfDelayed(Channel& channel);
	void delayedSend(Channel& channel);

	bool good() const {return (!isExternal() || m_extEndPoint.good()) && (m_intEndPoint.good());}

	void onChannelTimeOut(Channel* pChannel);

	//处理所有channels
	void processChannels(MessageHandlers* pMsgHandlers);
	inline int32 numExtChannels() const;

private:
	virtual void handleTimeout(TimerHandle handle, void * arg);
	void closeSocket();

private:
	EndPoint						m_extEndPoint;
	EndPoint						m_intEndPoint;

	ChannelMap						m_channelMap;

	EventDispatcher*				m_pDispatcher;

	void*							m_pExtensionData;

	ListenerReceiver*				m_pExtListenerReceiver;
	ListenerReceiver*				m_pIntListenerReceiver;

	DelayedChannels*				m_pDelayedChannels;

	ChannelTimeOutHandler*			m_pChannelTimeOutHandler;
	ChannelDeregisterHandler*		m_pChannelDeregisterHandler;

	const bool						m_isExternal;

	int32							m_numExtChannels;
};
}
}
#endif