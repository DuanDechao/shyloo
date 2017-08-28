#include "slnet_engine.h"
#include "slnet_session.h"
#include "sltime.h"
#include "slxml_reader.h"
#ifdef SL_OS_WINDOWS
#include <IPHlpApi.h>
#else
#include <ifaddrs.h>
#include <signal.h>
#include <arpa/inet.h>
#endif

using namespace sl::network;
namespace sl{
namespace core{

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
	if (m_pSLNetModule)
		m_pSLNetModule->release();
}

bool NetEngine::initialize(){
	char path[MAX_PATH] = { 0 };
	SafeSprintf(path, sizeof(path), "%s/core/server_conf.xml", sl::getAppPath());
	XmlReader server_conf;
	if (!server_conf.loadXml(path)){
		SLASSERT(false, "not find core file %s", path);
		return false;
	}

	if (server_conf.root()["server"][0].hasAttribute("pubIp")){
		const char* pubIp = server_conf.root()["server"][0].getAttributeString("pubIp");
		SafeSprintf(m_ip, sizeof(m_ip), "%s", pubIp);
	}

	if (strcmp(m_ip, "") == 0)
		readInternetIp();

	m_pSLNetModule = getSLNetModule();
	if(nullptr == m_pSLNetModule)
		return false;
	return true;
}

bool NetEngine::ready(){
	return true;
}

bool NetEngine::destory(){
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
	NetSession* pNetSession = NetSession::create(session);
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
	m_pSLNetModule->run(overTime / 5);
	//ECHO_ERROR("netengine loop");
	return sl::getTimeMilliSecond() - startTime;
}

void NetEngine::readInternetIp(){
#ifdef SL_OS_WINDOWS
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
#else
    sl::SafeMemset(m_ip, sizeof(m_ip), 0, sizeof(m_ip));
	ifaddrs* addrs = nullptr;
	getifaddrs(&addrs);
    
    bool first = true;
    for(ifaddrs* addr = addrs; addr; addr = addr->ifa_next){
		if(strcmp(addr->ifa_name, "lo") == 0)
			continue;
		
		if(addr->ifa_addr->sa_family == AF_INET){
			char ip[MAX_IP_LEN];
			inet_ntop(AF_INET, &(((sockaddr_in*)addr->ifa_addr)->sin_addr), ip, MAX_IP_LEN);
			if(strcmp(ip, "127.0.0.1") != 0){
				if(first){
					SafeSprintf(m_ip, sizeof(m_ip), "%s", ip);
					first = false;
				}
				else{
					if(!isLocalIp(ip)){
						SafeSprintf(m_ip, sizeof(m_ip), "%s", ip);
					}
				}
			}
		}
	}
	freeifaddrs(addrs);
#endif
}

bool NetEngine::isLocalIp(const char* ip){
	for (int32 i = 0; i < MAX_LOCAL_IP_PREFIX_NUM; i++){
		int32 len = (int32)strlen(g_localIpPrefix[i]);
		if (strncmp(ip, g_localIpPrefix[i], len) == 0)
			return true;
	}
	return false;
}

}
}
