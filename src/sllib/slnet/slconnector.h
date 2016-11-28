#ifndef SL_SLNET_CONNECTOR_H
#define SL_SLNET_CONNECTOR_H
#include "sltype.h"
#include "slnet.h"
#include "slendpoint.h"
#include "slnetwork_interface.h"
#include "sltcp_packet_receiver.h"
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

	virtual void SLAPI setBufferSize(uint32 dwRecvBufSize, uint32 dwSendBufSize);

	virtual bool SLAPI connect(const char* pszIp, uint16 wPort);

	virtual bool SLAPI reConnect(void);

	virtual void SLAPI release(void);
private:
	NetworkInterface*		m_pNetworkInterface;
	ISLSession*				m_pSession;
	int32					m_dwRecvBufSize;
	int32					m_dwSendBufSize;
	EndPoint				m_connEndPoint;
	TCPPacketReceiver*		m_tcpPacketReceiver;
};
}
}

#endif