#ifndef KERNEL_IPC_ENGINE_H
#define KERNEL_IPC_ENGINE_H
#include "slshm.h"
#include "slsingleton.h"
#include <unordered_map>
#include <list>
#include <set>
#include "slipc_session.h"
#include "slipc_mq.h"
namespace sl{
namespace core{
class IPCEngine : public SLSingleton<IPCEngine>{
	friend class SLSingleton<IPCEngine>;
public:
	virtual bool ready();
	virtual bool initialize();
	virtual bool destory();

	virtual bool addIPCServer(sl::api::ITcpServer* server,uint64 serverId);
	virtual bool addIPCClient(sl::api::ITcpSession* session, uint64 clientId, uint64 serverId, int32 size);
	virtual const char* getInternetIp() { return ""; }

	void onNewConnect(uint64 clientId);
	void onDisconnect(uint64 clientId);

	virtual int64 loop(int64 overTime);

private:
	IPCEngine(){}
	~IPCEngine();

private:
	shm::ISLShmMgr*			    m_pShmMgr;
	std::unordered_map<int64, IPCSession*> m_ipcSessons;
	IPCSessionFactory*			m_sessionFactory;
	uint64						m_serverId;
	SLIpcMq						m_ipcMQ;
};
}
}

#endif