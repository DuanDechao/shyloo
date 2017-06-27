#include "slconnector.h"
#include "slnet_module.h"
#include "sladdress.h"
#include "sltcp_packet_sender.h"

namespace sl
{
namespace network
{
CSLConnector::CSLConnector()
	:_pSession(nullptr),
	 _dwRecvBufSize(0),
	 _dwSendBufSize(0),
	 _wPort(0)
{
	_pNetworkInterface = CSLNetModule::getSingletonPtr()->getNetworkInterface();
}

CSLConnector::~CSLConnector(){
	_pSession = NULL;
	_pNetworkInterface = NULL;
	_pPacketParser = NULL;
	_dwRecvBufSize = 0;
	_dwSendBufSize = 0;
	_wPort = 0;
}

void CSLConnector::setSession(ISLSession* pSession){
	_pSession = pSession;
}

void CSLConnector::setBufferSize(uint32 dwRecvBufSize, uint32 dwSendBufSize){
	_dwRecvBufSize = dwRecvBufSize;
	_dwSendBufSize = dwSendBufSize;
}

bool CSLConnector::connect(const char* pszIP, uint16 wPort){
	if (nullptr == _pSession || nullptr == _pNetworkInterface || _pPacketParser == nullptr
		|| _dwRecvBufSize <= 0 || _dwSendBufSize <= 0){
		SLASSERT(false, "wtf");
		return false;
	}
	//SafeSprintf(_pszIP, sizeof(_pszIP), "%s", pszIP);
	_wPort = wPort;
	return _pNetworkInterface->createConnectingSocket(pszIP, wPort, _pSession, _pPacketParser, _dwRecvBufSize, _dwSendBufSize);
}

bool CSLConnector::reConnect(){
	if (_pszIP == nullptr || _wPort == 0)
		return false;

	return connect(_pszIP, _wPort);
}
void CSLConnector::release(){
	_dwRecvBufSize = 0;
	_dwSendBufSize = 0;

	Address addr(_pszIP, _wPort);
	Channel* pSvrChannel = _pNetworkInterface->findChannel(addr);
	if (pSvrChannel != NULL){
		pSvrChannel->disconnect();
	}

	DEL this;
}

void CSLConnector::setPacketParser(ISLPacketParser* poPacketParser){
	_pPacketParser = poPacketParser;
}

}
}