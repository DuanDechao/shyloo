#ifndef SL_KERNEL_H
#define SL_KERNEL_H
#include "slikernel.h"
#include "slsingleton.h"
#include "slimodule.h"
#include "slmulti_sys.h"
#include <unordered_map>
namespace sl
{
namespace core
{
class Kernel: public api::IKernel, public CSingleton<Kernel>
{
public:
	virtual bool ready();
	virtual bool initialize(int32 argc, char ** argv);
	virtual bool destory();

	virtual api::IModule * findModule(const char * name);

	//net interface
	virtual bool startTcpServer(api::ITcpServer * server, const char* ip, const int32 port, int32 sendSize, int32 recvSize);
	virtual bool startTcpClient(api::ITcpSession * client, const char* ip, const int32 port, int32 sendSize, int32 recvSize);

	//timer interface
	virtual bool startTimer(api::ITimer* timer, int64 delay, int32 count, int64 interval, const char * debug);
	virtual bool killTimer(api::ITimer* timer);
	virtual void pauseTimer(api::ITimer* timer);
	virtual void resumeTimer(api::ITimer* timer);

	void loop();
private:
	Kernel();
	~Kernel();

	void parse(int argc, char** argv);

	

private:
	bool	m_bShutDown;

	SLUnordered_map<std::string, std::string> _args;
};
}
}
#endif