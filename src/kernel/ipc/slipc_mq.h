#ifndef SL_KERNEL_IPC_MQ_H
#define SL_KERNEL_IPC_MQ_H
#include "slikernel.h"
#include <thread>
#include "sllist.h"
#include "slmutex.h"

namespace sl{
namespace core{

using namespace api;
struct PipeMsg{
	int8 type;
	int64 id;
};

class PipeMsgNode: public ISLListNode{
public:
	PipeMsgNode(PipeMsg msg) :_type(msg.type), _id(msg.id){}

	inline int8 getType() const { return _type; }
	inline int64 getId() const { return _id; }
private:
	int8 _type;
	int64 _id;
};

class SLIpcMq{
	enum{
		MSG_CONNECT = 0,
		MSG_CLOSE,
	};

public:
	SLIpcMq() :_svrNamePipe(NULL), _clientNamePipe(NULL), _start(false), _terminate(false){}
	virtual ~SLIpcMq(){}

	bool connect(const int64 serverId, const int64 clientId);
	bool listen(const int64 serverId);

	void processMsg();

	void threadRun();
	int64 loop();

	inline bool isStart() const { return _start; }

private:
	bool svrCreateNamedPipe(const char* pPipeName);
	bool clientOpenNamedPipe(const char* pPipeName);
	bool svrReadPipeMsg();
	bool clientWritePipeMsg(PipeMsg& msg);

private:
	std::thread		_thread;
	sl::SLList		_waitQueue;
	sl::SLList		_runningQueue;
	HANDLE			_svrNamePipe;
	HANDLE			_clientNamePipe;
	HANDLE			_hEvent;
	sl::spin_mutex  _mutex;
	bool			_terminate;
	bool			_start;
	char			_svrPipeName[128];
};

}
}

#endif