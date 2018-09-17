#ifndef SL_LOGIC_TELNET_SESSION_H
#define SL_LOGIC_TELNET_SESSION_H
#include "slikernel.h"
#include "GameDefine.h"
#include "slpool.h"

using namespace sl;
class TelnetServer;
class TelnetSession : public sl::api::ITcpSession{
public:
	static TelnetSession* create(TelnetServer* telnetServer){
		return CREATE_FROM_POOL(s_pool, telnetServer);
	}

	void release(){
		s_pool.recover(this);
	}

	virtual int32 onRecv(sl::api::IKernel* pKernel, const char* pContext, int dwLen);
	virtual void onConnected(sl::api::IKernel* pKernel);
	virtual void onDisconnect(sl::api::IKernel* pKernel);
	
	void send(const void* pContext, const int32 size);

protected:
	friend sl::SLPool<TelnetSession>;
	
	TelnetSession(TelnetServer* pTelnetServer)
		:_telnetServer(pTelnetServer)
	{}

	~TelnetSession(){}

private:
	TelnetServer*			_telnetServer;
	static sl::SLPool<TelnetSession> s_pool;
};

#endif
