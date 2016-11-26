#include "slchannel.h"
#include "slpacket_receiver.h"
#include "sltcp_packet_receiver.h"
#include "sludp_packet_receiver.h"
#include "slnetwork_interface.h"
#include "slevent_dispatcher.h"
#include "slpacket_sender.h"
#include "sladdress.h"
#include "slbundle.h"
#include "sltcp_packet_sender.h"
#include "sltcp_packet.h"
#include "slpacket_reader.h"
namespace sl
{
namespace network
{

static CObjectPool<Channel> g_objPool("Channel");
CObjectPool<Channel>& Channel::ObjPool()
{
	return g_objPool;
}

Channel* Channel::createPoolObject()
{
	return g_objPool.FetchObj();
}

void Channel::reclaimPoolObject(Channel* obj)
{
	g_objPool.ReleaseObj(obj);
}

void Channel::destroyObjPool()
{
	g_objPool.Destroy();
}

size_t Channel::getPoolObjectBytes()
{
	size_t bytes = sizeof(m_pNetworkInterface) + sizeof(m_traits) +
		sizeof(m_id) + sizeof(m_inactivityExceptionPeriod) +
		sizeof(m_lastReceivedTime) + (m_bufferedReceives.size() * sizeof(Packet*)) + sizeof(m_pPacketReader) 
		+ sizeof(m_flags) + sizeof(m_numBytesSent) + sizeof(m_numBytesReceived) + sizeof(m_numPacketsSent) + sizeof(m_numPacketsReceived)
		+ sizeof(m_lastTickBytesReceived) + sizeof(m_lastTickBytesSent) + sizeof(m_pFilter) + sizeof(m_pEndPoint) + sizeof(m_pPacketReceiver) + sizeof(m_pPacketSender)
		+ sizeof(m_channelType);

	return bytes;
}

Channel::SmartPoolObjectPtr Channel::createSmartPoolObj()
{
	return SmartPoolObjectPtr(new SmartPoolObject<Channel>(ObjPool().FetchObj(), g_objPool));
}

void Channel::onReclaimObject()
{
	this->clearState();
}

Channel::Channel(NetworkInterface& networkInterface,
				 const EndPoint* pEndPoint, Traits traits, ProtocolType pt,
				 PacketFilterPtr pFilter, ChannelID id)
				 :m_pNetworkInterface(NULL),
				  m_traits(traits),
				  m_protocolType(pt),
				  m_id(id),
				  m_inactivityExceptionPeriod(0),
				  m_lastReceivedTime(0),
				  m_bundles(),
				  m_pPacketReader(0),
				  m_numPacketsSent(0),
				  m_numPacketsReceived(0),
				  m_numBytesSent(0),
				  m_numBytesReceived(0),
				  m_lastTickBytesReceived(0),
				  m_lastTickBytesSent(0),
				  m_pFilter(pFilter),
				  m_pEndPoint(NULL),
				  m_pPacketReceiver(NULL),
				  m_pPacketSender(NULL),
				  m_channelType(CHANNEL_NORMAL),
				  m_flags(0)
{
	this->clearBundle();
	initialize(networkInterface, pEndPoint, traits, pt, pFilter, id);
}

Channel::Channel()
	:m_pNetworkInterface(NULL),
	 m_traits(EXTERNAL),
	 m_protocolType(PROTOCOL_TCP),
	 m_id(0),
	 m_inactivityExceptionPeriod(0),
	 m_lastReceivedTime(0),
	 m_bundles(),
	 m_pPacketReader(),
	 m_numPacketsSent(0),
	 m_numPacketsReceived(0),
	 m_numBytesSent(0),
	 m_numBytesReceived(0),
	 m_lastTickBytesReceived(0),
	 m_lastTickBytesSent(0),
	 m_pFilter(NULL),
	 m_pEndPoint(NULL),
	 m_pPacketReceiver(NULL),
	 m_pPacketSender(NULL),
	 m_channelType(CHANNEL_NORMAL),
	 m_flags(0)
{
	this->clearBundle();
}

Channel::~Channel()
{
	finalise();
}


bool Channel::initialize(NetworkInterface& networkInterface, const EndPoint* pEndPoint,
						 Traits traits, ProtocolType pt /* = PROTOCOL_TCP */, 
						 PacketFilterPtr pFilter /* = NULL */, 
						 ChannelID id /* = CHANNEL_ID_NULL */)
{
	m_id = id;
	m_protocolType = pt;
	m_traits = traits;
	m_pFilter = pFilter;
	m_pNetworkInterface = &networkInterface;
	this->setEndPoint(pEndPoint);

	SL_ASSERT(m_pNetworkInterface != NULL);
	SL_ASSERT(m_pEndPoint != NULL);

	if(m_protocolType == PROTOCOL_TCP)
	{
		if(m_pPacketReceiver)
		{
			if(m_pPacketReceiver->type() == PacketReceiver::UDP_PACKET_RECEIVER)
			{
				SAFE_RELEASE(m_pPacketReceiver);
				m_pPacketReceiver = new TCPPacketReceiver(*m_pEndPoint, *m_pNetworkInterface);
			}
		}
		else
		{
			m_pPacketReceiver = new TCPPacketReceiver(*m_pEndPoint, *m_pNetworkInterface);
		}

		SL_ASSERT(m_pPacketReceiver->type() == PacketReceiver::TCP_PACKET_RECEIVER);

		m_pNetworkInterface->getDispatcher().registerReadFileDescriptor(*m_pEndPoint, m_pPacketReceiver);
	}
	else
	{
		if(m_pPacketReceiver)
		{
			if(m_pPacketReceiver->type() == PacketReceiver::TCP_PACKET_RECEIVER)
			{
				SAFE_RELEASE(m_pPacketReceiver);
				m_pPacketReceiver = new UDPPacketReceiver(*m_pEndPoint, *m_pNetworkInterface);
			}
		}
		else
		{
			m_pPacketReceiver = new UDPPacketReceiver(*m_pEndPoint, *m_pNetworkInterface);
		}
		SL_ASSERT(m_pPacketReceiver->type() == PacketReceiver::UDP_PACKET_RECEIVER);
	}

	m_pPacketReceiver->SetEndPoint(m_pEndPoint);
	if(m_pPacketSender)
		m_pPacketSender->SetEndPoint(m_pEndPoint);

	/*startInactivityDetection((m_traits == INTERNAL) ? g_channelInternalTimeout:
													  g_channelExternalTimeout,
							  (m_traits == INTERNAL) ? g_channelInternalTimeout /2.f:
							                           g_channelExternalTimeout / 2.f);*/
	return true;
}

bool Channel::finalise()
{
	this->clearState();
	SAFE_RELEASE(m_pPacketReceiver);
	SAFE_RELEASE(m_pPacketReader);
	SAFE_RELEASE(m_pPacketSender);

	EndPoint::reclaimPoolObject(m_pEndPoint);
	m_pEndPoint = NULL;
	return true;
}

Channel* Channel::get(NetworkInterface& networkInterface, const Address& addr)
{
	return networkInterface.findChannel(addr);
}

Channel* Channel::get(NetworkInterface& networkInterface, const EndPoint* pEndPoint)
{
	return networkInterface.findChannel(pEndPoint->addr());
}

void Channel::send(const char* pBuf, uint32 dwLen)
{
	Bundle* pBundle = Bundle::createPoolObject();


	/*MessageHandler msgHandler;
	msgHandler.msgID = msgID;
	msgHandler.msgLen = NETWORK_VARIABLE_MESSAGE;*/
	MessageID msgID = *(MessageID*)pBuf;

	(*pBundle).newMessage(msgID);
	(*pBundle) << pBuf+sizeof(msgID);
	send(pBundle);
}

void Channel::disconnect()
{
	destroy();
	condemn();
}

//void Channel::startInactivityDetection(float inactivityPeriod, float checkPeriod /* = 1.f */)
//{
//	stopInactivityDetection();
//	//如果週期為負數則不檢查
//	if(inactivityPeriod > 0.001f)
//	{
//		checkPeriod = max(1.f, checkPeriod);
//		m_inactivityExceptionPeriod = uint64(inactivityPeriod * stampsPerSecond()) - uint64(0.05f * stampsPerSecond());
//		m_lastReceivedTime = timestamp();
//	}
//}

void Channel::setEndPoint(const EndPoint* pEndPoint)
{
	if(m_pEndPoint != pEndPoint)
	{
		EndPoint::reclaimPoolObject(m_pEndPoint);
		m_pEndPoint = const_cast<EndPoint*>(pEndPoint);
	}

	m_lastReceivedTime = getTimeMilliSecond();
}

void Channel::destroy()
{
	if(isDestroyed())
	{
		return;
	}
	clearState();
	m_flags |= FLAG_DESTROYED;
}

void Channel::clearState(bool warnOnDiscard /* = false */)
{
	//清空未處理的接受包緩存
	if(m_bufferedReceives.size() > 0)
	{
		BufferedReceives::iterator iter = m_bufferedReceives.begin();
		int hasDiscard = 0;

		for (;iter != m_bufferedReceives.end(); ++iter)
		{
			Packet* pPacket = (*iter);
			if(pPacket->length() > 0)
				hasDiscard++;
			RECLAIM_PACKET(pPacket->IsTCPPacket(), pPacket);
		}
		if(hasDiscard > 0 && warnOnDiscard)
		{

		}

		m_bufferedReceives.clear();
	}

	clearBundle();

	m_lastReceivedTime = getTimeMilliSecond();

	m_numPacketsSent = 0;
	m_numPacketsReceived = 0;
	m_numBytesSent = 0;
	m_numBytesReceived = 0;
	m_lastTickBytesReceived = 0;
	m_channelType = CHANNEL_NORMAL;

	if(m_pEndPoint && m_protocolType == PROTOCOL_TCP && !this->isDestroyed())
	{
		this->stopSend();

		if(m_pNetworkInterface)
		{
			if(!this->isDestroyed())
				m_pNetworkInterface->getDispatcher().deregisterReadFileDescriptor(*m_pEndPoint);
		}
	}

	m_flags = 0;
	m_pFilter = NULL;

	//stopInactivityDetection();

	//由于endpoint通常由外部给入，必须释放，频道重新激活时重新赋值
	if(m_pEndPoint)
	{
		m_pEndPoint->close();
		this->setEndPoint(NULL);
	}
}

Channel::Bundles& Channel::bundles()
{
	return m_bundles;
}

const Channel::Bundles & Channel::bundles() const
{
	return m_bundles;
}

int32 Channel::bundlesLength()
{
	int32 len = 0;
	Bundles::iterator iter = m_bundles.begin();
	for (; iter!= m_bundles.end(); ++iter)
	{
		len += (*iter)->packetsLength();
	}
	return len;

}

void Channel::delayedSend()
{
//	this->getNetworkInterface().delayedSend(*this);
}

const char* Channel::c_str() const
{
	//static char dodgyString[]
	static char dodgyString[MAX_BUF] = {"None"};
	char tdodgyString[MAX_BUF] = {0};

	if(m_pEndPoint && !m_pEndPoint->addr().isNone())
		m_pEndPoint->addr().writeToString(tdodgyString, MAX_BUF);

	CPlatForm::_sl_snprintf(dodgyString, MAX_BUF, "%s/%d/%d/%d", tdodgyString, m_id,
		this->isCondemn(), this->isDestroyed());
	
	return dodgyString;
}

void Channel::clearBundle()
{
	Bundles::iterator iter = m_bundles.begin();
	for (; iter != m_bundles.end(); ++iter)
	{
		Bundle::reclaimPoolObject((*iter));
	}

	m_bundles.clear();
}

void Channel::send(Bundle* pBundle /* = NULL */)
{
	if(isDestroyed())
	{
		this->clearBundle();
		if(pBundle)
			Bundle::reclaimPoolObject(pBundle);
		return;
	}

	if(isCondemn())
	{
		this->clearBundle();
		if(pBundle)
			Bundle::reclaimPoolObject(pBundle);
		return;
	}

	if(pBundle)
	{
		pBundle->setChannel(this);
		pBundle->finiMessage(true);
		m_bundles.push_back(pBundle);
	}

	uint32 bundleSize = (uint32)m_bundles.size();
	if(bundleSize == 0)
		return;

	if(!sending())
	{
		if(m_pPacketSender == NULL)
		{
			m_pPacketSender = new TCPPacketSender(*m_pEndPoint, *m_pNetworkInterface);
		}

		m_pPacketSender->processSend(this);

		//如果不能立即發送到系統緩衝區，那麼交給poller處理
		if(m_bundles.size() > 0 && !isCondemn() && !isDestroyed())
		{
			m_flags |= FLAG_SENDING;
			m_pNetworkInterface->getDispatcher().registerWriteFileDescriptor(*m_pEndPoint, m_pPacketSender);
		}
	}
}

void Channel::stopSend()
{
	if(!sending())
		return;

	m_flags &= ~FLAG_SENDING;

	m_pNetworkInterface->getDispatcher().deregisterWriteFileDescriptor(*m_pEndPoint);
}

void Channel::onSendCompleted()
{
	SL_ASSERT(m_bundles.size() == 0 && sending());
	stopSend();
}

void Channel::onPacketSent(int bytes, bool sendCompleted)
{
	if(sendCompleted)
	{
		++m_numPacketsSent;
		++g_numPacketsSent;
	}

	m_numBytesSent += bytes;
	g_numBytesSent += bytes;
	m_lastTickBytesSent += bytes;

	if(this->isExternal())
	{
		//if()
	}
}

void Channel::onPacketReceived(int bytes)
{
	m_lastReceivedTime  = getTimeMilliSecond();
	++m_numPacketsReceived;
	++g_numPacketsReceived;

	m_numBytesReceived += bytes;
	m_lastTickBytesReceived += bytes;
	g_numBytesReceived += bytes;

	if(this->isExternal())
	{

	}
}

void Channel::addReceiveWindow(Packet* pPacket)
{
	m_bufferedReceives.push_back(pPacket);
	uint32 size = (uint32)m_bufferedReceives.size();

	//if(network::g_)
}

void Channel::condemn()
{
	if(isCondemn())
		return;

	m_flags |= FLAG_CONDEMN;
}

void Channel::handshake()
{
	if(hasHandshake())
		return;

	if(m_bufferedReceives.size() > 0)
	{
		BufferedReceives::iterator packetIter = m_bufferedReceives.begin();
		Packet* pPacket = (*packetIter);

		m_flags |= FLAG_HANDSHAKE;

		//此處判定是否為websocket或者其他協議的握手
		
	}
}

void Channel::processPackets(/*MessageHandlers* pMsgHandlers*/)
{
	m_lastTickBytesReceived = 0;
	m_lastTickBytesSent = 0;

	/*if(m_pMsgHandlers != NULL)
	pMsgHandlers = m_pMsgHandlers;*/

	if(this->isDestroyed())
	{
		return;
	}

	if(this->isCondemn())
	{
		return;
	}

	if(!hasHandshake())
	{
		handshake();
	}

	BufferedReceives::iterator packetIter = m_bufferedReceives.begin();
	for (; packetIter != m_bufferedReceives.end(); ++packetIter)
	{
		Packet* pPacket = (*packetIter);
		m_pPacketReader->processMessages(/*pMsgHandlers, */pPacket);
		RECLAIM_PACKET(pPacket->IsTCPPacket(), pPacket);
	}

	m_bufferedReceives.clear();
}

bool Channel::waitSend()
{
	return getEndPoint()->waitSend();
}

EventDispatcher& Channel::dispatcher()
{
	return m_pNetworkInterface->getDispatcher();
}

Bundle* Channel::createSendBundle()
{
	if(m_bundles.size() > 0)
	{
		Bundle* pBundle = m_bundles.back();
		Bundle::Packets& packets = pBundle->packets();

		if(pBundle->packetHaveSpace())
		{
			//先從隊列中刪除
			m_bundles.pop_back();
			pBundle->setChannel(this);
			//pBundle->setCurrMsgHandler(NULL);
			pBundle->setCurrMsgPacketCount(0);
			pBundle->setCurrMsgLength(0);
			pBundle->setCurrMsgLengthPos(0);
			return pBundle;
		}
	}

	Bundle* pBundle = Bundle::createPoolObject();
	pBundle->setChannel(this);
	return pBundle;
}

}
}