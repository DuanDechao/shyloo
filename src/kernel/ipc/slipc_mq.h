#ifndef SL_KERNEL_IPC_MQ_H
#define SL_KERNEL_IPC_MQ_H
#include "slikernel.h"
#include <thread>
#include "sllist.h"
#include "slmutex.h"
#include "slsingleton.h"

namespace sl{
namespace core{
using namespace api;
class PipeMsgNode: public ISLListNode{
public:
	struct PipeMsg{
		int8 type;
		int64 id;
		int32 sendSize;
		int32 recvSize;
	};

	PipeMsgNode(PipeMsg& msg) :_type(msg.type), _id(msg.id), _sendSize(msg.sendSize), _recvSize(msg.recvSize){}

	inline int8 getType() const { return _type; }
	inline int64 getId() const { return _id; }
	inline int32 getSendSize() const { return _sendSize; }
	inline int32 getRecvSize() const { return _recvSize; }
private:
	int8 _type;
	int64 _id;
	int32 _sendSize;
	int32 _recvSize;
};

class SLIpcMq{
	enum{
		MSG_CONNECT = 0,
		MSG_CLOSE,
	};

public:
	static SLIpcMq* getInstance();

	inline void release(){ DEL this; }

	bool connect(const int64 serverId, const int64 clientId, const int32 sendSize, const int32 recvSize);
	bool listen(const int64 serverId);
	bool closePipe(const int64 serverId, const int64 clientId);

	void processMsg();

	void threadRun();
	int64 loop();

	inline bool isStart() const { return _start; }

private:
	SLIpcMq();
	virtual ~SLIpcMq();

private:
	bool svrCreateNamedPipe(const char* pPipeName);
	bool clientOpenNamedPipe(const char* pPipeName);
	bool svrReadPipeMsg();
	bool clientWritePipeMsg(PipeMsgNode::PipeMsg& msg);

private:
	std::thread		_thread;
	sl::SLList		_waitQueue;
	sl::SLList		_runningQueue;
	sl::spin_mutex  _mutex;
	bool			_terminate;
	bool			_start;
	char			_svrPipeName[128];
    int64			_serverId;

#ifdef SL_OS_WINDOWS
	HANDLE			_svrNamePipe;
	HANDLE			_clientNamePipe;
	HANDLE			_hEvent;
	OVERLAPPED      _ovlpd;
#else
	int32			_svrNamePipe;
	int32			_clientNamePipe;
#endif
};

}
}

#endif
