#ifndef SL_KERNEL_H
#define SL_KERNEL_H
#include "sliapplication.h"
#include "slikernel.h"
#include "slsingleton.h"
#include "slimodule.h"
#include "slmulti_sys.h"
#include <unordered_map>
#include "sllog.h"
namespace sl
{
namespace core
{
class Kernel: public api::IKernel, public IApplication{
public:
	static Kernel* getInstance();

	virtual bool ready();
	virtual bool initialize(int32 argc, char ** argv);
	virtual bool destory();

	virtual api::IModule * findModule(const char * name);

	//net interface
	virtual bool startTcpServer(api::ITcpServer * server, const char* ip, const int32 port, int32 sendSize = 0, int32 recvSize = 0);
	virtual bool startTelnetServer(sl::api::ITcpServer* server, const char* ip, const int32 port);
	virtual bool startTcpClient(api::ITcpSession * client, const char* ip, const int32 port, int32 sendSize = 0, int32 recvSize = 0);
	virtual const char* getInternetIp();
	virtual const char* getLocalIp();
	virtual uint64 getSpareTime();
	virtual void clearSpareTime();

	//ipc interface
	virtual bool addIPCServer(sl::api::ITcpServer* server, const int64 serverId);
	virtual bool addIPCClient(sl::api::ITcpSession* session, const int64 clientId, const int64 serverId, const int32 sendSize = 0, const int32 recvSize = 0);

	//timer interface
	virtual bool startTimer(api::ITimer* timer, int64 delay, int32 count, int64 interval, const char* file, const int32 line);
	virtual bool killTimer(api::ITimer* timer);
	virtual void pauseTimer(api::ITimer* timer);
	virtual void resumeTimer(api::ITimer* timer);

	
	// config interface
	virtual const char* getCmdArg(const char* name);
	virtual bool reloadCoreConfig(const char* coreFile);

	//async interface
	virtual void startAsync(const int64 threadId, api::IAsyncHandler* handler, const char* debug);
	virtual void stopAsync(api::IAsyncHandler* handler);

	//log interface
	virtual void syncLog(int32 filter, const char* log, const char* file, const int32 line);
//yy	virtual void asyncLog(int32 filter, const char* log, const char* file, const int32 line);
	virtual sl::api::ILogger* createLogger();

	virtual void shutdown() { _shutDown = true; }
	virtual bool isShutdown() { return _shutDown; }
	
	void loop();

	sl::api::ILogger* kernelLogger() {return _kernelLogger;}

public:
	Kernel(){}
	~Kernel(){}

	void parse(int argc, char** argv);

private:
	bool		_shutDown;
	std::unordered_map<std::string, std::string> _args;
	sl::api::ILogger*	_kernelLogger;
};
}
}

#define KERNEL_TRACE(format, a...)			sl::core::Kernel::getInstance()->kernelLogger()->traceLog(format, ##a)
#define KERNEL_INFO(format, a...)			sl::core::Kernel::getInstance()->kernelLogger()->infoLog(format, ##a)
#define KERNEL_DEBUG(format, a...)			sl::core::Kernel::getInstance()->kernelLogger()->debugLog(format, ##a)
#define KERNEL_WARNING(format, a...)        sl::core::Kernel::getInstance()->kernelLogger()->warningLog(format, ##a)
#define KERNEL_ERROR(format, a...)			sl::core::Kernel::getInstance()->kernelLogger()->errorLog(format, ##a)
#define KERNEL_FATAL(format, a...)			sl::core::Kernel::getInstance()->kernelLogger()->fatalLog(format, ##a)

#endif
