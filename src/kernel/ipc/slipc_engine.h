#ifndef KERNEL_IPC_ENGINE_H
#define KERNEL_IPC_ENGINE_H
#include "slshm.h"
#include "slsingleton.h"
#include <unordered_map>
#include "slipc_session.h"
#include "slipc_mq.h"
#include "slikernel.h"

namespace sl{
namespace core{
class IPCEngine : public SLSingleton<IPCEngine>{
	friend class SLSingleton<IPCEngine>;
public:
	virtual bool ready();
	virtual bool initialize();
	virtual bool destory();

	virtual bool addIPCClient(sl::api::ITcpSession* session, const int64 clientId, const int64 serverId, const int32 sendSize, const int32 recvSize);
	virtual bool addIPCServer(sl::api::ITcpServer* server, const int64 serverId);

	void onNewConnect(int64 clientId, int32 sendSize, int32 recvSize);
	void onDisconnect(int64 clientId);

	bool close(int64 serverId, int64 clientId);

	virtual int64 loop(int64 overTime);

private:
	IPCEngine(){}
	~IPCEngine();

private:
	shm::ISLShmMgr*							_shmMgr;
	std::unordered_map<int64, IPCSession*>  _ipcSessons;
	IPCSessionFactory*						_sessionFactory;
	uint64									_serverId;
	SLIpcMq*								_ipcMQ;
};
}
}

#endif