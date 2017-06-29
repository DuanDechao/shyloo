#ifndef __SL_CLINET_SESSION_H__
#define __SL_CLINET_SESSION_H__
#include "slikernel.h"

class Client;
class ClientSession : public sl::api::ITcpSession, public sl::api::ITimer{
public:
	ClientSession(Client* pClient) : _client(pClient), _ip(""), _port(0) {}
	virtual ~ClientSession(){
		_client = nullptr;
		_ip = "";
		_port = 0;
	}

	virtual void onStart(sl::api::IKernel* pKernel, int64 timetick){}
	virtual void onTime(sl::api::IKernel* pKernel, int64 timetick);
	virtual void onTerminate(sl::api::IKernel* pKernel, bool beForced, int64 timetick){}
	virtual void onPause(sl::api::IKernel* pKernel, int64 timetick) {}
	virtual void onResume(sl::api::IKernel* pKernel, int64 timetick) {}
	
	inline void setConnect(const char* ip, const int32 port){
		_ip = ip;
		_port = port;
	}

	virtual int32 onRecv(sl::api::IKernel* pKernel, const char* pContext, int dwLen);
	virtual void onConnected(sl::api::IKernel* pKernel);
	virtual void onDisconnect(sl::api::IKernel* pKernel);

private:
	Client*			_client;
	std::string		_ip;
	int32			_port;
};
#endif