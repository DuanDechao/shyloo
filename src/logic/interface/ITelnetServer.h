#ifndef _SL_INTERFACE_TELNETSERVER_H__
#define _SL_INTERFACE_TELNETSERVER_H__
#include "slimodule.h"

class ITelnetHandler{
public:
	virtual ~ITelnetHandler(){}
	virtual bool processTelnetCommand(std::string cmd, std::string& retBuf) = 0;
};

class ITelnetServer: public sl::api::IModule{
public:
	virtual ~ITelnetServer() {}
	virtual void rgsTelnetHandler(const char* handlerName, ITelnetHandler* handler) = 0;

};

#endif
