#ifndef SL_LOGIC_TELNET_SESSION_H
#define SL_LOGIC_TELNET_SESSION_H
#include "slikernel.h"
#include "GameDefine.h"
#include "slpool.h"
#include <deque>

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

private:
	std::string getWelcome();
	void sendEnter();
	void sendDelChar();
	void sendNewLine();
	void checkAfterStr();
	void setWillGoHead();
	void setWillEcho();
	void setWontEcho();
	void sendClientEcho(const std::string& content);
	void resetStartPosition();
	bool checkUDLR(const std::string& cmd);
	std::string getInputStartString();
	bool processCommand();
	std::string getHistoryCommand(bool isNextCommand);
	void historyCommandCheck();

protected:
	friend sl::SLPool<TelnetSession>;
	
	TelnetSession(TelnetServer* pTelnetServer)
		:_telnetServer(pTelnetServer),
		 _curPos(0),
		 _handlerName("python")
	{}

	~TelnetSession(){}

private:
	TelnetServer*				_telnetServer;
	int32						_curPos;
	std::string					_command;
	std::deque<std::string>		_historyCommand;
	int32						_historyCommandIndex;
	static sl::SLPool<TelnetSession> s_pool;
	std::string					_handlerName;
};

#endif
