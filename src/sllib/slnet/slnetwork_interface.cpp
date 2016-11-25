#include "slnetwork_interface.h"
#include "sllistener_receiver.h"
#include "slevent_dispatcher.h"
#include "slnet.h"
namespace sl
{
namespace network
{
NetworkInterface::NetworkInterface(EventDispatcher* pDispatcher, 
		int32 extlisteningPort_min /* = -1 */, int32 extlisteningPort_max /* = -1 */, const char* extlisteningInterface/* ="" */, 
		uint32 extrbuffer /* = 0 */, uint32 extwbuffer /* = 0 */, 
		int32 intlisteningPort /* = 0 */, const char* intlisteningInterface /* = "" */, 
		uint32 intrbuffer /* = 0 */, uint32 intwbuffer /* = 0 */)
		:m_extEndPoint(),
		 m_intEndPoint(),
		 m_channelMap(),
		 m_pDispatcher(pDispatcher),
		 m_pExtensionData(NULL),
		 m_pExtListenerReceiver(NULL),
		 m_pIntListenerReceiver(NULL),
//		 m_pDelayedChannels(new DelayedChannels()),
		 m_pChannelTimeOutHandler(NULL),
		 m_pChannelDeregisterHandler(NULL),
		 m_isExternal(extlisteningPort_min != -1),
		 m_numExtChannels(0),
		 m_pSessionFactory(NULL)
{
	if(isExternal())
	{
		//m_pExtListenerReceiver = new ListenerReceiver()
		m_pExtListenerReceiver = new ListenerReceiver(m_extEndPoint, Channel::EXTERNAL, *this);
		this->recreateListeningSocket("EXTERNAL", htons(extlisteningPort_min), htons(extlisteningPort_max),
			extlisteningInterface, &m_extEndPoint, m_pExtListenerReceiver, extrbuffer, extwbuffer);

		//如果配置了对外端口范围，如果范围过小这里extEndpoint可能没有端口可用了
		if(extlisteningPort_min != -1)
		{
			SL_ASSERT(m_extEndPoint.good());
		}
	}

	if(intlisteningPort != -1)
	{
		m_pIntListenerReceiver = new ListenerReceiver(m_intEndPoint, Channel::INTERNAL, *this);
		this->recreateListeningSocket("INTERNAL", intlisteningPort, intlisteningPort, intlisteningInterface,
			&m_intEndPoint, m_pIntListenerReceiver, intrbuffer, intwbuffer);
	}
}

NetworkInterface::~NetworkInterface()
{
	ChannelMap::iterator iter = m_channelMap.begin();

	while(iter != m_channelMap.end())
	{
		ChannelMap::iterator olditer = iter++;
		Channel* pChannel = olditer->second;
		pChannel->destroy();
		delete pChannel;
	}

	m_channelMap.clear();

	this->closeSocket();

	if(m_pDispatcher != NULL)
	{
		m_pDispatcher = NULL;
	}

	//SAFE_RELEASE();
	SAFE_RELEASE(m_pExtListenerReceiver);
	SAFE_RELEASE(m_pIntListenerReceiver);
}

void NetworkInterface::stop()
{
	this->closeSocket();
}

void NetworkInterface::closeSocket()
{
	if(m_extEndPoint.good())
	{
		this->getDispatcher().deregisterReadFileDescriptor(m_extEndPoint);
		m_extEndPoint.close();
	}
	if(m_intEndPoint.good())
	{
		this->getDispatcher().deregisterReadFileDescriptor(m_intEndPoint);
		m_intEndPoint.close();
	}
}

bool NetworkInterface::recreateListeningSocket(const char* pEndPointName, uint16 listeningPort_min, 
											   uint16 listeningPort_max, const char* listeningInterface, 
											   EndPoint* pEP, ListenerReceiver* pLR, uint32 rbuffer /* = 0 */, 
											   uint32 wbuffer /* = 0 */)
{
	SL_ASSERT(listeningInterface && pEP && pLR);

	if(pEP->good())
	{
		this->getDispatcher().deregisterReadFileDescriptor(*pEP);
		pEP->close();
	}

	Address address;
	address.m_ip = 0;
	address.m_port = 0;

	pEP->socket(SOCK_STREAM);
	if(!pEP->good())
	{
		return false;
	}

	this->getDispatcher().registerReadFileDescriptor(*pEP, pLR);
	uint32 ifIPAddr = INADDR_ANY;
	bool listeningInterfaceEmpty = (listeningInterface == NULL || listeningInterface[0] == 0);

	//查找指定接口名 NIP、MAC、IP是否可用
	if(pEP->findIndicatedInterface(listeningInterface, ifIPAddr) == 0)
	{
		char szIp[MAX_IP] = {0};
		Address::ip2string(ifIPAddr, szIp);
	}

	//如果不为空又找不到那么警告用户错误的设置
	else if(!listeningInterfaceEmpty)
	{
		//warning
	}

	//尝试绑定到端口，如果被占用向后递增
	bool foundport = false;
	uint32 listeningPort = listeningPort_min;
	if(listeningPort_min != listeningPort_max)
	{
		for (int IpIdx = ntohs(listeningPort_min); IpIdx <= ntohs(listeningPort_max); ++IpIdx)
		{
			listeningPort = htons(IpIdx);
			if(pEP->bind(listeningPort, ifIPAddr) != 0)
			{
				continue;
			}
			else
			{
				foundport = true;
				break;
			}
		}
	}
	else
	{
		if(pEP->bind(listeningPort, ifIPAddr) == 0)
		{
			foundport = true;
		}
	}

	//如果无法绑定到合适的端口那么报错返回，进程将推出
	if(!foundport)
	{
		pEP->close();
		return false;
	}

	//获得当前绑定的地址，如果是INADDR_ANY这里获得的IP是0
	pEP->getlocaladdress((uint16*)&address.m_port,
		(uint32*)&address.m_ip);

	if(0 == address.m_ip)
	{
		uint32 addr;
		if(0 == pEP->getDefaultInterfaceAddress(addr))
		{
			address.m_ip = addr;

			char szIp[MAX_IP] = {0};
		}
		else
		{
			pEP->close();
			return false;
		}
	}

	pEP->setnonblocking(true);
	pEP->setnodelay(true);
	pEP->addr(address);

	if(rbuffer > 0)
	{
		if(!pEP->setBufferSize(SO_RCVBUF, rbuffer))
		{

		}
	}
	if(wbuffer > 0)
	{
		if(!pEP->setBufferSize(SO_SNDBUF, wbuffer))
		{

		}
	}

	int backlog = network::g_SOMAXCONN;
	if(backlog < 5)
		backlog = 5;

	if(pEP->listen(backlog) == -1)
	{
		pEP->close();
		return false;
	}

	return true;
}

Channel* NetworkInterface::findChannel(const Address& addr)
{
	if(addr.m_ip == 0)
		return NULL;

	ChannelMap::iterator iter = m_channelMap.find(addr);
	Channel* pChannel = (iter != m_channelMap.end()) ? iter->second : NULL;
	return pChannel;
}

Channel* NetworkInterface::findChannel(int fd)
{
	ChannelMap::iterator iter = m_channelMap.begin();
	for (; iter != m_channelMap.end(); ++iter)
	{
		if(iter->second->getEndPoint() && *iter->second->getEndPoint() == fd)
			return iter->second;
	}

	return NULL;
}

bool NetworkInterface::registerChannel(Channel* pChannel)
{
	const Address& addr = pChannel->addr();
	SL_ASSERT(addr.m_ip != 0);
	SL_ASSERT(&pChannel->getNetworkInterface() == this);
	ChannelMap::iterator iter = m_channelMap.find(addr);
	Channel* pExisting = iter != m_channelMap.end() ? iter->second : NULL;

	if(pExisting)
	{
		return false;
	}
	m_channelMap[addr] = pChannel;
	if(pChannel->isExternal())
		m_numExtChannels++;

	if(m_pSessionFactory == NULL){
		SL_ERROR("network inferface have no sessionfactory");
		return false;
	}

	ISLSession* poSession = m_pSessionFactory->createSession(pChannel);
	if(NULL == poSession)
	{
		SL_ERROR("create session failed");
		deregisterChannel(pChannel);
		return false;
	}

	pChannel->setSession(poSession);

	return true;
}

bool NetworkInterface::deregisterAllChannels()
{
	ChannelMap::iterator iter = m_channelMap.begin();
	while(iter != m_channelMap.end())
	{
		ChannelMap::iterator oldIter = iter++;
		Channel* pChannel = oldIter->second;
		pChannel->destroy();
		network::Channel::reclaimPoolObject(pChannel);
	}

	m_channelMap.clear();
	m_numExtChannels = 0;

	return true;
}

bool NetworkInterface::deregisterChannel(Channel* pChannel)
{
	const Address& addr = pChannel->addr();
	SL_ASSERT(pChannel->getEndPoint() != NULL);

	if(pChannel->isExternal())
		m_numExtChannels--;

	if(m_channelMap.erase(addr))
	{
		return false;
	}
	if(m_pChannelDeregisterHandler)
	{
		//m_pChannelDeregisterHandler->
	}
	return true;
}

void NetworkInterface::onChannelTimeOut(Channel* pChannel)
{
	if(m_pChannelTimeOutHandler)
	{
		//m_pChannelTimeOutHandler->
	}
	else
	{

	}
}

void NetworkInterface::processChannels(/*MessageHandlers* pMsgHandlers*/)
{
	ChannelMap::iterator iter = m_channelMap.begin();
	for (; iter != m_channelMap.end();)
	{
		network::Channel* pChannel = iter->second;

		if(pChannel->isDestroyed())
		{
			++iter;
		}
		else if(pChannel->isCondemn())
		{
			++iter;
			deregisterChannel(pChannel);
			pChannel->destroy();
			network::Channel::reclaimPoolObject(pChannel);
		}
		else
		{
			pChannel->processPackets(/*pMsgHandlers*/);
			++iter;
		}
	}
}

const Address& NetworkInterface::extaddr() const
{
	return m_extEndPoint.addr();
}

const Address& NetworkInterface::intaddr() const
{
	return m_intEndPoint.addr();
}

int32 NetworkInterface::numExtChannels() const
{
	return m_numExtChannels;
}


}
}