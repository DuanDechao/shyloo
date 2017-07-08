#ifndef KERNEL_NET_ENGINE_H
#define KERNEL_NET_ENGINE_H
#include "slinet_engine.h"
#include "slnet.h"
#include "slsingleton.h"

namespace sl{
namespace core{

#define MAX_IP_LEN 128
#define MAX_LOCAL_IP_PREFIX_NUM 19

#ifdef SL_OS_WINDOWS
#pragma comment(lib,"Iphlpapi.lib")
#endif

class NetEngine: public SLSingleton<NetEngine>{
	friend class SLSingleton<NetEngine>;
public:
	virtual bool ready();
	virtual bool initialize();
	virtual bool destory();

	virtual bool addTcpServer(sl::api::ITcpServer* server, const char* ip, const short port, int sendSize, int recvSize);
	virtual bool addTcpClient(sl::api::ITcpSession* session, const char* ip, const short port, int sendSize, int recvSize);
	virtual const char* getInternetIp() { return m_ip; }
	virtual const char* getLocalIp() { return m_localIp; }

	virtual int64 loop(int64 overTime);

	void readInternetIp();

	bool isLocalIp(const char* ip);

private:
	NetEngine(){}
	~NetEngine();

private:
	network::ISLNet*			m_pSLNetModule;
	char						m_ip[MAX_IP_LEN];
	char						m_localIp[MAX_IP_LEN];
};
}
}

#endif
