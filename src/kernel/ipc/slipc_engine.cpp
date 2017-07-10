#include "slipc_engine.h"
#include "slipc_session.h"
#include "slkernel.h"
#include "slipc_mq.h"
#include "sltime.h"

using namespace sl::shm;
namespace sl{
namespace core{
IPCEngine::~IPCEngine(){
	if (_shmMgr)
		_shmMgr->release();

	if (_ipcMQ)
		_ipcMQ->release();
}

bool IPCEngine::initialize(){
	_shmMgr = newShmMgr();
	if (nullptr == _shmMgr)
		return false;

	_ipcMQ = SLIpcMq::getInstance();
	if (nullptr == _ipcMQ)
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
	if (nullptr == _shmMgr || _ipcMQ->isStart())
		return false;

	_serverId = serverId;
	_sessionFactory = NEW IPCSessionFactory(server);

	_ipcMQ->listen(serverId);
	
	return true;
}
bool IPCEngine::addIPCClient(sl::api::ITcpSession* session, const int64 clientId, const int64 serverId, const int32 sendSize, const int32 recvSize){
	SLASSERT(_shmMgr, "wtf");
	if (_ipcSessons.find(serverId) != _ipcSessons.end()){
		SLASSERT(false, "has conntected");
		return true;
	}

	char shmKey[128] = { 0 };
	SafeSprintf(shmKey, sizeof(shmKey), "%s/shmkey/%lld_%lld.key", Kernel::getInstance()->getIpcPath(), clientId, serverId);
	ISLShmQueue* shmQueue = _shmMgr->createShmQueue(true, shmKey, sendSize, recvSize);
	SLASSERT(shmQueue, "wtf");

	if (!_ipcMQ->connect(serverId, clientId, sendSize, recvSize)){
		_shmMgr->recover(shmQueue);
		return false; 
	}
	
	IPCSession* ipcSession = IPCSession::create(session, shmQueue, clientId, serverId);
	SLASSERT(ipcSession, "wtf");
	_ipcSessons[serverId] = ipcSession;
	ipcSession->onEstablish();

	return true;
}

int64 IPCEngine::loop(int64 overTime){
	int64 startTime = sl::getTimeMilliSecond();
	
	//_ipcMQ->loop();

	if (!_ipcSessons.empty()){
		int64 costTime = 0;
		while (true){
			bool needBreak = true;
			
			std::unordered_map<int64, IPCSession*>::iterator sessionItor = _ipcSessons.begin();
			for (; sessionItor != _ipcSessons.end(); ){
				int32 ret = sessionItor->second->procRecv();
				if (ret < 0){
					sessionItor = _ipcSessons.erase(sessionItor);
					continue;
				}

				if (ret > 0)
					needBreak = false;

				if (needBreak)
					break;

				sessionItor++;
			}
		}
	}
	return sl::getTimeMilliSecond() - startTime;
}


void IPCEngine::onNewConnect(int64 clientId, int32 sendSize, int32 recvSize){
	char shmKey[128] = { 0 };
	SafeSprintf(shmKey, sizeof(shmKey), "%s/shmkey/%lld_%lld.key", Kernel::getInstance()->getIpcPath(), clientId, _serverId);
	ISLShmQueue* shmQueue = _shmMgr->createShmQueue(false, shmKey, sendSize, recvSize, false);
	SLASSERT(shmQueue, "wtf");

	IPCSession* ipcSession = _sessionFactory->createSession(shmQueue, _serverId, clientId);
	_ipcSessons[clientId] = ipcSession;
	ipcSession->onEstablish();
}

void IPCEngine::onDisconnect(int64 clientId){
	if (_ipcSessons.find(clientId) == _ipcSessons.end()){
		SLASSERT(false, "wtf");
		return;
	}
	
	_ipcSessons[clientId]->onTerminate();
	_ipcSessons.erase(clientId);
}

bool IPCEngine::close(int64 serverId, int64 clientId){
	if (_ipcSessons.find(serverId) == _ipcSessons.end()){
		SLASSERT(false, "wtf");
		return false;
	}

	_ipcSessons[serverId]->onTerminate();
	_ipcSessons[serverId]->release();
	_ipcSessons.erase(serverId);

	return _ipcMQ->closePipe(serverId, clientId);
}


}
}
