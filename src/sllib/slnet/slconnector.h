#ifndef SL_SLNET_CONNECTOR_H
#define SL_SLNET_CONNECTOR_H

#include "slnet.h"
#include "slchannel.h"
namespace sl
{
namespace network
{
class CSLConnector: public ISLConnector
{
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
	ISLSession*			m_pSession;
	NetworkInterface*	m_pNetworkInterface;
	ISLPacketParser*	m_pPacketParser;

	uint32				m_dwRecvBufSize;
	uint32				m_dwSendBufSize;
	const char*			m_pszIP;
	uint16				m_wPort;
	
};
}
}
#endif