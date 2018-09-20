#ifndef SL_LOGIC_TELNET_SERVER_H
#define SL_LOGIC_YELNET_SERVER_H
#include "ITelnetServer.h"
#include "slikernel.h"
#include "TelnetSession.h"
#include "slsingleton.h"
#include <unordered_map>
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
	virtual void rgsTelnetHandler(const char* handlerName, ITelnetHandler* handler);
	ITelnetHandler* findTelnetHandler(const char* handlerName);

private:
	void startListening(sl::api::IKernel* pKernel);

private:
	sl::api::IKernel*	_pKernel;
	int32				_port;
	TelnetSessionTcpServer*	_pTelnetServer;
	std::unordered_map<std::string, ITelnetHandler*> _telnetHandlers;
};
#endif
