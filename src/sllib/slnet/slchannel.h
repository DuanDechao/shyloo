#ifndef _SL_LIB_NET_CHANNEL_H_
#define _SL_LIB_NET_CHANNEL_H_
#include "slpacket.h"
#include "slendpoint.h"
#include "slnetbase.h"
#include "slobjectpool.h"
#include "sladdress.h"
#include "slnet.h"
namespace sl{
namespace network{

class Bundle;
class NetworkInterface;
class PacketReader;
class PacketSender;
class PacketReceiver;

class Channel:public ISLChannel{
public:
	enum ChannelTypes{
		//普通通道
		CHANNEL_NORMAL = 0,

		///web通道
		CHANNEL_WEB = 1,
	};

	typedef std::vector<Packet*> BufferedReceives;
	typedef std::vector<Bundle*> Bundles;

public:
	Channel(NetworkInterface* networkInterface,
		const EndPoint* pEndPoint,
		ISLPacketParser* poPacketParser,
		ProtocolType pt = PROTOCOL_TCP,
		ChannelID id = CHANNEL_ID_NULL);

	virtual ~Channel();

	static Channel* create(NetworkInterface* networkInterface, const EndPoint* pEndPoint, ISLPacketParser* poPacketParser,
		ProtocolType pt = PROTOCOL_TCP, ChannelID id = CHANNEL_ID_NULL){
		return CREATE_FROM_POOL(s_pool, networkInterface, pEndPoint, poPacketParser, pt, id);
	}

	inline void release(){
		s_pool.recover(this);
	}

public:
	virtual bool SLAPI isConnected(void) { return (_flags & FLAG_CONNECTED) > 0;}
	virtual void SLAPI send(const char* pBuf, uint32 dwLen);
	virtual void SLAPI disconnect(void);

	virtual const uint32 SLAPI getRemoteIP(void){return _pEndPoint ? _pEndPoint->addr().ip() : 0;}
	virtual const char* SLAPI getRemoteIPStr(void) {return _pEndPoint ? _pEndPoint->addr().ipAsString() : "";}
	virtual const uint16 SLAPI getRemotePort(void) { return _pEndPoint ? _pEndPoint->addr().port() : 0; }

	virtual const uint32 SLAPI getLocalIP(void) {return 0;}
	virtual const char* SLAPI getLocalIPStr(void) {return "";}
	virtual const uint16 SLAPI getLocalPort(void) {return 0;}


public:
	inline const char* c_str() const;
	inline ChannelID id() const { return _id; }
	inline const Address& addr() const { _pEndPoint->addr(); }
	inline EndPoint* getEndPoint() const{ return  _pEndPoint; }

	inline uint32 numPacketSent() const { return _numPacketsSent; }
	inline uint32 numPacketReceived() const { return _numPacketsReceived; }
	inline uint32 numBytesSent() const { return _numBytesSent; }
	inline uint32 numBytesReceived() const { return _numBytesReceived; }
	inline uint64 lastReceivedTime() const { return _lastReceivedTime; }
	inline void updateLastReceivedTime() { _lastReceivedTime = getTimeMilliSecond(); }

	inline bool isDestroyed() const {return (_flags & FLAG_DESTROYED) > 0;}
	inline bool isCondemn() const { return (_flags & FLAG_CONDEMN) > 0; }
	inline bool sending() const { return (_flags & FLAG_SENDING) > 0; }

	inline NetworkInterface& getNetworkInterface()	{return *_pNetworkInterface;}
	inline NetworkInterface* getNetworkInterfacePtr() {return _pNetworkInterface;}
	inline void setNetworkInterface(NetworkInterface* pNetworkInterface) {_pNetworkInterface = pNetworkInterface;}
	inline void setSession(ISLSession* poSession) { _pSession = poSession; }
	inline ISLSession* getSession() { return _pSession; }
	
	inline void setConnected();
	inline void condemn();
	
	Bundles& bundles();
	const Bundles& bundles() const;
	int32 bundlesLength();
	
	void send(Bundle* pBundle = NULL);
	void stopSend();
	void delayedSend();
	bool waitSend();
	
	inline PacketReader* getPacketReader() const{ return _pPacketReader; }
	inline PacketSender* getPacketSender() const { return _pPacketSender; }
	inline void setPacketSender(PacketSender* pPacketSender){ _pPacketSender = pPacketSender; }
	inline PacketReceiver* getPacketReceiver() const{ return _pPacketReceiver; }
	void processPackets();
	void destroy(bool notify = true);

	void addReceiveWindow(Packet* pPacket);
	void onPacketReceived(int bytes);
	void onPacketSent(int bytes, bool sendCompleted);
	void onSendCompleted();

private:
	
	bool finalise();
	void clearState(bool warnOnDiscard = false);

	void clearBundle();
	Bundle* createSendBundle(); //创建发送bundle,该bundle可能是从send放入发送队列中获取的，如果队列为空，创建一个新的
	inline void pushBundle(Bundle* pBundle){ _bundles.push_back(pBundle); }

	void setEndPoint(const EndPoint* pEndPoint);
	BufferedReceives& bufferedReceives() { return _bufferedReceives; }

private:
	enum Flags{
		FLAG_SENDING	=	0x00000001,			///< 发送信息中
		FLAG_DESTROYED	=	0x00000002,			///< 通道已经销毁
		FLAG_CONDEMN	=	0x00000004,			///< 该频道已经变得不合法
		FLAG_CONNECTED	=	0x00000008,			///< 通道建立连接
	};

private:
	ProtocolType				_protocolType;
	ChannelID					_id;
	uint64						_lastReceivedTime;
	Bundles						_bundles;
	BufferedReceives			_bufferedReceives;
	ChannelTypes				_channelType;
	uint32						_flags;
	
	///statistics
	uint32						_numPacketsSent;
	uint32						_numPacketsReceived;
	uint32						_numBytesSent;
	uint32						_numBytesReceived;
	uint32						_lastTickBytesReceived;
	uint32						_lastTickBytesSent;

	//外部对象指针，不需要内部释放
	NetworkInterface*			_pNetworkInterface;
	ISLSession*					_pSession;
	ISLPacketParser*			_pPacketParser;

	//内部对象指针，需要内部释放
	EndPoint*					_pEndPoint;
	PacketReader*				_pPacketReader;
	PacketReceiver*				_pPacketReceiver;
	PacketSender*				_pPacketSender;
	
	static sl::SLPool<Channel>  s_pool;
};

}
}
#endif