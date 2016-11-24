#ifndef KERNEL_NET_ENGINE_H
#define KERNEL_NET_ENGINE_H
#include "slinet_engine.h"
#include "slsingleton.h"
#include "slnet.h"
namespace sl
{
namespace core
{
class NetEngine: public INetEngine, public CSingleton<NetEngine>
{
private:
	NetEngine();
	~NetEngine();

public:
	virtual bool ready();
	virtual bool initialize();
	virtual bool destory();

	virtual bool addTcpServer(sl::api::ITcpServer* server, const char* ip, const short port, int sendSize, int recvSize);
	virtual bool adddTcpClient(sl::api::ITcpSession* session, const char* ip, const short port, int sendSize, int recvSize);

private:
	ISLNet*			m_pSLNetModule;
	ISLListener*	m_pListener;
};
}
}

#endif