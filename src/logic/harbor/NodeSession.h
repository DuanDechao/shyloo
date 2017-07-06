#ifndef SL_LOGIC_NODE_SESSION_H
#define SL_LOGIC_NODE_SESSION_H
#include "slikernel.h"
#include "slshm.h"
#include "slstring.h"
#include "GameDefine.h"
#include "slpool.h"

using namespace sl;
class Harbor;
#define RECONNECT_INTERVAL 1 * SECOND
class NodeSession : public sl::api::ITcpSession, public sl::api::ITimer{
public:
	static NodeSession* create(Harbor* harbor){
		return CREATE_FROM_POOL(s_pool, harbor);
	}

	void release(){
		s_pool.recover(this);
	}

	virtual void onStart(sl::api::IKernel* pKernel, int64 timetick){}
	virtual void onTime(sl::api::IKernel* pKernel, int64 timetick);
	virtual void onTerminate(sl::api::IKernel* pKernel, bool beForced, int64 timetick){}
	virtual void onPause(sl::api::IKernel* pKernel, int64 timetick) {}
	virtual void onResume(sl::api::IKernel* pKernel, int64 timetick) {}

	virtual int32 onRecv(sl::api::IKernel* pKernel, const char* pContext, int dwLen);
	virtual void onConnected(sl::api::IKernel* pKernel);
	virtual void onDisconnect(sl::api::IKernel* pKernel);
	
	void send(const void* pContext, const int32 size);
	void prepareSendNodeMessage(const int32 messageId, const int32 size);

	inline void setConnect(const char* ip, const int32 port){
		_ip = ip;
		_port = port;
		_connect = true;
	}

	inline void setNodeInfo(const int32 nodeType, const int32 nodeId){
		_nodeType = nodeType;
		_nodeId = nodeId;
	}

	inline void setIpcTransfor(bool isIpc) { _ipcTransfor = isIpc; }
	inline bool isIpcTransfor() const { return _ipcTransfor; }

protected:
	friend sl::SLPool<NodeSession>;
	
	NodeSession(Harbor* harbor)
		:_harbor(harbor),
		_ready(false),
		_nodeType(0),
		_nodeId(0),
		_connect(false),
		_port(0),
		_ip(""),
		_ipcTransfor(false)
	{}

	~NodeSession(){}

private:
	Harbor*			_harbor;
	bool			_ready;
	int32			_nodeType;
	int32			_nodeId;

	//主动连接相关
	bool			_connect;
	int32			_port;
	sl::SLString<game::MAX_IP_LEN>	_ip;
	bool			_ipcTransfor;

	static sl::SLPool<NodeSession> s_pool;
};

#endif
