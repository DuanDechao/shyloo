#ifndef SL_LOGIC_NODE_SESSION_H
#define SL_LOGIC_NODE_SESSION_H
#include "slikernel.h"
#include "slobjectpool.h"
#include "slshm.h"
#include <string>

using namespace sl;
class Harbor;
#define RECONNECT_INTERVAL 1 * SECOND
class NodeSession : public sl::api::ITcpSession, public sl::api::ITimer{
	struct ShmMsgBuf{
		char buf[1024];
		int32 currSize;
		int32 msgSize;
	};
public:
	NodeSession()
		:m_pHarbor(nullptr),
		m_bReady(false),
		m_nodeType(0),
		m_nodeId(0),
		m_bConnect(false),
		m_ip(""),
		m_port(0),
		m_useShm(false)
	{
		m_msgBuf.currSize = 0;
		m_msgBuf.msgSize = 0;
	}

	NodeSession(Harbor* harbor)
		:m_pHarbor(harbor),
		 m_bReady(false),
		 m_nodeType(0),
		 m_nodeId(0),
		 m_bConnect(false),
		 m_ip(""),
		 m_port(0),
		 m_useShm(false)
	{
		m_msgBuf.currSize = 0;
		m_msgBuf.msgSize = 0;
	}
	virtual void onStart(sl::api::IKernel* pKernel, int64 timetick){}
	virtual void onTime(sl::api::IKernel* pKernel, int64 timetick);
	virtual void onTerminate(sl::api::IKernel* pKernel, bool beForced, int64 timetick){}
	virtual void onPause(sl::api::IKernel* pKernel, int64 timetick) {}
	virtual void onResume(sl::api::IKernel* pKernel, int64 timetick) {}

	virtual int32 onRecv(sl::api::IKernel* pKernel, const char* pContext, int dwLen);
	virtual void onConnected(sl::api::IKernel* pKernel);
	virtual void onDisconnect(sl::api::IKernel* pKernel);
	void setConnect(const char* ip, const int32 port);
	void send(const void* pContext, const int32 size);
	void prepareSendNodeMessage(const int32 messageId, const int32 size);

	std::string getShmKeyFile(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId);

private:
	Harbor*			m_pHarbor;
	bool			m_bReady;
	int32			m_nodeType;
	int32			m_nodeId;

	//主动连接相关
	bool			m_bConnect;
	std::string		m_ip;
	int32			m_port;
	
	bool			m_useShm;
	sl::shm::ISLShmQueue*	m_shmQueue;
	ShmMsgBuf       m_msgBuf;
};
CREATE_OBJECT_POOL(NodeSession);
#endif