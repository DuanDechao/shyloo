#ifndef __SL_CLINET_SESSION_H__
#define __SL_CLINET_SESSION_H__
#include "slikernel.h"
#include "slobjectpool.h"

class Client;
class ClientSession : public sl::api::ITcpSession, public sl::api::ITimer
{
public:
	ClientSession();
	ClientSession(Client* pClient);
	~ClientSession();

	virtual void onStart(sl::api::IKernel* pKernel, int64 timetick){}
	virtual void onTime(sl::api::IKernel* pKernel, int64 timetick);
	virtual void onTerminate(sl::api::IKernel* pKernel, bool beForced, int64 timetick){}
	virtual void onPause(sl::api::IKernel* pKernel, int64 timetick) {}
	virtual void onResume(sl::api::IKernel* pKernel, int64 timetick) {}
	
	void setConnect(const char* ip, const int32 port);

	virtual int32 onRecv(sl::api::IKernel* pKernel, const char* pContext, int dwLen);
	virtual void onConnected(sl::api::IKernel* pKernel);
	virtual void onDisconnect(sl::api::IKernel* pKernel);

private:
	int64			m_id;
	Client*			m_client;

	bool			m_bConnect;
	std::string		m_ip;
	int32			m_port;
};

CREATE_OBJECT_POOL(ClientSession);
#endif