#include "slipc_mq.h"
#include "slkernel.h"
#include <mutex>
#include "slipc_engine.h"

namespace sl{
namespace core{

void SLIpcMq::threadRun(){
	while (!_terminate){
		if (!svrCreateNamedPipe(_svrPipeName)){
			SLASSERT(false, "create svr pipe failed");
			return;
		}
		ECHO_TRACE("+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++");
		ECHO_TRACE("client conning....");
		svrReadPipeMsg();

		CloseHandle(_svrNamePipe);
		_svrNamePipe = NULL;
		ECHO_TRACE("-----------------------------------------------------------");
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
			IPCEngine::getInstance()->onNewConnect(msg->getId());
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

bool SLIpcMq::connect(const int64 serverId, const int64 clientId){
	char pipeName[128] = { 0 };
	SafeSprintf(pipeName, sizeof(pipeName), "\\\\.\\pipe\\%lld", serverId);
	if (!clientOpenNamedPipe(pipeName))
		return false;

	ECHO_TRACE("connect Namepip %s success", pipeName);
	PipeMsg msg{ MSG_CONNECT, clientId };
	return clientWritePipeMsg(msg);
}

bool SLIpcMq::svrCreateNamedPipe(const char* pPipeName){
	OVERLAPPED                ovlpd;
	_svrNamePipe = CreateNamedPipe(pPipeName, PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED, 0, 50, 1024, 1024, 0, NULL);
	if (INVALID_HANDLE_VALUE == _svrNamePipe){
		SLASSERT(false, "create pipe failed");
		_svrNamePipe = NULL;
		return false;
	}

	//添加事件以等待客户端连接命名管道
	//该事件为手动重置事件，且初始化状态为无信号状态
	_hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (!_hEvent){
		return false;
	}

	memset(&ovlpd, 0, sizeof(OVERLAPPED));
	ovlpd.hEvent = _hEvent;

	ECHO_TRACE("create Namepipe sever %s success", _svrPipeName);

	//等待客户端连接
	if (!ConnectNamedPipe(_svrNamePipe, &ovlpd)){
		if (ERROR_IO_PENDING != GetLastError()){
			CloseHandle(_svrNamePipe);
			CloseHandle(_hEvent);
			_svrNamePipe = NULL;
			SLASSERT(false, "wait client failed");
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

	CloseHandle(_hEvent);

	return true;
}

bool SLIpcMq::clientOpenNamedPipe(const char* pPipeName){
	//连接
	if (!WaitNamedPipe(pPipeName, NMPWAIT_WAIT_FOREVER)){
		SLASSERT(false, "wait name pipe[%s %d] failed", pPipeName, GetLastError());
		return false;
	}

	//打开命名管道
	_clientNamePipe = CreateFile(pPipeName, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (INVALID_HANDLE_VALUE == _clientNamePipe){
		//CloseHandle(_clientNamePipe);
		//SLASSERT(false, "open pipe failed");
		return false;
	}

	return true;
}

bool SLIpcMq::svrReadPipeMsg(){
	ECHO_ERROR("start recv pipe msg");
	DWORD    dwRead;
	PipeMsg	 msg;

	//从命名管道中读取数据
	if (!ReadFile(_svrNamePipe, &msg, sizeof(msg), &dwRead, NULL)){
		SLASSERT(false, "read pipe data failed");
		return false;
	}

	PipeMsgNode* msgNode = NEW PipeMsgNode(msg);
	_waitQueue.pushBack(msgNode);
	return true;
}

bool SLIpcMq::clientWritePipeMsg(PipeMsg& msg){
	DWORD                dwWrite;

	//向命名管道中写入数据
	if (!WriteFile(_clientNamePipe, &msg, sizeof(msg), &dwWrite, NULL)){
		SLASSERT(false, "write pipe data failed");
		return false;
	}

	CloseHandle(_clientNamePipe);
	_clientNamePipe = NULL;

	return true;
}



}
}