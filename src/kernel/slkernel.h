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
class Kernel: public api::IKernel,public IApplication{
public:
	static IKernel* getInstance();

	virtual bool ready();
	virtual bool initialize(int32 argc, char ** argv);
	virtual bool destory();

	virtual api::IModule * findModule(const char * name);

	//net interface
	virtual bool startTcpServer(api::ITcpServer * server, const char* ip, const int32 port, int32 sendSize, int32 recvSize);
	virtual bool startTcpClient(api::ITcpSession * client, const char* ip, const int32 port, int32 sendSize, int32 recvSize);
	virtual const char* getInternetIp();
	virtual const char* getLocalIp();

	//ipc interface
	virtual bool addIPCServer(sl::api::ITcpServer* server, const int64 serverId);
	virtual bool addIPCClient(sl::api::ITcpSession* session, const int64 clientId, const int64 serverId, const int32 sendSize, const int32 recvSize);

	//timer interface
	virtual bool startTimer(api::ITimer* timer, int64 delay, int32 count, int64 interval, const char* file, const int32 line);
	virtual bool killTimer(api::ITimer* timer);
	virtual void pauseTimer(api::ITimer* timer);
	virtual void resumeTimer(api::ITimer* timer);

	
	// config interface
	virtual const char* getCmdArg(const char* name);
	virtual const char* getCoreFile();
	virtual const char* getConfigFile();
	virtual const char* getEnvirPath();
	virtual const char* getIpcPath();

	//async interface
	virtual void startAsync(const int64 threadId, api::IAsyncHandler* handler, const char* debug);
	virtual void stopAsync(api::IAsyncHandler* handler);

	//log interface
	virtual void syncLog(int32 filter, const char* log, const char* file, const int32 line);
	virtual void asyncLog(int32 filter, const char* log, const char* file, const int32 line);

	virtual void shutdown() { _shutDown = true; }
	virtual bool isShutdown() { return _shutDown; }
	void loop();

public:
	Kernel(){}
	~Kernel(){}

	void parse(int argc, char** argv);

private:
	bool	_shutDown;
	std::unordered_map<std::string, std::string> _args;
};
}
}

#define KERNEL_LOG(format, ...) { \
	char log[8192]; \
	SafeSprintf(log, 8192, format, ##__VA_ARGS__); \
	Kernel::getInstance()->asyncLog(0x20, log, __FILE__, __LINE__); \
}

#define KERNEL_ERROR(format, ...) { \
	char log[8192]; \
	SafeSprintf(log, 8192, format, ##__VA_ARGS__); \
	Kernel::getInstance()->asyncLog(0x2, log, __FILE__, __LINE__); \
}

#endif