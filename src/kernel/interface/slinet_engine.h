#ifndef KERNEL_INET_ENGINE_H
#define KERNEL_INET_ENGINE_H
#include "slicore.h"
#include "slikernel.h"
namespace sl{
namespace core{
class INetEngine: public ICore{
public:
	virtual bool addTcpServer(sl::api::ITcpServer* server, const char* ip, const short port, int sendSize, int recvSize) = 0;
	virtual bool addTcpClient(sl::api::ITcpSession* session, const char* ip, const short port, int sendSize, int recvSize) = 0;
	virtual int64 loop(int64 overTime) = 0;
	virtual const char* getInternetIp() = 0;
};

}
}

#endif