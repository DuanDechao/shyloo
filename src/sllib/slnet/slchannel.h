#ifndef _SL_NETWORKCHANNEL_H_
#define _SL_NETWORKCHANNEL_H_
#include "slpacket.h"
#include "slendpoint.h"
#include "slnetbase.h"
#include "slobjectpool.h"
#include "sladdress.h"
#include "slnet.h"
namespace sl
{
namespace network
{
class Bundle;
class NetworkInterface;
class PacketReader;
class PacketSender;
class PacketReceiver;

class Channel:public ISLChannel
{
public:
	enum ChannelTypes
	{
		//普通通道
		CHANNEL_NORMAL = 0,

		///web通道
		CHANNEL_WEB = 1,
	};

	typedef std::vector<Packet*> BufferedReceives;
	typedef std::vector<Bundle*> Bundles;

public:
	Channel();
	Channel(NetworkInterface* networkInterface,
		const EndPoint* pEndPoint,
		ISLPacketParser* poPacketParser,
		ProtocolType pt = PROTOCOL_TCP,
		ChannelID id = CHANNEL_ID_NULL);

	virtual ~Channel();

public:
	virtual bool SLAPI isConnected(void) { return (m_flags & FLAG_CONNECTED) > 0;}
	virtual void SLAPI send(const char* pBuf, uint32 dwLen);
	virtual void SLAPI disconnect(void);

	virtual const uint32 SLAPI getRemoteIP(void){return m_pEndPoint ? m_pEndPoint->addr().m_ip : 0;}
	virtual const char* SLAPI getRemoteIPStr(void) {return m_pEndPoint ? m_pEndPoint->addr().ipAsString() : "";}
	virtual const uint16 SLAPI getRemotePort(void) { return m_pEndPoint ? m_pEndPoint->addr().m_port : 0; }

	virtual const uint32 SLAPI getLocalIP(void) {return 0;}
	virtual const char* SLAPI getLocalIPStr(void) {return "";}
	virtual const uint16 SLAPI getLocalPort(void) {return 0;}


public:
	const char* c_str() const;
	ChannelID id() const { return m_id; }
	inline const Address& addr() const;

	uint32 numPacketSent() const { return m_numPacketsSent; }
	uint32 numPacketReceived() const { return m_numPacketsReceived; }
	uint32 numBytesSent() const { return m_numBytesSent; }
	uint32 numBytesReceived() const { return m_numBytesReceived; }
	uint64 lastReceivedTime() const { return m_lastReceivedTime; }
	void updateLastReceivedTime() { m_lastReceivedTime = getTimeMilliSecond(); }

	void setConnected();
	bool isDestroyed() const {return (m_flags & FLAG_DESTROYED) > 0;}
	bool isCondemn() const { return (m_flags & FLAG_CONDEMN) > 0; }
	void condemn();
	bool sending() const { return (m_flags & FLAG_SENDING) > 0; }

	NetworkInterface& getNetworkInterface()	{return *m_pNetworkInterface;}
	NetworkInterface* getNetworkInterfacePtr() {return m_pNetworkInterface;}
	void setNetworkInterface(NetworkInterface* pNetworkInterface) {m_pNetworkInterface = pNetworkInterface;}
	inline void setSession(ISLSession* poSession) { m_pSession = poSession; }
	inline ISLSession* getSession() { return m_pSession; }
	
	inline EndPoint* getEndPoint() const;
	Bundles& bundles();
	const Bundles& bundles() const;
	int32 bundlesLength();
	
	void send(Bundle* pBundle = NULL);
	void stopSend();
	void delayedSend();
	bool waitSend();
	
	inline PacketReader* getPacketReader() const;
	inline PacketSender* getPacketSender() const;
	inline void setPacketSender(PacketSender* pPacketSender);
	inline PacketReceiver* getPacketReceiver() const;
	void processPackets();
	void destroy();

	void addReceiveWindow(Packet* pPacket);
	void onPacketReceived(int bytes);
	void onPacketSent(int bytes, bool sendCompleted);
	void onSendCompleted();

private:
	
	bool finalise();
	void clearState(bool warnOnDiscard = false);

	void clearBundle();
	Bundle* createSendBundle(); //创建发送bundle,该bundle可能是从send放入发送队列中获取的，如果队列为空，创建一个新的
	inline void pushBundle(Bundle* pBundle);

	
	void setEndPoint(const EndPoint* pEndPoint);
	BufferedReceives& bufferedReceives() { return m_bufferedReceives; }


private:
	enum Flags
	{
		FLAG_SENDING	=	0x00000001,			///< 发送信息中
		FLAG_DESTROYED	=	0x00000002,			///< 通道已经销毁
		FLAG_CONDEMN	=	0x00000004,			///< 该频道已经变得不合法
		FLAG_CONNECTED	=	0x00000008,			///< 通道建立连接
	};

private:
	
	ProtocolType				m_protocolType;
	ChannelID					m_id;
	uint64						m_lastReceivedTime;
	Bundles						m_bundles;
	BufferedReceives			m_bufferedReceives;
	ChannelTypes				m_channelType;
	uint32						m_flags;
	
	///statistics
	uint32						m_numPacketsSent;
	uint32						m_numPacketsReceived;
	uint32						m_numBytesSent;
	uint32						m_numBytesReceived;
	uint32						m_lastTickBytesReceived;
	uint32						m_lastTickBytesSent;

	//外部对象指针，不需要内部释放
	NetworkInterface*			m_pNetworkInterface;
	ISLSession*					m_pSession;
	ISLPacketParser*			m_pPacketParser;

	//内部对象指针，需要内部释放
	EndPoint*					m_pEndPoint;
	PacketReader*				m_pPacketReader;
	PacketReceiver*				m_pPacketReceiver;
	PacketSender*				m_pPacketSender;
	
	
};

CREATE_OBJECT_POOL(Channel);

}
}
#include "slchannel.inl"
#endif