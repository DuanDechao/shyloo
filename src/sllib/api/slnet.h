#ifndef SL_SLNET_H
#define SL_SLNET_H
#include "sltype.h"
namespace sl{
namespace network{

class ISLSession;
class ISLSessionFactory;
class ISLPacketParser;
class ISLChannel{
public:
	virtual bool SLAPI isConnected(void) = 0;
	virtual void SLAPI send(const char* pBuf, uint32 dwLen) = 0;
	virtual void SLAPI disconnect(void) = 0;
	virtual const unsigned int SLAPI getRemoteIP(void) = 0;
	virtual const char* SLAPI getRemoteIPStr(void) = 0;
	virtual const unsigned short SLAPI getRemotePort(void) = 0;
	virtual const unsigned int SLAPI getLocalIP(void) = 0;
	virtual const char* SLAPI getLocalIPStr(void) = 0;
	virtual const unsigned short SLAPI getLocalPort(void) = 0;
	virtual void SLAPI adjustSendBuffSize(const int32 size) = 0;
	virtual void SLAPI adjustRecvBuffSize(const int32 size) = 0;
};

class ISLConnector{
public:
	virtual void SLAPI setSession(ISLSession* pSession) = 0;
	virtual bool SLAPI connect(const char* pszIP, uint16 wPort) = 0;
	virtual bool SLAPI reConnect(void) = 0;
	virtual void SLAPI setBufferSize(uint32 dwRecvBufSize, uint32 dwSendBufSize) = 0;
	virtual void SLAPI setPacketParser(ISLPacketParser* poPacketParser) = 0;
	virtual void SLAPI release(void) = 0;
};

class ISLListener{
public:
	virtual void SLAPI setSessionFactory(ISLSessionFactory* poSessionFactory) = 0;
	virtual void SLAPI setBufferSize(uint32 dwRecvBufSize, uint32 dwSendBufSize) = 0;
	virtual void SLAPI setPacketParser(ISLPacketParser* poPacketParser) = 0;
	virtual bool SLAPI start(const char* pszIP, uint16 wPort, bool bReUseAddr = true) = 0;
	virtual bool SLAPI stop(void) = 0;
	virtual void SLAPI release(void) = 0;
};

class ISLSession{
public:
	virtual void SLAPI setChannel(ISLChannel* poChannel) = 0;
	virtual void SLAPI onEstablish(void) = 0;
	virtual void SLAPI onTerminate() = 0;
	virtual void SLAPI release(void) = 0;
	virtual void SLAPI onRecv(const char* pBuf, uint32 dwLen) = 0;
};

class ISLSessionFactory{
public:
	virtual ISLSession* SLAPI createSession(ISLChannel* poChannel) = 0;
};

class ISLPacketParser{
public:
	virtual int32 SLAPI parsePacket(const char* pDataBuf, int32 len) = 0;
};

class ISLNet{
public:
	virtual ISLListener* SLAPI createListener() = 0;
	virtual ISLConnector* SLAPI createConnector() = 0;
	virtual bool SLAPI run(int64 overtime) = 0;
	virtual void SLAPI release() = 0;
};

extern "C" SL_DLL_API sl::network::ISLNet* SLAPI getSLNetModule(void);

}
}



#endif