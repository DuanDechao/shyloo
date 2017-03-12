#include "slnet_engine.h"
#include "slnet_session.h"
namespace sl
{
namespace core
{
NetEngine::~NetEngine(){
	m_pSLNetModule->release();
}

bool NetEngine::initialize()
{
	m_pSLNetModule = network::getSLNetModule();
	if(nullptr == m_pSLNetModule)
		return false;
	return true;
}

bool NetEngine::ready()
{
	return true;
}

bool NetEngine::destory()
{
	DEL this;
	return true;
}

bool NetEngine::addTcpServer(sl::api::ITcpServer* server, const char* ip, const short port, int sendSize, int recvSize)
{
	if(nullptr == m_pSLNetModule)
		return false;

	ISLListener* pListener = m_pSLNetModule->createListener();
	if (nullptr == pListener)
		return false;

	pListener->setBufferSize(recvSize, sendSize);
	pListener->setPacketParser(NEW NetPacketParser);
	pListener->setSessionFactory(NEW ServerSessionFactory(server));
	if (!pListener->start(ip, port)){
		//SLASSERT(false);
		return false;
	}
	return true;
}
bool NetEngine::addTcpClient(sl::api::ITcpSession* session, const char* ip, const short port, int sendSize, int recvSize)
{
	if (nullptr == m_pSLNetModule)
		return false;

	ISLConnector* pConnector = m_pSLNetModule->createConnector();
	if (nullptr == pConnector)
		return false;

	pConnector->setBufferSize(recvSize, sendSize);
	pConnector->setPacketParser(NEW NetPacketParser);
	NetSession* pNetSession = CREATE_POOL_OBJECT(NetSession, session);
	SLASSERT(pNetSession, "wtf");
	pConnector->setSession(pNetSession);
	if (!pConnector->connect(ip, port)){
		//SLASSERT(false, "connect failed");
		return false;
	}
	return true;
}

int64 NetEngine::loop(int64 overTime){
	int64 startTime = sl::getTimeMilliSecond();
	m_pSLNetModule->run(overTime);
	return sl::getTimeMilliSecond() - startTime;
}

}
}