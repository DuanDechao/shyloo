#include "slnet_engine.h"
#include "slnet_session.h"
namespace sl
{
namespace core
{

INetEngine* NetEngine::getInstance(){
	static NetEngine* p = nullptr;
	if (!p){
		p = NEW NetEngine;
		if (!p->ready()){
			SLASSERT(false, "netEngine not ready");
			DEL p;
			p = nullptr;
		}
	}
	return p;
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

	m_pListener = m_pSLNetModule->createListener();
	if(nullptr == m_pListener)
		return false;

	m_pListener->setBufferSize(recvSize, sendSize);
	m_pListener->setPacketParser(NEW NetPacketParser);
	m_pListener->setSessionFactory(NEW ServerSessionFactory(server));
	if(!m_pListener->start(ip, port)){
		//SLASSERT(false);
		return false;
	}
	return true;
}
bool NetEngine::addTcpClient(sl::api::ITcpSession* session, const char* ip, const short port, int sendSize, int recvSize)
{
	return true;
}

int64 NetEngine::processing(int64 overTime){
	m_pSLNetModule->run(overTime);
}

}
}