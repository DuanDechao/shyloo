#ifndef KERNEL_IIPC_ENGINE_H
#define KERNEL_IIPC_ENGINE_H
#include "slicore.h"
#include "slikernel.h"
namespace sl{
namespace core{

class IIPCEngine: public ICore{
public:
	virtual bool addIPCClient(sl::api::ITcpSession* session, const int64 clientId, const int64 serverId, const int32 sendSize, const int32 recvSize) = 0;
	virtual bool addIPCServer(sl::api::ITcpServer* server, const int64 serverId) = 0;
	virtual int64 loop(int64 overTime) = 0;
};

}
}

#endif