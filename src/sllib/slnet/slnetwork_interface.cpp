#include "slnetwork_interface.h"
#include "sllistener_receiver.h"
#include "sltcp_packet_receiver.h"
#include "slevent_dispatcher.h"
#include "slnet.h"
#include "slnet_module.h"
#include "sltcp_packet_sender.h"
namespace sl{
namespace network{
NetworkInterface::NetworkInterface(EventDispatcher* pEventDispatcher)
		:_channelMap(),
		 _pDispatcher(pEventDispatcher),
		 _pDelayedChannels(NULL),
		 _pChannelTimeOutHandler(NULL),
		 _pChannelDeregisterHandler(NULL),
		 _numExtChannels(0),
		 _lastCheckDestroyChannelTime(getTimeMilliSecond())
{}

NetworkInterface::~NetworkInterface(){
	ChannelMap::iterator iter = _channelMap.begin();

	while(iter != _channelMap.end()){
		ChannelMap::iterator olditer = iter++;
		Channel* pChannel = olditer->second;
		pChannel->destroy(false);
		pChannel->release();
	}

	_channelMap.clear();

	if(_pDispatcher != NULL){
		_pDispatcher = NULL;
	}
	_numExtChannels = 0;
}

bool NetworkInterface::createListeningSocket(const char* listeningInterface, uint16 listeningPort, 
											   EndPoint* pEP, ListenerReceiver* pLR, uint32 rbuffer /* = 0 */, 
											   uint32 wbuffer /* = 0 */)
{
	SLASSERT(listeningInterface && pLR && _pDispatcher, "wtf");
	
	if(pEP->good()){
		_pDispatcher->deregisterReadFileDescriptor((int32)*pEP);
		pEP->close();
	}

	Address address;
	address._ip = 0;
	address._port = 0;

	pEP->socket(SOCK_STREAM);
	if (!pEP->good()){
		return false;
	}

	_pDispatcher->registerReadFileDescriptor((int32)*pEP, pLR);
	uint32 ifIPAddr = INADDR_ANY;

	//查找指定接口名 NIP、MAC、IP是否可用
	if(pEP->findIndicatedInterface(listeningInterface, ifIPAddr) == 0){
		char szIp[MAX_IP] = {0};
		Address::ip2string(ifIPAddr, szIp);
	}

	//尝试绑定到端口，如果被占用向后递增
	if(pEP->bind(htons(listeningPort), ifIPAddr) != 0){
		pEP->close();
		pEP->release();
		return false;
	}


	//获得当前绑定的地址，如果是INADDR_ANY这里获得的IP是0
	pEP->getlocaladdress((uint16*)&address._port,(uint32*)&address._ip);

	if(0 == address._ip){
		uint32 addr;
		if(0 == pEP->getDefaultInterfaceAddress(addr)){
			address._ip = addr;
		}
		else{
			pEP->close();
			return false;
		}
	}

	pEP->setnonblocking(true);
	pEP->setnodelay(true);
	pEP->addr(address);

	if(rbuffer > 0){
		if(!pEP->setBufferSize(SO_RCVBUF, rbuffer)){
			SLASSERT(false, "wtf");
		}
	}
	if(wbuffer > 0){
		if(!pEP->setBufferSize(SO_SNDBUF, wbuffer)){
			SLASSERT(false, "wtf");
		}
	}

	int backlog = network::g_SOMAXCONN;
	if(backlog < 5)
		backlog = 5;

	if(pEP->listen(backlog) == -1){
		pEP->close();
		return false;
	}

	return true;
}

bool NetworkInterface::createConnectingSocket(const char* serverIp, uint16 serverPort, ISLSession* pSession,
											  ISLPacketParser* poPacketParser, uint32 rbuffer /*= 0*/, uint32 wbuffer /*= 0*/)
{
	
	EndPoint* pSvrEndPoint = EndPoint::create(0, 0);
	SLASSERT(pSvrEndPoint, "wtf");

	pSvrEndPoint->socket(SOCK_STREAM);
	if (!pSvrEndPoint->good()){
		return false;
	}
	
	uint32 address;
	Address::string2ip(serverIp, address);
	int32 ret = 0;
	if ((ret = pSvrEndPoint->connect(htons(serverPort), address)) == -1){
		int32 error = WSAGetLastError();
		if (error != WSAEWOULDBLOCK){
			//SLASSERT(false, "wtf");
			pSvrEndPoint->close();
			return false;
		}
	}
	
	Address addr(serverIp, serverPort);
	pSvrEndPoint->addr(addr);
	Channel* pSvrChannel = Channel::create(this, pSvrEndPoint, poPacketParser);
	if(!pSvrChannel){
		SLASSERT(false, "wtf");
		return false;
	}
	

	if(!this->registerChannel(pSvrChannel)){
		pSvrChannel->destroy();
		pSvrChannel->release();
		return false;
	}

	pSvrChannel->setSession(pSession);
	pSession->setChannel(pSvrChannel);

	if(pSvrChannel->getPacketSender() == nullptr){
		TCPPacketSender* pPackerSender = CREATE_POOL_OBJECT(TCPPacketSender, pSvrEndPoint, this);
		pSvrChannel->setPacketSender(pPackerSender);
		//getDispatcher().registerWriteFileDescriptor((int32)(*pSvrEndPoint), pPackerSender);
	}

	if (ret != -1){
		ISLSession* poSession = pSvrChannel->getSession();
		poSession->onEstablish();
		pSvrChannel->setConnected();
	}
	else{
		getDispatcher().registerWriteFileDescriptor((int32)(*pSvrEndPoint), pSvrChannel->getPacketSender());
	}

	return true;
}

Channel* NetworkInterface::findChannel(const Address& addr){
	if(addr._ip == 0)
		return NULL;

	ChannelMap::iterator iter = _channelMap.find(addr);
	Channel* pChannel = (iter != _channelMap.end()) ? iter->second : NULL;
	return pChannel;
}

Channel* NetworkInterface::findChannel(int fd){
	ChannelMap::iterator iter = _channelMap.begin();
	for (; iter != _channelMap.end(); ++iter){
		if(iter->second->getEndPoint() && *iter->second->getEndPoint() == fd)
			return iter->second;
	}

	return NULL;
}

bool NetworkInterface::registerChannel(Channel* pChannel){
	const Address& addr = pChannel->addr();
	SLASSERT(addr._ip != 0, "wtf");
	ChannelMap::iterator iter = _channelMap.find(addr);
	Channel* pExisting = iter != _channelMap.end() ? iter->second : NULL;

	if(pExisting){
		return false;
	}
	_channelMap[addr] = pChannel;
	_numExtChannels++;

	return true;
}

bool NetworkInterface::deregisterAllChannels(){
	ChannelMap::iterator iter = _channelMap.begin();
	while(iter != _channelMap.end()){
		ChannelMap::iterator oldIter = iter++;
		Channel* pChannel = oldIter->second;
		pChannel->destroy();
		pChannel->release();
	}

	_channelMap.clear();
	_numExtChannels = 0;

	return true;
}

bool NetworkInterface::deregisterChannel(Channel* pChannel){
	const Address& addr = pChannel->addr();
	SLASSERT(pChannel->getEndPoint() != NULL, "wtf");

	_numExtChannels--;

	if(_channelMap.erase(addr)){
		return false;
	}

	if(_pChannelDeregisterHandler){
		//m_pChannelDeregisterHandler->
	}
	return true;
}

bool NetworkInterface::deregisterSocket(int32 fd){
	bool ret1 = this->getDispatcher().deregisterReadFileDescriptor(fd);
	bool ret2 = this->getDispatcher().deregisterWriteFileDescriptor(fd);
	return ret1 || ret2; 
}

void NetworkInterface::onChannelTimeOut(Channel* pChannel){
	if(_pChannelTimeOutHandler){
		//m_pChannelTimeOutHandler->
	}
	else{

	}
}

int32 NetworkInterface::numExtChannels() const{
	return _numExtChannels;
}

int32 NetworkInterface::checkDestroyChannel(){
	if (getTimeMilliSecond() - _lastCheckDestroyChannelTime <= CHECK_DESTROY_CHANNEL_TIME)
		return 0;

	ChannelMap::iterator iter = _channelMap.begin();
	ChannelMap::iterator iterEnd = _channelMap.end();

	int32 destroyCount = 0;
	while(iter != iterEnd){
		Channel* pChannel = iter->second;
		if (nullptr == pChannel || pChannel->isDestroyed()){
			pChannel->release();
			iter = _channelMap.erase(iter);
			destroyCount++;
		}
		else if (pChannel->isCondemn()){
			pChannel->destroy();
			pChannel->release();
			iter = _channelMap.erase(iter);
			destroyCount++;
		}
		else{
			++iter;
		}
	}

	_lastCheckDestroyChannelTime = getTimeMilliSecond();

	return destroyCount;
}

}
}