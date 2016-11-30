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

	virtual void SLAPI setSession(ISLSession* pSession) = 0;

	virtual bool SLAPI connect(const char* pszIP, uint16 wPort) = 0;

	virtual bool SLAPI reConnect(void) = 0;

	virtual void SLAPI setBufferSize(uint32 dwRecvBufSize, uint32 dwSendBufSize) = 0;

	virtual void SLAPI release(void) = 0;

private:
	Channel*			m_pSvrChannel;
	ISLSession*			m_pSession;
	EventDispatcher*	m_pEventDispatcher;
	EndPoint*			m_pEndPoint;
	uint32				m_dwRecvBufSize;
	uint32				m_dwSendBufSize;
	bool				m_bConnected;
};
}
}
#endif