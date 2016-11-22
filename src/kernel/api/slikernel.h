#ifndef SL_KERNEL_IKERNEL_H
#define SL_KERNEL_IKERNEL_H
namespace sl
{
namespace api
{
class IKernel;
class IPipe
{
public:
	virtual ~IPipe() {}
	virtual void send(const void* pContext, int dwLen) = 0;
	virtual void close() = 0;
};

class ISocket
{
public:
	virtual ~ISocket(){}
	IPipe* m_pPipe;
};

class ITcpSession: public ISocket
{
public:
	virtual ~ITcpSession(){}
	virtual void onRecv(IKernel* pKernel, const char* pContext, int dwLen) = 0;
};

class ITcpServer
{
public:
	virtual ~ITcpServer(){}
	virtual ITcpSession* mallocTcpSession(IKernel* pKernel) = 0;
};


class IKernel
{

};
}
}

#endif