#include "slipc_engine.h"
#include "slipc_session.h"
#include "slkernel.h"
using namespace sl::shm;
namespace sl{
namespace core{
IPCEngine::~IPCEngine(){
	m_pShmMgr->release();
}

bool IPCEngine::initialize(){
	m_pShmMgr = newShmMgr();
	if (nullptr == m_pShmMgr)
		return false;
	return true;
}

bool IPCEngine::ready(){
	return true;
}

bool IPCEngine::destory(){
	DEL this;
	return true;
}

bool IPCEngine::addIPCServer(sl::api::ITcpServer* server, uint64 serverId){
	if (nullptr == m_pShmMgr || m_ipcMQ.isStart())
		return false;

	m_serverId = serverId;
	m_sessionFactory = NEW IPCSessionFactory(server);

	m_ipcMQ.listen(serverId);
	
	return true;
}
bool IPCEngine::addIPCClient(sl::api::ITcpSession* session, uint64 clientId, uint64 serverId, int32 size){
	SLASSERT(m_pShmMgr, "wtf");
	if (m_ipcSessons.find(serverId) != m_ipcSessons.end()){
		SLASSERT(false, "has conntected");
		return true;
	}

	if (!m_ipcMQ.connect(serverId, clientId)){
		//SLASSERT(false, "connect failed");
		return false; 
	}

	char shmKey[128] = { 0 };
	SafeSprintf(shmKey, sizeof(shmKey), "./shmkey/%lld_%lld.key", (uint64)clientId, (uint64)serverId);
	ISLShmQueue* shmQueue = m_pShmMgr->createShmQueue(true, shmKey, size);
	SLASSERT(shmQueue, "wtf");
	
	IPCSession* ipcSession = CREATE_POOL_OBJECT(IPCSession, session, shmQueue, clientId, serverId);
	SLASSERT(ipcSession, "wtf");
	m_ipcSessons[serverId] = ipcSession;
	ipcSession->onEstablish();

	return true;
}

int64 IPCEngine::loop(int64 overTime){
	int64 startTime = sl::getTimeMilliSecond();
	
	m_ipcMQ.loop();

	if (!m_ipcSessons.empty()){
		int64 costTime = 0;
		std::unordered_map<int64, IPCSession*>::iterator sessionItor = m_ipcSessons.begin();
		for (; sessionItor != m_ipcSessons.end(); sessionItor++){
			costTime = sl::getTimeMilliSecond() - startTime;
			if (costTime > overTime)
				return costTime;

			sessionItor->second->procRecv();
		}
			
	}
	//ECHO_ERROR("IPCEngine loop");
	return sl::getTimeMilliSecond() - startTime;
}


void IPCEngine::onNewConnect(uint64 clientId){
	char shmKey[128] = { 0 };
	SafeSprintf(shmKey, sizeof(shmKey), "./shmkey/%lld_%lld.key", (uint64)clientId, (uint64)m_serverId);
	ISLShmQueue* shmQueue = m_pShmMgr->createShmQueue(false, shmKey, 65525);
	SLASSERT(shmQueue, "wtf");

	IPCSession* ipcSession = m_sessionFactory->createSession(shmQueue, m_serverId, clientId);
	m_ipcSessons[clientId] = ipcSession;
	ipcSession->onEstablish();
}

void IPCEngine::onDisconnect(uint64 clientId){
	if (m_ipcSessons.find(clientId) == m_ipcSessons.end()){
		SLASSERT(false, "wtf");
		return;
	}
	
	return m_ipcSessons[clientId]->onTerminate();
}

const char* IPCEngine::getInternetIp(){
	return Kernel::getInstance()->getInternetIp();
}


}
}