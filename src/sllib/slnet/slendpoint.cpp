#include "slendpoint.h"
#include "slpacket_sender.h"
#ifdef SL_OS_WINDOWS
#include <IPHlpApi.h>
#endif
namespace sl{
namespace network{

static bool g_networkInitted = false;
sl::SLPool<EndPoint> EndPoint::s_pool;
EndPoint::EndPoint(uint32 networkAddr /* = 0 */, uint16 networkport /* = 0 */)
	:_socket(SL_INVALID_SOCKET)
{
	if (networkAddr){
		_address._ip = networkAddr;
		_address._port = networkport;
	}
}

EndPoint::EndPoint(Address address)
	:_socket(SL_INVALID_SOCKET)
{
	if (address._ip > 0){
		_address = address;
	}
}

inline EndPoint::~EndPoint(){
	this->closeEndPoint();
}

bool EndPoint::getClosedPort(network::Address& closedPort){
	bool isResultSet = false;

#ifdef SL_OS_LINUX
#else

#endif
	return isResultSet;
}

int EndPoint::getBufferSize(int optname) const{
	SLASSERT(optname == SO_SNDBUF || optname == SO_RCVBUF, "wtf");

	int recvbuf = -1;
	socklen_t rbargssize = sizeof(int);
	int rberr = getsockopt(_socket, SOL_SOCKET, optname, (char*)&recvbuf, &rbargssize);

	if(rberr == 0 && rbargssize == sizeof(int))
		return recvbuf;

	return -1;
}

bool EndPoint::getInterfaces(std::map<uint32, std::string>& interfaces){
#ifdef SL_OS_WINDOWS
	int count = 0;
	char hostname[1024];
	struct hostent* inaddrs;

	if(gethostname(hostname, 1024) == 0){
		inaddrs = gethostbyname(hostname);
		if(inaddrs){
			while(inaddrs->h_addr_list[count]){
				unsigned long addrs = *(unsigned long*)inaddrs->h_addr_list[count];
				interfaces[addrs] = "eth0";
				char* ip = inet_ntoa(*(struct in_addr*)inaddrs->h_addr_list[count]);
				++count;
			}
		}
	}
	return count > 0;
#else
	return true;
#endif
}

int EndPoint::findIndicatedInterface(const char* spec, uint32& address){
	address = 0;
	if(spec == NULL || spec[0] == 0){
		return -1;
	}

	if(0 != Address::string2ip(spec, address)){
		return -1;
	}
	else if(0 != this->getInterfaceAddressByMAC(spec, address)){
		return -1;
	}
	else if(0 != this->getInterfaceAddressByName(spec, address)){
		return -1;
	}

	return 0;
}

int EndPoint::getInterfaceAddressByName(const char* name, uint32& address){
	int ret = -1;
#ifdef SL_OS_WINDOWS

	PIP_ADAPTER_INFO pIpAdapterInfo = new IP_ADAPTER_INFO();
	unsigned long size = sizeof(IP_ADAPTER_INFO);

	int ret_info = ::GetAdaptersInfo(pIpAdapterInfo, &size);

	if(ERROR_BUFFER_OVERFLOW == ret_info){
		delete pIpAdapterInfo;
		pIpAdapterInfo = (PIP_ADAPTER_INFO)new unsigned char[size];
		ret_info = ::GetAdaptersInfo(pIpAdapterInfo, &size);
	}

	if(ERROR_SUCCESS == ret_info){
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

	if(pIpAdapterInfo){
		delete pIpAdapterInfo;
	}
#else
#endif
	return ret;

}	

bool EndPoint::setBufferSize(int optname, int size){
	setsockopt(_socket, SOL_SOCKET, optname, (const char*)&size, sizeof(size));
	return this->getBufferSize(optname) >= size;
}

bool EndPoint::recvAll(void* gramData, int gramSize){
	while(gramSize > 0){
		int len = this->recv(gramData, gramSize);
		if(len <= 0){
			if(len == 0){
				//warning
			}else{
				//warning
			}
			return false;
		}
		gramSize -= len;
		gramData = ((char*)gramData) + len;
	}
	return true;
}

network::Address EndPoint::getlocalAddress() const{
	network::Address addr((uint32)0,0);
	if(this->getlocaladdress((uint16*)&addr._port, (uint32*)&addr._ip) == -1){

	}
	return addr;
}

network::Address EndPoint::getremoteAddress() const{
	network::Address addr((uint32)0, 0);
	if(this->getremoteaddress((uint16*)&addr._port, (uint32*)&addr._ip) == -1){

	}
	return addr;
}

void EndPoint::initNetwork(){
	if (g_networkInitted)
		return;

	g_networkInitted = true;

#ifdef SL_OS_WINDOWS
	WSAData wsdata;
	WSAStartup(0x202, &wsdata);
#endif 
}

bool EndPoint::waitSend(){
	fd_set fds;
	struct timeval tv = {0, 10000};
	FD_ZERO(&fds);
	FD_SET(_socket, &fds);

	return select((int32)(_socket + 1), NULL, &fds, NULL, &tv) > 0;
}

}
}
