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
Channel::Channel(NetworkInterface* networkInterface, const EndPoint* pEndPoint, ISLPacketParser* poPacketParser, const int32 recvSize, const int32 sendSize, ProtocolType pt)
	:_protocolType(pt),
	_lastReceivedTime(0),
	_flags(FLAG_NONE),
	_recvBuf(nullptr),
	_sendBuf(nullptr),
	_recvSize(recvSize),
    _sendSize(sendSize),

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
				_pPacketReceiver->release();
				_pPacketReceiver = TCPPacketReceiver::create(this, _pNetworkInterface);
			}
		}
		else{
			_pPacketReceiver = TCPPacketReceiver::create(this, _pNetworkInterface);
		}

		SLASSERT(_pPacketReceiver->type() == PacketReceiver::TCP_PACKET_RECEIVER, "wtf");
		_pNetworkInterface->getDispatcher().registerReadFileDescriptor((int32)*_pEndPoint, _pPacketReceiver);
	}
	else{
		if (_pPacketReceiver){
			if (_pPacketReceiver->type() == PacketReceiver::TCP_PACKET_RECEIVER){
				_pPacketReceiver->release();
				_pPacketReceiver = UDPPacketReceiver::create(this, _pNetworkInterface);
			}
		}
		else{
			_pPacketReceiver = UDPPacketReceiver::create(this, _pNetworkInterface);
		}
		SLASSERT(_pPacketReceiver->type() == PacketReceiver::UDP_PACKET_RECEIVER, "wtf");
	}

	_recvBuf = (sl::SLRingBuffer*)SLMALLOC(_recvSize);
	SLASSERT(_recvBuf, "wtf");
	_recvBuf->init(_recvSize, true);

	_sendBuf = (sl::SLRingBuffer*)SLMALLOC(_sendSize);
	SLASSERT(_sendBuf, "wtf");
	_sendBuf->init(_sendSize, true);
}

Channel::~Channel(){
	if (!isDestroyed())
		finalise();

	if (_recvBuf)
		SLFREE(_recvBuf);

	if (_sendBuf)
		SLFREE(_sendBuf);

	_recvBuf = nullptr;
	_sendBuf = nullptr;

	if (_pPacketReceiver)
	_pPacketReceiver->release();

	if (_pPacketSender)
	_pPacketSender->release();

	_pPacketReceiver = NULL;
	_pPacketSender = NULL;
}

bool Channel::finalise(){
	this->clearState();
	return true;
}

void Channel::send(const char* pBuf, uint32 dwLen){
	const int32 freeLen = _sendBuf->getFreeSize();
	if (dwLen > (uint32)(freeLen)){
		SLASSERT(false, "wtf");
		destroy();
		return;
	}
	
	if (isDestroyed()){
		return;
	}

	_sendBuf->put(pBuf, dwLen);

	if (!sending()){
		if (_pPacketSender == NULL){
			_pPacketSender = TCPPacketSender::create(this, _pNetworkInterface);
		}

		_pPacketSender->processSend(this);

		//如果不能立即發送到系統緩衝區，那麼交給poller處理
		if (_sendBuf->getDataSize() > 0 && !isDestroyed()){
			_flags |= FLAG_SENDING;
			_pNetworkInterface->getDispatcher().registerWriteFileDescriptor((int32)(*getEndPoint()), _pPacketSender);
		}
	}
}

void Channel::adjustSendBuffSize(const int32 size){
	sl::SLRingBuffer* newBuf = adjustNewBuffSize(_sendBuf, size);
	if (newBuf){
		_sendBuf = newBuf;
		_sendSize = size;
	}
}

void Channel::adjustRecvBuffSize(const int32 size){
	sl::SLRingBuffer* newBuf = adjustNewBuffSize(_recvBuf, size);
	if (newBuf){
		_recvBuf = newBuf;
		_recvSize = size;
	}
}

sl::SLRingBuffer* Channel::adjustNewBuffSize(SLRingBuffer* buf, const int32 newSize){
	int32 dataSize = buf->getDataSize();
	if (dataSize > newSize)
		return nullptr;

	sl::SLRingBuffer* newBuff = (sl::SLRingBuffer*)SLMALLOC(newSize);
	SLASSERT(newBuff, "wtf");
	newBuff->init(newSize, true);

	if (dataSize > 0){
		char* temp = (char*)alloca(dataSize);
		const char* data = buf->get(temp, dataSize, true);
		if (!data){
			SLASSERT(false, "wtf");
			SLFREE(newBuff);
			return nullptr;
		}
		newBuff->put(data, dataSize);
	}

	return newBuff;
}

void Channel::disconnect(){
	destroy();
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
	_pSession->release();

	_pNetworkInterface->deregisterChannel(this);
	finalise();

	_flags |= FLAG_DESTROYED;
	//release();
}

void Channel::setConnected() { 
	if (isConnected()){
		return;
	}
	_flags |= FLAG_CONNECTED; 
}

void Channel::clearState(bool warnOnDiscard /* = false */){
	_lastReceivedTime = getTimeMilliSecond();

	_numPacketsSent = 0;
	_numPacketsReceived = 0;
	_numBytesSent = 0;
	_numBytesReceived = 0;
	_lastTickBytesReceived = 0;
	_lastTickBytesSent = 0;
	_flags = 0;

	if(_pEndPoint && _protocolType == PROTOCOL_TCP && !this->isDestroyed()){
		this->stopSend();
		_pNetworkInterface->getDispatcher().deregisterReadFileDescriptor((int32)*_pEndPoint);
	}
	
	//由于endpoint通常由外部给入，必须释放，频道重新激活时重新赋值
	if(_pEndPoint){
		_pEndPoint->closeEndPoint();
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

	SafeSprintf(dodgyString, MAX_BUF, "%s/%d", tdodgyString, this->isDestroyed());
	
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

void Channel::processPackets(){
	_lastTickBytesReceived = 0;
	_lastTickBytesSent = 0;

	if(this->isDestroyed())
		return;
	
	const int32 dataSize = _recvBuf->getDataSize();
	if (dataSize <= 0)
		return;

	char* temp = (char*)alloca(dataSize);
	const char* data = _recvBuf->get(temp, dataSize, true);
	if (data == nullptr){
		SLASSERT(false, "wtf");
		return;
	}

	int32 used = 0;
	int32 totalUsed = 0;
	do{
		used = _pPacketParser->parsePacket(data + totalUsed, dataSize - totalUsed);
		if (used > 0){
			SLASSERT(totalUsed + used <= dataSize, "wtf");
			_pSession->onRecv(data + totalUsed, used);
			totalUsed += used;
		}
	} while (used > 0 && totalUsed < dataSize);

	if (used >= 0){
		if (totalUsed > 0)
			_recvBuf->readOut(totalUsed);
	}
	else{
		this->destroy();
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
