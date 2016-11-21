#ifndef _SL_NETWORKCHANNEL_H_
#define _SL_NETWORKCHANNEL_H_
#include "sltimer.h"
#include "sltimestamp.h"
#include "slcommon.h"
#include "slpacket.h"
#include "slendpoint.h"
#include "slnetbase.h"
#include "slpacket_filter.h"
#include "slobjectpool.h"
#include "sladdress.h"
#include "slnet.h"
namespace sl
{
namespace network
{
class Bundle;
class NetworkInterface;
class MessageHandlers;
class PacketReader;
class PacketSender;
class PacketReceiver;
//class PacketFilter;
class EventDispatcher;

class Channel:public TimerHandler, public PoolObject, public ISLChannel
{
public:
	typedef SLShared_ptr<SmartPoolObject<Channel>> SmartPoolObjectPtr;
	static SmartPoolObjectPtr createSmartPoolObj();
	static CObjectPool<Channel>& ObjPool();
	static Channel* createPoolObject();
	static void reclaimPoolObject(Channel* obj);
	static void destroyObjPool();
	void onReclaimObject();
	virtual size_t getPoolObjectBytes();

	enum Traits
	{
		//server to server
		INTERNAL = 0,

		//client to server
		EXTERNAL = 1,
	};

	enum ChannelTypes
	{
		//��ͨͨ��
		CHANNEL_NORMAL = 0,

		///webͨ��
		CHANNEL_WEB = 1,
	};

	typedef std::vector<Packet*> BufferedReceives;
	typedef std::vector<Bundle*> Bundles;
public:
	Channel();

	Channel(NetworkInterface& networkInterface,
		const EndPoint* pEndPoint,
		Traits traits,
		ProtocolType pt = PROTOCOL_TCP,
		PacketFilterPtr pFilter = NULL,
		ChannelID id = CHANNEL_ID_NULL);

	virtual ~Channel();

	virtual void SLAPI Send(MessageID msgID, const char* pBuf, int dwLen);

	static Channel* get(NetworkInterface& networkInterface,
		const Address& addr);

	static Channel* get(NetworkInterface& networkInterface,
		const EndPoint* pSocket);

	void startInactivityDetection(float inactivityPeriod,
		float checkPeriod = 1.f);

	void stopInactivityDetection();

	PacketFilterPtr getFilter() const {return m_pFilter;}
	void setFilter(PacketFilterPtr pFilter) {m_pFilter = pFilter;}

	void destroy();

	bool isDestroyed() const {return (m_flags & FLAG_DESTROYED) > 0;}

	NetworkInterface& getNetworkInterface()	{return *m_pNetworkInterface;}
	NetworkInterface* getNetworkInterfacePtr() {return m_pNetworkInterface;}

	void setNetworkInterface(NetworkInterface* pNetworkInterface) {m_pNetworkInterface = pNetworkInterface;}

	inline const Address& addr() const;
	void setEndPoint(const EndPoint* pEndPoint);
	inline EndPoint* getEndPoint() const;

	Bundles& bundles();

	/*
		��������bundle,��bundle�����Ǵ�send���뷢�Ͷ����л�ȡ�ģ��������Ϊ�գ�����һ���µ�
	*/
	Bundle* createSendBundle();
	int32 bundlesLength();

	const Bundles& bundles() const;

	inline void pushBundle(Bundle* pBundle);
	void clearBundle();

	bool sending() const {return (m_flags & FLAG_SENDING) > 0;}
	void stopSend();

	void send(Bundle* pBundle = NULL);
	
	void delayedSend();

	inline PacketReader* getPacketReader() const;
	inline PacketSender* getPacketSender() const;
	inline void setPacketSender(PacketSender* pPacketSender);
	inline PacketReceiver* getPacketReceiver() const;

	Traits traits() const {return m_traits;}
	bool isExternal() const {return m_traits == EXTERNAL;}
	bool isInternal() const {return m_traits == INTERNAL;}

	void onPacketReceived(int bytes);
	void onPacketSent(int bytes, bool sendCompleted);
	void onSendCompleted();

	const char* c_str() const;
	ChannelID id() const {return m_id;}

	uint32 numPacketSent() const {return m_numPacketsSent;}
	uint32 numPacketReceived() const {return m_numPacketsReceived;}
	uint32 numBytesSent() const {return m_numBytesSent;}
	uint32 numBytesReceived() const {return m_numBytesReceived;}

	uint64 lastReceivedTime() const {return m_lastReceivedTime;}
	void updateLastReceivedTime() {m_lastReceivedTime = timestamp();}

	void addReceiveWindow(Packet* pPacket);

	BufferedReceives& bufferedReceives() {return m_bufferedReceives;}

	void processPackets(network::MessageHandlers* pMsgHandlers);

	bool isCondemn() const {return (m_flags & FLAG_CONDEMN) > 0;}
	void condemn();

	bool hasHandshake() const {return (m_flags & FLAG_HANDSHAKE) > 0;}

	ENTITY_ID getProxyID() const {return m_proxyID;}
	void setProxyID(ENTITY_ID pid){m_proxyID = pid;}

	COMPONENT_ID getComponentID() const {return m_componentID;}
	void setComponentID(COMPONENT_ID cid) {m_componentID = cid;}

	virtual void handshake();

	MessageHandlers* getMsgHandlers() const {return m_pMsgHandlers;}
	void setMsgHandlers(MessageHandlers* pMsgHandlers) {m_pMsgHandlers = pMsgHandlers;}

	bool waitSend();

	bool initialize(NetworkInterface& networkInterface,
		const EndPoint* pEndPoint,
		Traits traits,
		ProtocolType pt = PROTOCOL_TCP,
		PacketFilterPtr pFilter = NULL,
		ChannelID id = CHANNEL_ID_NULL);

	bool finalise();

	inline void setSession(ISLSession* poSession) {m_pSession = poSession;}

private:
	enum Flags
	{
		FLAG_SENDING	=	0x00000001,			///< ������Ϣ��
		FLAG_DESTROYED	=	0x00000002,			///< ͨ���Ѿ�����
		FLAG_HANDSHAKE	=	0x00000004,			///< �Ѿ����ֹ�
		FLAG_CONDEMN	=	0x00000008,			///< ��Ƶ���Ѿ���ò��Ϸ�
	};

	enum TimeOutType
	{
		TIMEOUT_INACTIVITY_CHECK
	};

	virtual void handlerTimeOut(TimerHandle, void* pUser);
	void clearState(bool warnOnDiscard = false);
	EventDispatcher& dispatcher();

private:
	NetworkInterface*			m_pNetworkInterface;
	Traits						m_traits;
	ProtocolType				m_protocolType;

	ChannelID					m_id;

	TimerHandle					m_inactivityTimerHandle;

	uint64						m_inactivityExceptionPeriod;

	uint64						m_lastReceivedTime;

	Bundles						m_bundles;

	BufferedReceives			m_bufferedReceives;

	PacketReader*				m_pPacketReader;

	///statistics

	uint32						m_numPacketsSent;
	uint32						m_numPacketsReceived;
	uint32						m_numBytesSent;
	uint32						m_numBytesReceived;
	uint32						m_lastTickBytesReceived;
	uint32						m_lastTickBytesSent;

	PacketFilterPtr				m_pFilter;

	EndPoint*					m_pEndPoint;
	PacketReceiver*				m_pPacketReceiver;
	PacketSender*				m_pPacketSender;

	///������ⲿͨ���Ҵ�����һ��ǰ������ǰ�˴���ID
	ENTITY_ID					m_proxyID;

	//ͨ�����
	ChannelTypes				m_channelType;

	COMPONENT_ID				m_componentID;

	//֧��ָ��ĳ��ͨ��ʹ�õ�ĳ����Ϣhandlers
	sl::network::MessageHandlers* m_pMsgHandlers;

	uint32						m_flags;

	ISLSession*					m_pSession;
};

}
}
#include "slchannel.inl"
#endif