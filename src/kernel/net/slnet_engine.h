#ifndef KERNEL_NET_ENGINE_H
#define KERNEL_NET_ENGINE_H
#include "slinet_engine.h"
#include "slnet.h"
namespace sl
{
namespace core
{
class NetEngine: public INetEngine
{
private:
	NetEngine(){}
	~NetEngine(){}

public:
	static INetEngine* getInstance();

	virtual bool ready();
	virtual bool initialize();
	virtual bool destory();

	virtual bool addTcpServer(sl::api::ITcpServer* server, const char* ip, const short port, int sendSize, int recvSize);
	virtual bool addTcpClient(sl::api::ITcpSession* session, const char* ip, const short port, int sendSize, int recvSize);


	virtual int64 processing(int64 overTime);
private:
	network::ISLNet*			m_pSLNetModule;
	network::ISLListener*		m_pListener;
};
}
}

#endif