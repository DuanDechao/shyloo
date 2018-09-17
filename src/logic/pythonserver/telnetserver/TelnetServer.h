#ifndef SL_LOGIC_TELNET_SERVER_H
#define SL_LOGIC_YELNET_SERVER_H
#include "IPythonServer.h"
#include "slikernel.h"
#include "TelnetSession.h"
#include "slsingleton.h"
class TelnetServer;
class TelnetSessionTcpServer : public sl::api::ITcpServer{
public:
	TelnetSessionTcpServer(TelnetServer* pTelnetServer) :_telnetServer(pTelnetServer){}
	virtual ~TelnetSessionTcpServer(){}
	virtual sl::api::ITcpSession* mallocTcpSession(sl::api::IKernel* pKernel);
	virtual void setListenPort(uint16 port){}

private:
	TelnetServer* _telnetServer;
};

class TelnetServer: public ITelnetServer, public sl::SLHolder<TelnetServer>{
public:
	virtual bool initialize(sl::api::IKernel * pKernel);
	virtual bool launched(sl::api::IKernel * pKernel);
	virtual bool destory(sl::api::IKernel * pKernel);

private:
	void startListening(sl::api::IKernel* pKernel);

private:
	sl::api::IKernel*	_pKernel;
	int32				_port;
	TelnetSessionTcpServer*	_pTelnetServer;
};
#endif
