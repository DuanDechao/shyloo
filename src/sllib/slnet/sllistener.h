#ifndef SL_LIB_NET_LISTENER_H
#define SL_LIB_NET_LISTENER_H
#include "slnet.h"
#include "slnetwork_interface.h"
namespace sl{
namespace network{
class CSLListener : public ISLListener{
public:
	CSLListener();
	virtual ~CSLListener();

	virtual void SLAPI setSessionFactory(ISLSessionFactory* poSessionFactory);
	virtual void SLAPI setBufferSize(uint32 dwRecvBufSize, uint32 dwSendBufSize);
	virtual void SLAPI setPacketParser(ISLPacketParser* poPacketParser);
	virtual bool SLAPI start(const char* pszIP, uint16 wPort, bool bReUseAddr = true);
	virtual bool SLAPI stop(void);
	virtual void SLAPI release(void);

private:
	NetworkInterface*		_pNetworkInterface;
	ListenerReceiver*		_pListenerReceiver;
	EndPoint*			    _pListenEndPoint;
	int32					_dwRecvBufSize;
	int32					_dwSendBufSize;
};
}
}


#endif