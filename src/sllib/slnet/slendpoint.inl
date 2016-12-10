namespace sl{
namespace network{
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

inline void EndPoint::setFileDescriptor(int32 fd)
{
	m_socket = fd;
}

inline void EndPoint::socket(int32 type)
{
	this->setFileDescriptor((int32)::socket(AF_INET, type, 0));
	if((m_socket == INVALID_SOCKET) && (WSAGetLastError() == WSANOTINITIALISED))
	{
		EndPoint::initNetwork();
		this->setFileDescriptor((int32)::socket(AF_INET, type, 0));
	}
}

inline int32 EndPoint::setnodelay(bool nodelay /* = true */)
{
	int32 arg = int32(nodelay);
	return setsockopt(m_socket, IPPROTO_TCP, TCP_NODELAY, (char*)&arg, sizeof(int));
}

inline int32 EndPoint::setnonblocking(bool nonblocking)
{
#ifdef SL_OS_WINDOWS
	u_long val = nonblocking ? 1: 0;
	return ::ioctlsocket(m_socket, FIONBIO, &val);
#else
	int32 val = nonblocking ? O_NONBLOCK : 0;
	return ::fcntl(m_socket, F_SETFL, val);
#endif
}

inline int32 EndPoint::setbroadcast(bool broadcast)
{
#ifdef SL_OS_WINDOWS
	 bool val;
#else
	int32 val;
	if(broadcast)
	{
		val  = 2;
		::setsockopt(m_socket, SOL_IP, IP_MULTICAST_TTL, &val, sizeof(int));
	}
#endif
	val  = broadcast ? 1 : 0;
	return ::setsockopt(m_socket, SOL_SOCKET, SO_BROADCAST, (char*)&val, sizeof(val));
}

inline int32 EndPoint::setreuseaddr(bool reuseaddr)
{
#ifdef SL_OS_WINDOWS
	bool val;
#else
	int val;
#endif
	val = reuseaddr ? 1: 0;
	return setsockopt(m_socket, SOL_SOCKET, SO_REUSEADDR, (char*)&val, sizeof(val));
}

inline int32 EndPoint::setlinger(uint16 onoff, uint16 linger)
{
	struct linger l = {0};
	l.l_onoff = onoff;
	l.l_linger = linger;
	return setsockopt(m_socket, SOL_SOCKET, SO_LINGER, (const char*)&l, sizeof(l));
}

inline int32 EndPoint::setkeepalive(bool keepalive)
{
#ifdef SL_OS_WINDOWS
	bool val;
#else
	int val;
#endif
	val = keepalive ? 1 : 0;
	return setsockopt(m_socket, SOL_SOCKET, SO_KEEPALIVE, (char*)&val, sizeof(val));
}

inline int32 EndPoint::bind(uint16 networkPort, uint32 networkAddr)
{
	sockaddr_in sin;
	memset(&sin, 0, sizeof(sockaddr_in));
	sin.sin_family = AF_INET;
	sin.sin_port = networkPort;
	sin.sin_addr.s_addr = networkAddr;
	return ::bind(m_socket, (struct sockaddr*)&sin, sizeof(sin));
}

inline int32 EndPoint::joinMulticastGroup(uint32 networkAddr)
{
#ifdef SL_OS_WINDOWS
	return -1;
#else
	struct ip_mreqn req;
#endif
}

inline int32 EndPoint::quitMulticastGroup(uint32 networkAddr)
{
#ifdef SL_OS_WINDOWS
	return -1;
#else
	struct ip_mreqn req;
#endif
}

inline int32 EndPoint::close()
{
	if(m_socket == -1)
	{
		return 0;
	}
#ifdef SL_OS_LINUX
	int32 ret = close(m_socket);
#else
	int32 ret = closesocket(m_socket);
#endif

	if(ret == 0)
	{
		this->setFileDescriptor(-1);
	}

#ifdef SL_OS_WINDOWS
	m_socket = INVALID_SOCKET;
#else
	m_socket = -1;
#endif
	m_address = Address::NONE;

	return ret;
}

inline int32 EndPoint::getlocaladdress(uint16* networkPort, uint32* networkAddr) const
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

	int32 ret = ::getpeername(m_socket, (struct sockaddr*)&sin, &sinLen);
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

inline int32 EndPoint::getremotehostname(std::string* host) const
{
	sockaddr_in sin;
	socklen_t sinlen = sizeof(sin);

	int32 ret = getpeername(m_socket, (struct sockaddr*)&sin, &sinlen);
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

inline int32 EndPoint::sendto(void* gramData, int32 gramSize, uint16 networkPort, uint32 networkAddr)
{
	sockaddr_in sin;
	sin.sin_family = AF_INET;
	sin.sin_port = networkPort;
	sin.sin_addr.s_addr = networkAddr;

	return this->sendto(gramData, gramSize, sin);
}

inline int32 EndPoint::sendto(void* gramData, int32 gramSize, struct sockaddr_in& sin)
{
	return ::sendto(m_socket, (char*)gramData, gramSize, 0, (sockaddr*)&sin, sizeof(sin)); 
}

inline int32 EndPoint::recvfrom(void* gramData, int32 gramSize, uint16* networkPort, uint32* networkAddr)
{
	sockaddr_in sin;
	int32 result = this->recvfrom(gramData, gramSize, sin);

	if(result >= 0)
	{
		if(networkAddr != NULL) *networkAddr = sin.sin_addr.s_addr;
		if(networkPort != NULL) *networkPort = sin.sin_port;
	}
	return result;
}

inline int32 EndPoint::recvfrom(void* gramData, int32 gramSize, struct sockaddr_in& sin)
{
	socklen_t sinlen = sizeof(sin);
	int ret = ::recvfrom(m_socket, (char*)gramData, gramSize, 0, (sockaddr*)&sin, &sinlen);
	return ret;
}

inline int32 EndPoint::listen(int32 backlog)
{
	return ::listen(m_socket, backlog);
}

inline int32 EndPoint::connect(bool autosetflags /* = true */)
{
	return connect(m_address.m_port, m_address.m_ip, autosetflags);
}

inline int32 EndPoint::connect(uint16 networkport, uint32 networkAddr /* = INADDR_BROADCAST */, bool autosetflags /* = true */)
{
	sockaddr_in sin;
	sin.sin_family = AF_INET;
	sin.sin_port  = networkport;
	sin.sin_addr.s_addr = networkAddr;

	int32 ret = ::connect(m_socket, (sockaddr*)&sin, sizeof(sin));
	if(autosetflags)
	{
		setnonblocking(true);
		setnodelay(true);
	}
	return ret;
}

inline EndPoint* EndPoint::accept(uint16* networkPort /* = NULL */, uint32* networkAddr /* = NULL */, bool autosetflags /* = true */)
{
	sockaddr_in sin;
	socklen_t sinLen = sizeof(sin);
	int32 ret = (int32)::accept(m_socket, (sockaddr*)&sin, &sinLen);
#ifdef SL_OS_LINUX
	if(ret < 0) return NULL;
#else
	if(ret == INVALID_SOCKET) return NULL;
#endif

	EndPoint* pNew = CREATE_POOL_OBJECT(EndPoint);
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

inline int32 EndPoint::send(const void* gramData, int32 gramSize)
{
	return ::send(m_socket, (char*)gramData, gramSize, 0);
}

inline int32 EndPoint::recv(void* gramData, int32 gramSize)
{
	return ::recv(m_socket, (char*)gramData, gramSize, 0);
}

#ifdef SL_OS_LINUX
inline int32 EndPoint::getInterfaceFlags(char* name, int32& flag)
{

}
#else
inline int32 EndPoint::getInterfaceFlags(char* name, int32& flag)
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

inline int32 EndPoint::getInterfaceAddress(const char* name, uint32& address)
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

}
}