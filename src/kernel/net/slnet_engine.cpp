#include "slnet_engine.h"
#include "slnet_session.h"
#include <IPHlpApi.h>
using namespace sl::network;
namespace sl
{
namespace core
{

char g_localIpPrefix[MAX_LOCAL_IP_PREFIX_NUM][MAX_IP_LEN] = {
	"0.",
	"10.",
	"172.16.",
	"172.17.",
	"172.18.",
	"172.19.",
	"172.20.",
	"172.21.",
	"172.22.",
	"172.23.",
	"172.24.",
	"172.25.",
	"172.26.",
	"172.27.",
	"172.28.",
	"172.29.",
	"172.30.",
	"172.31.",
	"192.168.",
};

NetEngine::~NetEngine(){
	m_pSLNetModule->release();
}

bool NetEngine::initialize()
{
	readInternetIp();
	m_pSLNetModule = getSLNetModule();
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

bool NetEngine::addTcpServer(sl::api::ITcpServer* server, const char* ip, const short port, int sendSize, int recvSize){
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
bool NetEngine::addTcpClient(sl::api::ITcpSession* session, const char* ip, const short port, int sendSize, int recvSize){
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
	//ECHO_ERROR("netengine loop");
	return sl::getTimeMilliSecond() - startTime;
}

void NetEngine::readInternetIp(){
	sl::SafeMemset(m_ip, sizeof(m_ip), 0, sizeof(m_ip));
	PIP_ADAPTER_INFO adapters = (PIP_ADAPTER_INFO)SLMALLOC(sizeof(IP_ADAPTER_INFO));
	unsigned long size = sizeof(IP_ADAPTER_INFO);
	int32 ret = ::GetAdaptersInfo(adapters, &size);
	if (ERROR_BUFFER_OVERFLOW == ret){
		SLFREE(adapters);
		adapters = (PIP_ADAPTER_INFO)SLMALLOC(size);
		ret = ::GetAdaptersInfo(adapters, &size);
	}

	if (ERROR_SUCCESS == ret){
		const char* select = nullptr;
		PIP_ADAPTER_INFO adapter = adapters;
		while (adapter){
			if (adapter->Type == MIB_IF_TYPE_ETHERNET){
				PIP_ADDR_STRING ip = &(adapter->IpAddressList);
				while (ip){
					if (select == nullptr){
						select = ip->IpAddress.String;
						SafeSprintf(m_localIp, sizeof(m_localIp), "%s", ip->IpAddress.String);
					}
					else{
						if (!isLocalIp(ip->IpAddress.String)){
							select = ip->IpAddress.String;
						}
						else{
							SafeSprintf(m_localIp, sizeof(m_localIp), "%s", ip->IpAddress.String);
						}
					}
					ip = ip->Next;
				}
			}

			adapter = adapter->Next;
		}

		if (select){
			SafeSprintf(m_ip, sizeof(m_ip), "%s", select);
		}
	}

	SLFREE(adapters);
}

bool NetEngine::isLocalIp(const char* ip){
	for (int32 i = 0; i < MAX_LOCAL_IP_PREFIX_NUM; i++){
		int32 len = strlen(g_localIpPrefix[i]);
		if (strncmp(ip, g_localIpPrefix[i], len) == 0)
			return true;
	}
	return false;
}

}
}