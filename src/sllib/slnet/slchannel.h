#ifndef _SL_LIB_NET_CHANNEL_H_
#define _SL_LIB_NET_CHANNEL_H_
#include "slendpoint.h"
#include "slnetbase.h"
#include "sladdress.h"
#include "slnet.h"
#include "slring_buffer.h"

namespace sl{
namespace network{
class NetworkInterface;
class PacketReader;
class PacketSender;
class PacketReceiver;

class Channel:public ISLChannel{
public:
	inline static Channel* create(NetworkInterface* networkInterface, const EndPoint* pEndPoint, ISLPacketParser* poPacketParser,
		const int32 recvSize, const int32 sendSize, ProtocolType pt = PROTOCOL_TCP)
	{
		return CREATE_FROM_POOL(s_pool, networkInterface, pEndPoint, poPacketParser, recvSize, sendSize, pt);
	}

	inline void release(){
		s_pool.recover(this);
	}

public:
	virtual bool SLAPI isConnected(void) { return (_flags & FLAG_CONNECTED) > 0;}
	virtual void SLAPI send(const char* pBuf, uint32 dwLen);
	virtual void SLAPI disconnect(void);

	virtual const uint32 SLAPI getRemoteIP(void){ return _pEndPoint ? _pEndPoint->getremoteAddress().ip() : 0; }
	virtual const char* SLAPI getRemoteIPStr(void) { return _pEndPoint ? _pEndPoint->getremoteAddress().ipAsString() : ""; }
	virtual const uint16 SLAPI getRemotePort(void) { return _pEndPoint ? _pEndPoint->getremoteAddress().port() : 0; }

	virtual const uint32 SLAPI getLocalIP(void) { return _pEndPoint ? _pEndPoint->getlocalAddress().ip() : 0; }
	virtual const char* SLAPI getLocalIPStr(void) { return _pEndPoint ? _pEndPoint->getlocalAddress().ipAsString() : ""; }
	virtual const uint16 SLAPI getLocalPort(void) { return _pEndPoint ? _pEndPoint->getlocalAddress().port() : 0;; }

	virtual void SLAPI adjustSendBuffSize(const int32 size);
	virtual void SLAPI adjustRecvBuffSize(const int32 size);

public:
	inline const char* c_str() const;
	inline const Address& addr() const { return _pEndPoint->addr(); }
	inline EndPoint* getEndPoint() const{ return  _pEndPoint; }

	inline uint32 numPacketSent() const { return _numPacketsSent; }
	inline uint32 numPacketReceived() const { return _numPacketsReceived; }
	inline uint32 numBytesSent() const { return _numBytesSent; }
	inline uint32 numBytesReceived() const { return _numBytesReceived; }
	inline uint64 lastReceivedTime() const { return _lastReceivedTime; }
	inline void updateLastReceivedTime() { _lastReceivedTime = getTimeMilliSecond(); }

	inline bool isDestroyed() const {return (_flags & FLAG_DESTROYED) > 0;}
	inline bool sending() const { return (_flags & FLAG_SENDING) > 0; }

	inline NetworkInterface& getNetworkInterface()	{return *_pNetworkInterface;}
	inline NetworkInterface* getNetworkInterfacePtr() {return _pNetworkInterface;}
	inline void setNetworkInterface(NetworkInterface* pNetworkInterface) {_pNetworkInterface = pNetworkInterface;}
	inline void setSession(ISLSession* poSession) { _pSession = poSession; }
	inline ISLSession* getSession() { return _pSession; }
	
	void setConnected();
	void stopSend();
	void delayedSend();
	bool waitSend();
	
	inline PacketSender* getPacketSender() const { return _pPacketSender; }
	inline void setPacketSender(PacketSender* pPacketSender){ _pPacketSender = pPacketSender; }
	inline PacketReceiver* getPacketReceiver() const{ return _pPacketReceiver; }
	inline void setPacketReceiver(PacketReceiver* pPacketReceiver) { _pPacketReceiver = pPacketReceiver; }
	void processPackets();
	void destroy(bool notify = true);

	void onPacketReceived(int bytes);
	void onPacketSent(int bytes, bool sendCompleted);
	void onSendCompleted();


	int32 recvFromEndPoint();
	int32 sendToEndPoint();

	inline bool sendBufEmpty() const { return _sendBuf->getDataSize() == 0; }

private:
	friend sl::SLPool<Channel>;
	Channel(NetworkInterface* networkInterface,
		const EndPoint* pEndPoint,
		ISLPacketParser* poPacketParser,
		const int32 recvSize, const int32 sendSize,
		ProtocolType pt = PROTOCOL_TCP);

	virtual ~Channel();
	
	bool finalise();
	void clearState(bool warnOnDiscard = false);
	void setEndPoint(const EndPoint* pEndPoint);

	sl::SLRingBuffer* adjustNewBuffSize(SLRingBuffer* buf, const int32 newSize);

private:
	enum Flags{
		FLAG_NONE		=   0x00000000,
		FLAG_SENDING	=	0x00000001,			///< 发送信息中
		FLAG_DESTROYED	=	0x00000002,			///< 通道已经销毁
		FLAG_CONNECTED	=	0x00000004,			///< 通道建立连接
	};

private:
	ProtocolType				_protocolType;
	uint64						_lastReceivedTime;
	uint32						_flags;
	sl::SLRingBuffer*			_recvBuf;
	sl::SLRingBuffer*			_sendBuf;
	int32						_recvSize;
	int32						_sendSize;

	
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
	PacketReceiver*				_pPacketReceiver;
	PacketSender*				_pPacketSender;
	
	static sl::SLPool<Channel>  s_pool;
};

}
}
#endif
