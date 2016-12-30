#ifndef SL_KERNEL_H
#define SL_KERNEL_H
#include "sliapplication.h"
#include "slikernel.h"
#include "slsingleton.h"
#include "slimodule.h"
#include "slmulti_sys.h"
#include <unordered_map>
namespace sl
{
namespace core
{
class Kernel: public api::IKernel,public IApplication
{
public:
	static IKernel* getInstance();

	virtual bool ready();
	virtual bool initialize(int32 argc, char ** argv);
	virtual bool destory(){ return false; }

	virtual api::IModule * findModule(const char * name){ return nullptr; }

	//net interface
	virtual bool startTcpServer(api::ITcpServer * server, const char* ip, const int32 port, int32 sendSize, int32 recvSize){ return false; }
	virtual bool startTcpClient(api::ITcpSession * client, const char* ip, const int32 port, int32 sendSize, int32 recvSize){ return false; }

	//timer interface
	virtual bool startTimer(api::ITimer* timer, int64 delay, int32 count, int64 interval){ return false; }
	virtual bool killTimer(api::ITimer* timer){ return false; }
	virtual void pauseTimer(api::ITimer* timer){ return; }
	virtual void resumeTimer(api::ITimer* timer){ return; }

	void loop();

	const char* getCmdArg(const char* name);

	const char* getCoreFile();

public:
	Kernel(){}
	~Kernel(){}

	void parse(int argc, char** argv);

private:
	bool	m_bShutDown;

	SLUnordered_map<std::string, std::string> _args;
};
}
}
#endif