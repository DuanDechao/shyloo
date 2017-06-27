#include "slchannel.h"
#include "slpacket_receiver.h"
#include "sltcp_packet_receiver.h"
#include "sludp_packet_receiver.h"
#include "slnetwork_interface.h"
#include "slevent_dispatcher.h"
#include "sladdress.h"
#include "sltcp_packet_sender.h"
namespace sl{
namespace network{

sl::SLPool<Channel>  Channel::s_pool;
Channel::Channel(NetworkInterface* networkInterface, const EndPoint* pEndPoint, ISLPacketParser* poPacketParser, ProtocolType pt, ChannelID id)
	:_flags(0),
	_protocolType(pt),
	_id(id),
	_lastReceivedTime(0),
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
	_pPacketReceiver(NULL),
	_pPacketSender(NULL)
{
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

	_recvBuf = (sl::SLRingBuffer*)SLMALLOC(102400);
	_recvBuf->init(102400, true);

	_sendBuf = (sl::SLRingBuffer*)SLMALLOC(102400);
	_sendBuf->init(102400, true);
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

	_pPacketReceiver = NULL;
	_pPacketSender = NULL;
	return true;
}

void Channel::send(const char* pBuf, uint32 dwLen){
	const int32 freeLen = _sendBuf->getFreeSize();
	if (dwLen > (uint32)(freeLen)){
		SLASSERT(false, "wtf");
		condemn();
		return;
	}
	
	if (isDestroyed() || isCondemn()){
		return;
	}

	_sendBuf->put(pBuf, dwLen);

	if (!sending()){
		if (_pPacketSender == NULL){
			_pPacketSender = CREATE_POOL_OBJECT(TCPPacketSender, _pEndPoint, _pNetworkInterface);
		}

		_pPacketSender->processSend(this);

		//如果不能立即發送到系統緩衝區，那麼交給poller處理
		if (_sendBuf->getDataSize() > 0 && !isCondemn() && !isDestroyed()){
			_flags |= FLAG_SENDING;
			_pNetworkInterface->getDispatcher().registerWriteFileDescriptor((int32)(*_pEndPoint), _pPacketSender);
		}
	}
}

void Channel::disconnect(){
	condemn();
}

void Channel::setEndPoint(const EndPoint* pEndPoint){
	if (_pEndPoint != pEndPoint && _pEndPoint){
		_pEndPoint->release();
	}

	_pEndPoint = const_cast<EndPoint*>(pEndPoint);
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
	/*if(_bufferedReceives.size() > 0){
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

	clearBundle();*/

	_lastReceivedTime = getTimeMilliSecond();

	_numPacketsSent = 0;
	_numPacketsReceived = 0;
	_numBytesSent = 0;
	_numBytesReceived = 0;
	_lastTickBytesReceived = 0;
	_lastTickBytesSent = 0;
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

void Channel::stopSend(){
	if(!sending())
		return;

	_flags &= ~FLAG_SENDING;

	_pNetworkInterface->getDispatcher().deregisterWriteFileDescriptor((int32)*_pEndPoint);
}

void Channel::onSendCompleted(){
	if (_sendBuf->getDataSize() == 0 && sending())
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

	char temp[40960];
	const int32 dataSize = _recvBuf->getDataSize();
	int32 dataLen = dataSize > 40960 ? 40960 : dataSize;
	const char* data = _recvBuf->get(temp, dataLen, true);
	if (data == nullptr)
		return;

	int32 used = 0;
	int32 totalUsed = 0;
	do{
		used = _pPacketParser->parsePacket(data + totalUsed, dataLen - totalUsed);
		if (used > 0){
			SLASSERT(totalUsed + used <= dataLen, "wtf");
			_pSession->onRecv(data + totalUsed, used);
			totalUsed += used;
		}
	} while (used > 0 && totalUsed < dataLen);

	if (used >= 0){
		if (totalUsed > 0)
			_recvBuf->readOut(totalUsed);
	}
	else{
		this->condemn();
	}
}

bool Channel::waitSend(){
	return getEndPoint()->waitSend();
}


int32 Channel::recvFromEndPoint(){
	int32 recvSize = 0;
	char* recvBuf = _recvBuf->writePtr(recvSize);
	if (recvSize <= 0){
		SLASSERT(false, "wtf");
		return -1;
	}

	int32 len = _pEndPoint->recv(recvBuf, recvSize);
	if (len > 0)
		_recvBuf->writeIn(len);

	return len;
}

int32 Channel::sendToEndPoint(){
	const int32 dataSize = _sendBuf->getDataSize();
	char* tmpBuf = (char*)alloca(dataSize);
	const char* data = _sendBuf->get(tmpBuf, dataSize, true);
	
	int32 len = _pEndPoint->send(data, dataSize);
	if (len > 0){
		_sendBuf->readOut(len);
	}

	return len;
}

}
}