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
namespace sl{
namespace network{

Channel::Channel(NetworkInterface* networkInterface, const EndPoint* pEndPoint, ISLPacketParser* poPacketParser, ProtocolType pt, ChannelID id)
	:_channelType(CHANNEL_NORMAL),
	_flags(0),
	_protocolType(pt),
	_id(id),
	_lastReceivedTime(0),
	_bundles(),
	_bufferedReceives(),
				  
	_numPacketsSent(0),
	_numPacketsReceived(0),
	_numBytesSent(0),
	_numBytesReceived(0),
	_lastTickBytesReceived(0),
	_lastTickBytesSent(0),
				 
	_pNetworkInterface(networkInterface),
	_pSession(NULL),
	_pPacketParser(poPacketParser),

	_pEndPoint(NULL),
	_pPacketReader(NULL),
	_pPacketReceiver(NULL),
	_pPacketSender(NULL)
{
	this->clearBundle();
	this->setEndPoint(pEndPoint);

	SLASSERT(_pNetworkInterface != NULL, "wtf");
	SLASSERT(_pEndPoint != NULL, "wtf");

	if (_protocolType == PROTOCOL_TCP){
		if (_pPacketReceiver){
			if (_pPacketReceiver->type() == PacketReceiver::UDP_PACKET_RECEIVER){
				RELEASE_POOL_OBJECT(UDPPacketReceiver, (UDPPacketReceiver*)_pPacketReceiver);
				_pPacketReceiver = CREATE_POOL_OBJECT(TCPPacketReceiver, _pEndPoint, _pNetworkInterface);
			}
		}
		else{
			_pPacketReceiver = CREATE_POOL_OBJECT(TCPPacketReceiver, _pEndPoint, _pNetworkInterface);
		}

		SLASSERT(_pPacketReceiver->type() == PacketReceiver::TCP_PACKET_RECEIVER, "wtf");

		_pNetworkInterface->getDispatcher().registerReadFileDescriptor((int32)*_pEndPoint, _pPacketReceiver);
	}
	else{
		if (_pPacketReceiver){
			if (_pPacketReceiver->type() == PacketReceiver::TCP_PACKET_RECEIVER){
				RELEASE_POOL_OBJECT(TCPPacketReceiver, (TCPPacketReceiver*)_pPacketReceiver);
				_pPacketReceiver = CREATE_POOL_OBJECT(UDPPacketReceiver, _pEndPoint, _pNetworkInterface);
			}
		}
		else{
			_pPacketReceiver = CREATE_POOL_OBJECT(UDPPacketReceiver, _pEndPoint, _pNetworkInterface);;
		}
		SLASSERT(_pPacketReceiver->type() == PacketReceiver::UDP_PACKET_RECEIVER, "wtf");
	}

	_pPacketReceiver->SetEndPoint(_pEndPoint);

	if (_pPacketSender){
		_pPacketSender->SetEndPoint(_pEndPoint);
	}
}

Channel::~Channel(){
	if (!isDestroyed())
		finalise();
}

bool Channel::finalise(){
	this->clearState();

	if (_protocolType == PROTOCOL_TCP){
		RELEASE_POOL_OBJECT(TCPPacketReceiver, (TCPPacketReceiver*)_pPacketReceiver);
		RELEASE_POOL_OBJECT(TCPPacketSender, (TCPPacketSender*)_pPacketSender);
	}
	else{
		RELEASE_POOL_OBJECT(UDPPacketReceiver, (UDPPacketReceiver*)_pPacketReceiver);
	}
	RELEASE_POOL_OBJECT(PacketReader, _pPacketReader);

	_pPacketReceiver = NULL;
	_pPacketSender = NULL;
	_pPacketReader = NULL;
	return true;
}

void Channel::send(const char* pBuf, uint32 dwLen){
	Bundle* pBundle = createSendBundle();
	
	if (dwLen > (uint32)(pBundle->packetMaxSize())){
		SLASSERT(false, "wtf");
		condemn();
		return;
	}

	pBundle->newMessage();
	pBundle->append(pBuf, dwLen);
	send(pBundle);
}

void Channel::disconnect(){
	condemn();
}

void Channel::setEndPoint(const EndPoint* pEndPoint){
	if(_pEndPoint != pEndPoint){
		RELEASE_POOL_OBJECT(EndPoint, _pEndPoint);
		_pEndPoint = const_cast<EndPoint*>(pEndPoint);
	}

	_lastReceivedTime = getTimeMilliSecond();
}

void Channel::destroy(bool notify){
	if(isDestroyed()){
		return;
	}

	if (nullptr != _pSession && notify){
		_pSession->onTerminate();
	}

	finalise();
	
	_flags |= FLAG_DESTROYED;
}

void Channel::setConnected() { 
	if (isConnected()){
		return;
	}
	_flags |= FLAG_CONNECTED; 
}

void Channel::clearState(bool warnOnDiscard /* = false */){
	//清空未處理的接受包緩存
	if(_bufferedReceives.size() > 0){
		BufferedReceives::iterator iter = _bufferedReceives.begin();
		int hasDiscard = 0;

		for (;iter != _bufferedReceives.end(); ++iter){
			Packet* pPacket = (*iter);
			if(pPacket->length() > 0)
				hasDiscard++;

			RECLAIM_PACKET(pPacket->IsTCPPacket(), pPacket);
		}
		if(hasDiscard > 0 && warnOnDiscard){
			ECHO_TRACE("Channel::clearState(%s): Discarding %d buffered packet(s)",
				this->c_str(), hasDiscard);
		}

		_bufferedReceives.clear();
	}

	clearBundle();

	_lastReceivedTime = getTimeMilliSecond();

	_numPacketsSent = 0;
	_numPacketsReceived = 0;
	_numBytesSent = 0;
	_numBytesReceived = 0;
	_lastTickBytesReceived = 0;
	_lastTickBytesSent = 0;
	_channelType = CHANNEL_NORMAL;
	_id = CHANNEL_ID_NULL;
	_flags = 0;

	if(_pEndPoint && _protocolType == PROTOCOL_TCP && !this->isDestroyed()){
		this->stopSend();
		_pNetworkInterface->getDispatcher().deregisterReadFileDescriptor((int32)*_pEndPoint);
	}
	
	//由于endpoint通常由外部给入，必须释放，频道重新激活时重新赋值
	if(_pEndPoint){
		_pEndPoint->close();
		this->setEndPoint(NULL);
	}
}

Channel::Bundles& Channel::bundles(){
	return _bundles;
}

const Channel::Bundles & Channel::bundles() const{
	return _bundles;
}

int32 Channel::bundlesLength(){
	int32 len = 0;
	Bundles::iterator iter = _bundles.begin();
	for (; iter!= _bundles.end(); ++iter){
		len += (*iter)->packetsLength();
	}
	return len;

}

void Channel::delayedSend(){
//	this->getNetworkInterface().delayedSend(*this);
}

const char* Channel::c_str() const{
	static char dodgyString[MAX_BUF] = "None";
	char tdodgyString[MAX_BUF] = {0};

	if(_pEndPoint && !_pEndPoint->addr().isNone())
		_pEndPoint->addr().writeToString(tdodgyString, MAX_BUF);

	SafeSprintf(dodgyString, MAX_BUF, "%s/%d/%d/%d", tdodgyString, _id,
		this->isCondemn(), this->isDestroyed());
	
	return dodgyString;
}

void Channel::clearBundle(){
	Bundles::iterator iter = _bundles.begin();
	for (; iter != _bundles.end(); ++iter){
		RELEASE_POOL_OBJECT(Bundle, *iter);
	}

	_bundles.clear();
}

void Channel::send(Bundle* pBundle /* = NULL */){
	if(isDestroyed()){
		this->clearBundle();
		if(pBundle){
			RELEASE_POOL_OBJECT(Bundle, pBundle);
		}
		return;
	}

	if(isCondemn()){
		this->clearBundle();
		if(pBundle){
			RELEASE_POOL_OBJECT(Bundle, pBundle);
		}
		return;
	}

	if(pBundle){
		pBundle->setChannel(this);
		pBundle->finiMessage(true);
		_bundles.push_back(pBundle);
	}

	uint32 bundleSize = (uint32)_bundles.size();
	if(bundleSize == 0)
		return;

	if(!sending()){
		if(_pPacketSender == NULL){
			_pPacketSender = CREATE_POOL_OBJECT(TCPPacketSender, _pEndPoint, _pNetworkInterface);
		}

		_pPacketSender->processSend(this);

		//如果不能立即發送到系統緩衝區，那麼交給poller處理
		if(_bundles.size() > 0 && !isCondemn() && !isDestroyed()){
			_flags |= FLAG_SENDING;
			_pNetworkInterface->getDispatcher().registerWriteFileDescriptor((int32)(*_pEndPoint), _pPacketSender);
		}
	}
}

void Channel::stopSend(){
	if(!sending())
		return;

	_flags &= ~FLAG_SENDING;

	_pNetworkInterface->getDispatcher().deregisterWriteFileDescriptor((int32)*_pEndPoint);
}

void Channel::onSendCompleted(){
	SLASSERT(_bundles.size() == 0 && sending(), "wtf");
	stopSend();
}

void Channel::onPacketSent(int bytes, bool sendCompleted){
	if(sendCompleted){
		++_numPacketsSent;
		++g_numPacketsSent;
	}

	_numBytesSent += bytes;
	g_numBytesSent += bytes;
	_lastTickBytesSent += bytes;

}

void Channel::onPacketReceived(int bytes){
	_lastReceivedTime  = getTimeMilliSecond();
	++_numPacketsReceived;
	++g_numPacketsReceived;

	_numBytesReceived += bytes;
	_lastTickBytesReceived += bytes;
	g_numBytesReceived += bytes;

}

void Channel::addReceiveWindow(Packet* pPacket){
	_bufferedReceives.push_back(pPacket);
	uint32 size = (uint32)_bufferedReceives.size();
}

void Channel::condemn(){
	if(isCondemn())
		return;

	_flags |= FLAG_CONDEMN;
}

void Channel::processPackets(){
	_lastTickBytesReceived = 0;
	_lastTickBytesSent = 0;

	if(this->isDestroyed())
		return;

	if(this->isCondemn())
		return;

	if(_pPacketReader == nullptr)
		_pPacketReader = CREATE_POOL_OBJECT(PacketReader, this, _pPacketParser);
	SLASSERT(_pPacketReader, "wtf");

	BufferedReceives::iterator packetIter = _bufferedReceives.begin();
	for (; packetIter != _bufferedReceives.end(); ++packetIter){
		Packet* pPacket = (*packetIter);
		_pPacketReader->processMessages(pPacket);
		RECLAIM_PACKET(pPacket->IsTCPPacket(), pPacket);
	}

	_bufferedReceives.clear();
}

bool Channel::waitSend(){
	return getEndPoint()->waitSend();
}

Bundle* Channel::createSendBundle(){
	if(_bundles.size() > 0){
		Bundle* pBundle = _bundles.back();
		Bundle::Packets& packets = pBundle->packets();

		if(pBundle->packetHaveSpace()){
			//先從隊列中刪除
			_bundles.pop_back();
			pBundle->setChannel(this);
			return pBundle;
		}
	}

	Bundle* pBundle = CREATE_POOL_OBJECT(Bundle);
	pBundle->setChannel(this);
	return pBundle;
}

}
}