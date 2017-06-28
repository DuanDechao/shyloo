#include "slipc_engine.h"
#include "slipc_session.h"
#include "slkernel.h"
#include "slipc_mq.h"
#include "sltime.h"

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

	m_ipcMQ = SLIpcMq::getInstance();
	if (nullptr == m_ipcMQ)
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

bool IPCEngine::addIPCServer(sl::api::ITcpServer* server, const int64 serverId){
	if (nullptr == m_pShmMgr || m_ipcMQ->isStart())
		return false;

	m_serverId = serverId;
	m_sessionFactory = NEW IPCSessionFactory(server);

	m_ipcMQ->listen(serverId);
	
	return true;
}
bool IPCEngine::addIPCClient(sl::api::ITcpSession* session, const int64 clientId, const int64 serverId, const int32 sendSize, const int32 recvSize){
	SLASSERT(m_pShmMgr, "wtf");
	if (m_ipcSessons.find(serverId) != m_ipcSessons.end()){
		SLASSERT(false, "has conntected");
		return true;
	}

	char shmKey[128] = { 0 };
	SafeSprintf(shmKey, sizeof(shmKey), "%s/shmkey/%lld_%lld.key", Kernel::getInstance()->getIpcPath(), clientId, serverId);
	ISLShmQueue* shmQueue = m_pShmMgr->createShmQueue(true, shmKey, sendSize, recvSize);
	SLASSERT(shmQueue, "wtf");

	if (!m_ipcMQ->connect(serverId, clientId, sendSize, recvSize)){
		m_pShmMgr->recover(shmQueue);
		return false; 
	}
	
	IPCSession* ipcSession = IPCSession::create(session, shmQueue, clientId, serverId);
	SLASSERT(ipcSession, "wtf");
	m_ipcSessons[serverId] = ipcSession;
	ipcSession->onEstablish();

	return true;
}

int64 IPCEngine::loop(int64 overTime){
	int64 startTime = sl::getTimeMilliSecond();
	
	m_ipcMQ->loop();

	if (!m_ipcSessons.empty()){
		int64 costTime = 0;
		while (true){
			bool needBreak = true;
			
			std::unordered_map<int64, IPCSession*>::iterator sessionItor = m_ipcSessons.begin();
			for (; sessionItor != m_ipcSessons.end(); sessionItor++){
				if (sessionItor->second->procRecv() > 0)
					needBreak = false;

				if (needBreak)
					break;
			}
		}
	}
	//ECHO_ERROR("IPCEngine loop");
	return sl::getTimeMilliSecond() - startTime;
}


void IPCEngine::onNewConnect(int64 clientId, int32 sendSize, int32 recvSize){
	char shmKey[128] = { 0 };
	SafeSprintf(shmKey, sizeof(shmKey), "%s/shmkey/%lld_%lld.key", Kernel::getInstance()->getIpcPath(), clientId, m_serverId);
	ISLShmQueue* shmQueue = m_pShmMgr->createShmQueue(false, shmKey, sendSize, recvSize, false);
	SLASSERT(shmQueue, "wtf");

	IPCSession* ipcSession = m_sessionFactory->createSession(shmQueue, m_serverId, clientId);
	m_ipcSessons[clientId] = ipcSession;
	ipcSession->onEstablish();
}

void IPCEngine::onDisconnect(int64 clientId){
	if (m_ipcSessons.find(clientId) == m_ipcSessons.end()){
		SLASSERT(false, "wtf");
		return;
	}
	
	return m_ipcSessons[clientId]->onTerminate();
}


}
}