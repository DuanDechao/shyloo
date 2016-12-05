#include "slnetwork_interface.h"
#include "sllistener_receiver.h"
#include "sltcp_packet_receiver.h"
#include "slevent_dispatcher.h"
#include "slnet.h"
#include "slnet_module.h"
namespace sl
{
namespace network
{
NetworkInterface::NetworkInterface(EventDispatcher* pEventDispatcher)
		:m_channelMap(),
		 m_pDispatcher(pEventDispatcher),
		 m_pDelayedChannels(NULL),
		 m_pChannelTimeOutHandler(NULL),
		 m_pChannelDeregisterHandler(NULL),
		 m_numExtChannels(0)
{
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

	if(m_pDispatcher != NULL)
	{
		m_pDispatcher = NULL;
	}
	m_numExtChannels = 0;
}

bool NetworkInterface::createListeningSocket(const char* listeningInterface, uint16 listeningPort, 
											   EndPoint* pEP, ListenerReceiver* pLR, uint32 rbuffer /* = 0 */, 
											   uint32 wbuffer /* = 0 */)
{
	SLASSERT(listeningInterface && pLR && m_pDispatcher, "wtf");
	
	if(pEP->good())
	{
		m_pDispatcher->deregisterReadFileDescriptor((int32)*pEP);
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

	m_pDispatcher->registerReadFileDescriptor((int32)*pEP, pLR);
	uint32 ifIPAddr = INADDR_ANY;

	//查找指定接口名 NIP、MAC、IP是否可用
	if(pEP->findIndicatedInterface(listeningInterface, ifIPAddr) == 0)
	{
		char szIp[MAX_IP] = {0};
		Address::ip2string(ifIPAddr, szIp);
	}

	//尝试绑定到端口，如果被占用向后递增
	if(pEP->bind(htons(listeningPort), ifIPAddr) != 0)
	{
		pEP->close();
		EndPoint::reclaimPoolObject(pEP);
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
			SLASSERT(false, "wtf");
		}
	}
	if(wbuffer > 0)
	{
		if(!pEP->setBufferSize(SO_SNDBUF, wbuffer))
		{
			SLASSERT(false, "wtf");
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

bool NetworkInterface::createConnectingSocket(const char* serverIp, uint16 serverPort,  EndPoint* pEP, ISLSession* pSession, uint32 rbuffer /*= 0*/, uint32 wbuffer /*= 0*/)
{
	SLASSERT(pEP, "wtf");
	if(pEP->good())
	{
		this->getDispatcher().deregisterReadFileDescriptor((int32)*pEP);
		pEP->close();
	}
	pEP->socket(SOCK_STREAM);
	if(!pEP->good())
	{
		return false;
	}
	
	uint32 address;
	Address::string2ip(serverIp, address);
	int32 ret = 0;
	if((ret = pEP->connect(htons(serverPort), address)) == -1)
	{
		pEP->close();
		return false;
	}

	Address addr(serverIp, serverPort);
	pEP->addr(addr);
	Channel* pSvrChannel = Channel::createPoolObject();
	SLASSERT(pSvrChannel, "w");
	if(!pSvrChannel->initialize(*this, pEP, Channel::Traits::EXTERNAL))
	{
		SLASSERT(false, "wtf");
		return false;
	}
	

	if(!this->registerChannel(pSvrChannel))
	{
		pSvrChannel->destroy();
		Channel::reclaimPoolObject(pSvrChannel);
		return false;
	}

	if(!pSvrChannel->isConnected()){
		pSvrChannel->setSession(pSession);
		pSession->setChannel(pSvrChannel);
		pSession->onEstablish();
		pSvrChannel->setConnected(true);
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
	SLASSERT(addr.m_ip != 0, "wtf");
//	SLASSERT(&pChannel->getNetworkInterface() == this, "wtf");
	ChannelMap::iterator iter = m_channelMap.find(addr);
	Channel* pExisting = iter != m_channelMap.end() ? iter->second : NULL;

	if(pExisting)
	{
		return false;
	}
	m_channelMap[addr] = pChannel;
	if(pChannel->isExternal())
		m_numExtChannels++;

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
	SLASSERT(pChannel->getEndPoint() != NULL, "wtf");

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

bool NetworkInterface::deregisterSocket(int32 fd)
{
	bool ret1 = this->getDispatcher().deregisterReadFileDescriptor(fd);
	bool ret2 = this->getDispatcher().deregisterWriteFileDescriptor(fd);
	return ret1 || ret2; 
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

int32 NetworkInterface::numExtChannels() const
{
	return m_numExtChannels;
}

//void NetworkInterface::processChannels()
//{
//	ChannelMap::iterator iter = m_channelMap.begin();
//	for(; iter != m_channelMap.end(); )
//	{
//		Channel* pChannel = iter->second;
//
//		if(pChannel->isDestroyed())
//		{
//			++iter;
//		}
//		else if(pChannel->isCondemn())
//		{
//			++iter;
//
//			deregisterChannel(pChannel);
//			pChannel->destroy();
//			Channel::reclaimPoolObject(pChannel);
//		}
//		else
//		{
//			pChannel->processPackets();
//			++iter;
//		}
//	}
//}

}
}