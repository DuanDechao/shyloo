#ifndef SL_SLLISTENER_H
#define SL_SLLISTENER_H
#include "slnet.h"
#include "slnetwork_interface.h"
namespace sl
{
using namespace network;
class CSLListener: public ISLListener
{
public:
	CSLListener();
	virtual ~CSLListener();

	virtual void SLAPI setSessionFactory(ISLSessionFactory* poSessionFactory);

	virtual void SLAPI setBufferSize(int dwRecvBufSize, int dwSendBufSize);

	virtual bool SLAPI start(const char* pszIP, short wPort, bool bReUseAddr = true);

	virtual bool SLAPI stop(void);

	virtual void SLAPI release(void);

private:
	NetworkInterface*		m_pNetworkInterface;
	ISLSessionFactory*		m_pSessionFactory;
	int						m_dwRecvBufSize;
	int						m_dwSendBufSize;
	bool					m_bStart;

}; 
}


#endif