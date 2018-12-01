#ifndef SL_FORWARD_MSG_BUFFER_H
#define SL_FORWARD_MSG_BUFFER_H
#include "slikernel.h"
#include "IHarbor.h"
#include "ICluster.h"
#include <list>
class ForwardMsg{
public:
	ForwardMsg(const int32 nodeId, int32 msgId, const void* msgBuffer, const int32 msgSize)
		:_nodeId(nodeId),
		_msgId(msgId),
		_msgSize(msgSize)
	{
		_msgBuffer.append((const char*)msgBuffer, msgSize);
	}
	~ForwardMsg() {}

	inline int32 nodeId() {return _nodeId;}
	inline int32 msgId() {return _msgId;}
	inline const void* buffer() {return _msgBuffer.data();}
	inline int32 msgSize() {return _msgSize;}
private:
	int32 _nodeId;
	int32 _msgId;
	int32 _msgSize;
	std::string _msgBuffer;
};

class ForwardMsgBuffer: public sl::api::ITimer{
public:
	ForwardMsgBuffer(sl::api::IKernel* pKernel, const int32 nodeType)
		:_nodeType(nodeType),
		 _start(false),
		 _kernel(pKernel)
	{}

	~ForwardMsgBuffer(){}
	
	virtual void onStart(sl::api::IKernel* pKernel, int64 timetick){}
	virtual void onTime(sl::api::IKernel* pKernel, int64 timetick) { process();}
	virtual void onTerminate(sl::api::IKernel* pKernel, bool beForced, int64 timetick){}
	virtual void onPause(sl::api::IKernel* pKernel, int64 timetick){}
	virtual void onResume(sl::api::IKernel* pKernel, int64 timetick){}

	void push(ForwardMsg* msg){
		_bufferMsgs.push_back(msg);
		if(!_start){
			sl::api::IKernel* pKernel = _kernel;
			START_TIMER(this, 0, -1, 1000);
			_start = true;
		}
	}

	void process(){
		if(_bufferMsgs.size() <= 0){
			if(_start)
				_kernel->killTimer(this);
			_start = false;
			return;
		}

		auto msgItor = _bufferMsgs.begin();
		for(;msgItor != _bufferMsgs.end();){
			ForwardMsg* msg = *msgItor;
			int32 nodeId = msg->nodeId();
			if(nodeId == 0){
				nodeId = SLMODULE(Cluster)->getBestNodeId(_nodeType);
			}

			int32 state = SLMODULE(Cluster)->getNodeState(_nodeType, nodeId);
			if(state != ServerState::S_RUNNING){
				msgItor++;
				continue;
			}

			sl::OBStream msgStream((const char*)msg->buffer(), msg->msgSize());
			SLMODULE(Harbor)->send(_nodeType, nodeId, msg->msgId(), msgStream);
			printf("send buffer msg.... %d %d\n", _nodeType, nodeId);
			
			DEL msg;
			msgItor = _bufferMsgs.erase(msgItor);
		}
	}

private:
	int32 _nodeType;
	std::list<ForwardMsg*> _bufferMsgs;
	bool  _start;
	sl::api::IKernel*	_kernel;
};
#endif
