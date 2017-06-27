#ifndef SL_LIB_NET_CONNECTOR_H
#define SL_LIB_NET_CONNECTOR_H

#include "slnet.h"
#include "slchannel.h"
namespace sl{
namespace network{

class CSLConnector: public ISLConnector{
public:
	CSLConnector();
	virtual ~CSLConnector();

	virtual void SLAPI setSession(ISLSession* pSession);
	virtual bool SLAPI connect(const char* pszIP, uint16 wPort);
	virtual bool SLAPI reConnect(void);
	virtual void SLAPI setBufferSize(uint32 dwRecvBufSize, uint32 dwSendBufSize);
	virtual void SLAPI setPacketParser(ISLPacketParser* poPacketParser);
	virtual void SLAPI release(void);

private:
	//外部对象指针，不需要本类释放
	ISLSession*			_pSession;
	NetworkInterface*	_pNetworkInterface;
	ISLPacketParser*	_pPacketParser;

	uint32				_dwRecvBufSize;
	uint32				_dwSendBufSize;
	char				_pszIP[256];
	uint16				_wPort;
	
};
}
}
#endif