#include "slendpoint.h"
#include "slbundle.h"
#include "slpacket_sender.h"
namespace sl
{
namespace network
{

static bool g_networkInitted = false;
static CObjectPool<EndPoint> g_objPool("EndPoint");
CObjectPool<EndPoint>& EndPoint::ObjPool()
{
	return g_objPool;
}

EndPoint* EndPoint::createPoolObject()
{
	return g_objPool.FetchObj();
}

void EndPoint::reclaimPoolObject(EndPoint* obj)
{
	g_objPool.ReleaseObj(obj);
}

void EndPoint::destroyObjPool()
{
	g_objPool.Destroy();
}

EndPoint::SmartPoolObjectPtr EndPoint::createSmartPoolObj()
{
	return SmartPoolObjectPtr(new SmartPoolObject<EndPoint>(ObjPool().FetchObj(), g_objPool));
}

void EndPoint::onReclaimObject()
{
#ifdef SL_OS_WINDOWS
	m_socket = INVALID_SOCKET;
#else
	m_socket = -1;
#endif

	m_address = Address::NONE;
}



inline EndPoint::EndPoint(uint32 networkAddr /* = 0 */, uint16 networkport /* = 0 */)
	:m_socket(INVALID_SOCKET)
{
	if(networkAddr)
	{
		m_address.m_ip		= networkAddr;
		m_address.m_port	= networkport;
	}
}

inline EndPoint::EndPoint(Address address)
	:m_socket(INVALID_SOCKET)
{
	if(address.m_ip > 0)
	{
		m_address = address;
	}
}

inline EndPoint::~EndPoint()
{
	this->close();
}

uint32 EndPoint::getRTT()
{
	return 0;
}

inline bool EndPoint::good() const
{
	return m_socket != INVALID_SOCKET;
}

inline EndPoint::operator SLSOCKET() const
{
	return m_socket;
}

inline SLSOCKET EndPoint::socket() const
{
	return m_socket;
}

inline void EndPoint::setFileDescriptor(int fd)
{
	m_socket = fd;
}

inline void EndPoint::socket(int type)
{
	this->setFileDescriptor((int)::socket(AF_INET, type, 0));
	if((m_socket == INVALID_SOCKET) && (WSAGetLastError() == WSANOTINITIALISED))
	{
		EndPoint::initNetwork();
		this->setFileDescriptor((int)::socket(AF_INET, type, 0));
	}
}

inline int EndPoint::setnodelay(bool nodelay /* = true */)
{
	int arg = int(nodelay);
	return setsockopt(m_socket, IPPROTO_TCP, TCP_NODELAY, (char*)&arg, sizeof(int));
}

inline int EndPoint::setnonblocking(bool nonblocking)
{
#ifdef SL_OS_WINDOWS
	u_long val = nonblocking ? 1: 0;
	return ::ioctlsocket(m_socket, FIONBIO, &val);
#else
	int val = nonblocking ? O_NONBLOCK : 0;
	return ::fcntl(m_socket, F_SETFL, val);
#endif
}

inline int EndPoint::setbroadcast(bool broadcast)
{
#ifdef SL_OS_WINDOWS
	 bool val;
#else
	int val;
	if(broadcast)
	{
		val  = 2;
		::setsockopt(m_socket, SOL_IP, IP_MULTICAST_TTL, &val, sizeof(int));
	}
#endif
	val  = broadcast ? 1 : 0;
	return ::setsockopt(m_socket, SOL_SOCKET, SO_BROADCAST, (char*)&val, sizeof(val));
}

inline int EndPoint::setreuseaddr(bool reuseaddr)
{
#ifdef SL_OS_WINDOWS
	bool val;
#else
	int val;
#endif
	val = reuseaddr ? 1: 0;
	return setsockopt(m_socket, SOL_SOCKET, SO_REUSEADDR, (char*)&val, sizeof(val));
}

inline int EndPoint::setlinger(uint16 onoff, uint16 linger)
{
	struct linger l = {0};
	l.l_onoff = onoff;
	l.l_linger = linger;
	return setsockopt(m_socket, SOL_SOCKET, SO_LINGER, (const char*)&l, sizeof(l));
}

inline int EndPoint::setkeepalive(bool keepalive)
{
#ifdef SL_OS_WINDOWS
	bool val;
#else
	int val;
#endif
	val = keepalive ? 1 : 0;
	return setsockopt(m_socket, SOL_SOCKET, SO_KEEPALIVE, (char*)&val, sizeof(val));
}

inline int EndPoint::bind(uint16 networkPort, uint32 networkAddr)
{
	sockaddr_in sin;
	memset(&sin, 0, sizeof(sockaddr_in));
	sin.sin_family = AF_INET;
	sin.sin_port = networkPort;
	sin.sin_addr.s_addr = networkAddr;
	return ::bind(m_socket, (struct sockaddr*)&sin, sizeof(sin));
}

inline int EndPoint::joinMulticastGroup(uint32 networkAddr)
{
#ifdef SL_OS_WINDOWS
	return -1;
#else
	struct ip_mreqn req;
#endif
}

inline int EndPoint::quitMulticastGroup(uint32 networkAddr)
{
#ifdef SL_OS_WINDOWS
	return -1;
#else
	struct ip_mreqn req;
#endif
}

inline int EndPoint::close()
{
	if(m_socket == -1)
	{
		return 0;
	}
#ifdef SL_OS_LINUX
	int ret = close(m_socket);
#else
	int ret = closesocket(m_socket);
#endif

	if(ret == 0)
	{
		this->setFileDescriptor(-1);
	}
	
	return ret;
}

inline int EndPoint::getlocaladdress(uint16* networkPort, uint32* networkAddr) const
{
	sockaddr_in		sin;
	socklen_t		sinLen = sizeof(sin);

	int ret = getsockname(m_socket, (struct sockaddr*)&sin, &sinLen);
	if(ret == 0)
	{
		if(networkPort != NULL) *networkPort = sin.sin_port;
		if(networkAddr != NULL) *networkAddr = sin.sin_addr.s_addr;
	}
	return ret;
}

inline int EndPoint::getremoteaddress(uint16* networkPort, uint32* networkAddr) const
{
	sockaddr_in		sin;
	socklen_t		sinLen = sizeof(sin);

	int ret = ::getpeername(m_socket, (struct sockaddr*)&sin, &sinLen);
	if(ret == 0)
	{
		if(networkAddr != NULL) *networkAddr = sin.sin_addr.s_addr;
		if(networkPort != NULL) *networkPort = sin.sin_port;
	}
	return ret;
}

inline const char* EndPoint::c_str() const
{
	return m_address.c_str();
}

inline const Address& EndPoint::addr() const
{
	return m_address;
}

inline void EndPoint::addr(const Address& newAddress)
{
	m_address = newAddress;
}

inline void EndPoint::addr(uint16 newNeteorkPort, uint32 newNetworkAddress)
{
	m_address.m_port = newNeteorkPort;
	m_address.m_ip = newNetworkAddress;
}

inline int EndPoint::getremotehostname(std::string* host) const
{
	sockaddr_in sin;
	socklen_t sinlen = sizeof(sin);

	int ret = getpeername(m_socket, (struct sockaddr*)&sin, &sinlen);
	if(ret == 0)
	{
		hostent* h = gethostbyaddr((char*)&sin.sin_addr, sizeof(sin.sin_addr), AF_INET);
		if(h)
		{
			*host = h->h_name;
		}
		else
		{
			ret = -1;
		}
	}

	return ret;
}

inline int EndPoint::sendto(void* gramData, int gramSize, uint16 networkPort, uint32 networkAddr)
{
	sockaddr_in sin;
	sin.sin_family = AF_INET;
	sin.sin_port = networkPort;
	sin.sin_addr.s_addr = networkAddr;

	return this->sendto(gramData, gramSize, sin);
}

inline int EndPoint::sendto(void* gramData, int gramSize, struct sockaddr_in& sin)
{
	return ::sendto(m_socket, (char*)gramData, gramSize, 0, (sockaddr*)&sin, sizeof(sin)); 
}

inline int EndPoint::recvfrom(void* gramData, int gramSize, uint16* networkPort, uint32* networkAddr)
{
	sockaddr_in sin;
	int result = this->recvfrom(gramData, gramSize, sin);

	if(result >= 0)
	{
		if(networkAddr != NULL) *networkAddr = sin.sin_addr.s_addr;
		if(networkPort != NULL) *networkPort = sin.sin_port;
	}
	return result;
}

inline int EndPoint::recvfrom(void* gramData, int gramSize, struct sockaddr_in& sin)
{
	socklen_t sinlen = sizeof(sin);
	int ret = ::recvfrom(m_socket, (char*)gramData, gramSize, 0, (sockaddr*)&sin, &sinlen);
	return ret;
}

inline int EndPoint::listen(int backlog)
{
	return ::listen(m_socket, backlog);
}

inline int EndPoint::connect(bool autosetflags /* = true */)
{
	return connect(m_address.m_port, m_address.m_ip, autosetflags);
}

inline int EndPoint::connect(uint16 networkport, uint32 networkAddr /* = INADDR_BROADCAST */, bool autosetflags /* = true */)
{
	sockaddr_in sin;
	sin.sin_family = AF_INET;
	sin.sin_port  = networkport;
	sin.sin_addr.s_addr = networkAddr;

	int ret = ::connect(m_socket, (sockaddr*)&sin, sizeof(sin));
	if(autosetflags)
	{
		setnonblocking(true);
		setnodelay(true);
	}
}

inline EndPoint* EndPoint::accept(uint16* networkPort /* = NULL */, uint32* networkAddr /* = NULL */, bool autosetflags /* = true */)
{
	sockaddr_in sin;
	socklen_t sinLen = sizeof(sin);
	int ret = (int)::accept(m_socket, (sockaddr*)&sin, &sinLen);
#ifdef SL_OS_LINUX
	if(ret < 0) return NULL;
#else
	if(ret == INVALID_SOCKET) return NULL;
#endif

	EndPoint* pNew = EndPoint::createPoolObject();
	pNew->setFileDescriptor(ret);
	pNew->addr(sin.sin_port, sin.sin_addr.s_addr);

	if(autosetflags)
	{
		pNew->setnonblocking(true);
		pNew->setnodelay(true);
	}

	if(networkAddr != NULL) *networkPort = sin.sin_port;
	if(networkPort != NULL) *networkAddr = sin.sin_addr.s_addr;

	return pNew;
}

inline int EndPoint::send(const void* gramData, int gramSize)
{
	return ::send(m_socket, (char*)gramData, gramSize, 0);
}

inline int EndPoint::recv(void* gramData, int gramSize)
{
	return ::recv(m_socket, (char*)gramData, gramSize, 0);
}

#ifdef SL_OS_LINUX
inline int EndPoint::getInterfaceFlags(char* name, int& flag)
{

}
#else
inline int EndPoint::getInterfaceFlags(char* name, int& flag)
{
	if(!strcmp(name, "eth0"))
	{
		flag = IFF_UP | IFF_BROADCAST | IFF_NOTRAILERS |
			IFF_RUNNING | IFF_MULTICAST;
		return 0;
	}

	else if(strcmp(name, "lo"))
	{
		flag = IFF_UP | IFF_LOOPBACK | IFF_RUNNING;
		return 0;
	}

	return -1;
}

inline int EndPoint::getInterfaceAddress(const char* name, uint32& address)
{
	if(!strcmp(name, "eth0"))
	{
		char myname[256];
		::gethostname(myname, sizeof(myname));

		struct  hostent* myhost = gethostbyname(myname);
		if(!myhost)
		{
			return -1;
		}

		address = ((struct in_addr*)(myhost->h_addr_list[0]))->s_addr;
		return 0;
	}
	else if(!strcmp(name, "lo"))
	{
		address = htonl(0x7F000001);
		return 0;
	}
	return -1;
}
#endif

//---------------------------------------------------------------

bool EndPoint::getClosedPort(network::Address& closedPort)
{
	bool isResultSet = false;

#ifdef SL_OS_LINUX
#else

#endif
	return isResultSet;
}

int EndPoint::getBufferSize(int optname) const
{
	SL_ASSERT(optname == SO_SNDBUF || optname == SO_RCVBUF);

	int recvbuf = -1;
	socklen_t rbargssize = sizeof(int);

	int rberr = getsockopt(m_socket, SOL_SOCKET, optname, (char*)&recvbuf, &rbargssize);

	if(rberr == 0 && rbargssize == sizeof(int))
		return recvbuf;

	return -1;
}

bool EndPoint::getInterfaces(std::map<uint32, std::string>& interfaces)
{
#ifdef SL_OS_WINDOWS
	int count = 0;
	char hostname[1024];
	struct hostent* inaddrs;

	if(gethostname(hostname, 1024) == 0)
	{
		inaddrs = gethostbyname(hostname);
		if(inaddrs)
		{
			while(inaddrs->h_addr_list[count])
			{
				unsigned long addrs = *(unsigned long*)inaddrs->h_addr_list[count];
				interfaces[addrs] = "eth0";
				char* ip = inet_ntoa(*(struct in_addr*)inaddrs->h_addr_list[count]);
				//SL_DEBUG("EndPoint::getInterfaces: found eth0 %s\n", ip);
				++count;
			}
		}
	}
	return count > 0;
#else
#endif
}

int EndPoint::findIndicatedInterface(const char* spec, uint32& address)
{
	address = 0;
	if(spec == NULL || spec[0] == 0)
	{
		return -1;
	}

	if(0 != Address::string2ip(spec, address))
	{
		return -1;
	}
	else if(0 != this->getInterfaceAddressByMAC(spec, address))
	{
		return -1;
	}
	else if(0 != this->getInterfaceAddressByName(spec, address))
	{
		return -1;
	}

	return 0;
}

int EndPoint::getInterfaceAddressByName(const char* name, uint32& address)
{
	int ret = -1;
#ifdef SL_OS_WINDOWS

	PIP_ADAPTER_INFO pIpAdapterInfo = new IP_ADAPTER_INFO();
	unsigned long size = sizeof(IP_ADAPTER_INFO);

	int ret_info = ::GetAdaptersInfo(pIpAdapterInfo, &size);

	if(ERROR_BUFFER_OVERFLOW == ret_info)
	{
		delete pIpAdapterInfo;
		pIpAdapterInfo = (PIP_ADAPTER_INFO)new unsigned char[size];
		ret_info = ::GetAdaptersInfo(pIpAdapterInfo, &size);
	}

	if(ERROR_SUCCESS == ret_info)
	{
		PIP_ADAPTER_INFO _pIpAdapterInfo = pIpAdapterInfo;
		while(_pIpAdapterInfo)
		{
			if(!strcmp(_pIpAdapterInfo->AdapterName, name))
			{
				IP_ADDR_STRING* pIpAddrString = &(_pIpAdapterInfo->IpAddressList);
				ret = Address::string2ip(pIpAddrString->IpAddress.String, address);
				break;
			}

			_pIpAdapterInfo = _pIpAdapterInfo->Next;
		}
	}

	if(pIpAdapterInfo)
	{
		delete pIpAdapterInfo;
	}
#else
#endif
	return ret;

}	

bool EndPoint::setBufferSize(int optname, int size)
{
	setsockopt(m_socket, SOL_SOCKET, optname, (const char*)&size, sizeof(size));
	return this->getBufferSize(optname) >= size;
}

bool EndPoint::recvAll(void* gramData, int gramSize)
{
	while(gramSize > 0)
	{
		int len = this->recv(gramData, gramSize);
		if(len <= 0)
		{
			if(len == 0)
			{
				//warning
			}else
			{
				//warning
			}
			return false;
		}
		gramSize -= len;
		gramData = ((char*)gramData) + len;
	}
	return true;
}

network::Address EndPoint::getlocalAddress() const
{
	network::Address addr(0,0);

	if(this->getlocaladdress((uint16*)&addr.m_port, (uint32*)&addr.m_ip) == -1)
	{

	}
	return addr;
}

network::Address EndPoint::getremoteAddress() const
{
	network::Address addr(0,0);
	if(this->getremoteaddress((uint16*)&addr.m_port,
		(uint32*)&addr.m_ip) == -1)
	{

	}
	return addr;
}

void EndPoint::initNetwork()
{
	if(g_networkInitted)
		return;

	g_networkInitted = true;

#ifdef SL_OS_WINDOWS
	WSAData wsdata;
	WSAStartup(0x202, &wsdata);
#endif // SL_OS_WINDOWS
}

bool EndPoint::waitSend()
{
	fd_set fds;
	struct timeval tv = {0, 10000};
	FD_ZERO(&fds);
	FD_SET(m_socket, &fds);

	return select(m_socket + 1, NULL, &fds, NULL, &tv) > 0;
}

void EndPoint::send(Bundle* pBundle)
{
	SEND_BUNDLE((*this), (*pBundle));
}
void EndPoint::sendto(Bundle* pBundle, uint16 networkPort, uint32 networkAddr /* = BROADCAST */)
{
	SENDTO_BUNDLE((*this), networkAddr, networkPort, (*pBundle));
}

}
}