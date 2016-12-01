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

	virtual bool SLAPI reConnect(void){return true;}

	virtual void SLAPI setBufferSize(uint32 dwRecvBufSize, uint32 dwSendBufSize);

	virtual void SLAPI release(void);

private:
	ISLSession*			m_pSession;
	NetworkInterface*	m_pNetworkInterface;
	EndPoint			m_pSvrEndPoint;
	uint32				m_dwRecvBufSize;
	uint32				m_dwSendBufSize;
};
}
}
#endif