#include "slipc_mq.h"
#include "slkernel.h"
#include <mutex>
#include "slipc_engine.h"

namespace sl{
namespace core{
SLIpcMq* SLIpcMq::getInstance(){
	static SLIpcMq* p = NULL;
	if (p == NULL)
		p = new SLIpcMq;
	return p;
}

SLIpcMq::SLIpcMq() 
	:_svrNamePipe(NULL),
	_clientNamePipe(NULL), 
	_start(false), 
	_terminate(false)
{}

SLIpcMq::~SLIpcMq(){
	_terminate = true;

	if (_start)
		_thread.join();

	if (_svrNamePipe)
		CloseHandle(_svrNamePipe);
	_svrNamePipe = NULL;
	
	if (_clientNamePipe)
		CloseHandle(_clientNamePipe);
	_clientNamePipe = NULL;
}

void SLIpcMq::threadRun(){
	if (!svrCreateNamedPipe(_svrPipeName))
		return;

	while (!_terminate){
		
		if (!svrReadPipeMsg())
			return;

		DisconnectNamedPipe(_svrNamePipe);
	}
}

int64 SLIpcMq::loop(){
	if (_runningQueue.isEmpty()){
		std::unique_lock<sl::spin_mutex> lock(_mutex);
		_runningQueue.merge(_waitQueue);
		_waitQueue.clear();
	}

	if (_runningQueue.isEmpty()){
		return 0;
	}

	processMsg();

	return 0;
}

void SLIpcMq::processMsg(){
	while (!_runningQueue.isEmpty()){
		PipeMsgNode* msg = (PipeMsgNode*)_runningQueue.popFront();
		
		if (msg->getType() == MSG_CONNECT){
			IPCEngine::getInstance()->onNewConnect(msg->getId(), msg->getRecvSize(), msg->getSendSize());
		}
		if (msg->getType() == MSG_CLOSE){
			IPCEngine::getInstance()->onDisconnect(msg->getId());
		}

		DEL msg;
	}
}

bool SLIpcMq::listen(const int64 serverId){
	if (NULL != _svrNamePipe)
		return false;
	
	SafeSprintf(_svrPipeName, sizeof(_svrPipeName), "\\\\.\\pipe\\%lld", serverId);

	_thread = std::thread(&SLIpcMq::threadRun, this);
	_start = true;

	return true;
}

bool SLIpcMq::connect(const int64 serverId, const int64 clientId, const int32 sendSize, const int32 recvSize){
	int32 nodeId = serverId & 0xFFFFFFFF;
	int32 nodeType = (uint64)serverId >> 32;
	char pipeName[128] = { 0 };
	SafeSprintf(pipeName, sizeof(pipeName), "\\\\.\\pipe\\%lld", serverId);
	if (!clientOpenNamedPipe(pipeName))
		return false;

	ECHO_TRACE("SLIpcMq connect Namepip [%d %d]success", nodeType, nodeId);
	PipeMsgNode::PipeMsg msg{ MSG_CONNECT, clientId, sendSize, recvSize};
	return clientWritePipeMsg(msg);
}

bool SLIpcMq::svrCreateNamedPipe(const char* pPipeName){
	_svrNamePipe = CreateNamedPipe(pPipeName, PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED, 0, 20, 512, 512, 10, NULL);
	if (INVALID_HANDLE_VALUE == _svrNamePipe){
		SLASSERT(false, "create pipe failed[%d]", GetLastError());
		_svrNamePipe = NULL;
		return false;
	}

	//添加事件以等待客户端连接命名管道
	_hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (!_hEvent){
		return false;
	}

	memset(&_ovlpd, 0, sizeof(OVERLAPPED));
	_ovlpd.hEvent = _hEvent;

	ECHO_TRACE("create Namepipe sever %s success", _svrPipeName);

	return true;
}

bool SLIpcMq::clientOpenNamedPipe(const char* pPipeName){
	//连接
	if (!WaitNamedPipe(pPipeName, NMPWAIT_USE_DEFAULT_WAIT)){
		ECHO_ERROR("wait name pipe[%s %d] failed", pPipeName, GetLastError());
		return false;
	}

	//打开命名管道
	_clientNamePipe = CreateFile(pPipeName, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (INVALID_HANDLE_VALUE == _clientNamePipe){
		//CloseHandle(_clientNamePipe);
		SLASSERT(false, "open pipe failed");
		return false;
	}

	return true;
}

bool SLIpcMq::svrReadPipeMsg(){
	//等待客户端连接
	if (!ConnectNamedPipe(_svrNamePipe, &_ovlpd)){
		if (ERROR_IO_PENDING != GetLastError() && ERROR_PIPE_CONNECTED != GetLastError()){
			CloseHandle(_svrNamePipe);
			CloseHandle(_hEvent);
			_svrNamePipe = NULL;
			int32 errcode = GetLastError();
			SLASSERT(false, "wait client failed[%d]", GetLastError());
			return false;
		}
	}

	//等待事件 hEvent 失败
	if (WAIT_FAILED == WaitForSingleObject(_hEvent, INFINITE)){
		CloseHandle(_svrNamePipe);
		CloseHandle(_hEvent);
		SLASSERT(false, "wait event failed");
		return false;
	}

	DWORD    dwRead;
	PipeMsgNode::PipeMsg	 msg;
	//从命名管道中读取数据
	if (!ReadFile(_svrNamePipe, &msg, sizeof(msg), &dwRead, NULL)){
		SLASSERT(false, "read pipe data failed[%d]", GetLastError());
		return false;
	}

	PipeMsgNode* msgNode = NEW PipeMsgNode(msg);
	_waitQueue.pushBack(msgNode);
	return true;
}

bool SLIpcMq::clientWritePipeMsg(PipeMsgNode::PipeMsg& msg){
	DWORD                dwWrite;
	//向命名管道中写入数据
	if (!WriteFile(_clientNamePipe, &msg, sizeof(msg), &dwWrite, NULL)){
		ECHO_ERROR("write pipe data failed[%d]", GetLastError());
		return false;
	}

	return true;
}



}
}