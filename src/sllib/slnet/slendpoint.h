#ifndef _SL_LIB_NET_ENDPOINT_H_
#define _SL_LIB_NET_ENDPOINT_H_

#include "sladdress.h"
#include "slnetbase.h"
#include "slpool.h"
#include <map>

namespace sl{
namespace network{

#ifdef SL_OS_WINDOWS
#pragma comment(lib,"Iphlpapi.lib")
#endif

class EndPoint{
public:
	inline static EndPoint* create(Address address){
		return CREATE_FROM_POOL(s_pool, address);
	}

	inline static EndPoint* create(uint32 ip = 0, uint16 port = 0){
		return CREATE_FROM_POOL(s_pool, ip, port);
	}

	inline void release(){
		s_pool.recover(this);
	}

	static void initNetwork();
	inline bool good() const { return _socket != SL_INVALID_SOCKET; }

	inline operator SLSOCKET() const{ return _socket; }
	inline SLSOCKET socket() const { return _socket; }
	void socket(int32 type);

	inline void setFileDescriptor(int fd){ _socket = fd; }

	inline int32 joinMulticastGroup(uint32 networkAddr);
	inline int32 quitMulticastGroup(uint32 networkAddr);

	inline int32 setnonblocking(bool nonblocking);
	inline int32 setbroadcast(bool broadcast);
	inline int32 setreuseaddr(bool reuseaddr);
	inline int32 setkeepalive(bool keepalive);
	inline int32 setnodelay(bool nodelay = true);
	inline int32 setlinger(uint16 onoff, uint16 linger);

	inline int32 bind(uint16 networkPort = 0, uint32 networkAddr = INADDR_ANY);
	inline int32 listen(int32 backlog = 5);

	inline int32 connect(uint16 networkport, uint32 networkAddr = INADDR_BROADCAST, bool autosetflags = true);
	inline int32 connect(bool autosetflags = true);

	inline EndPoint* accept(uint16* networkPort = NULL, uint32* networkAddr = NULL, bool autosetflags = true);

	inline int32 send(const void* gramData, int32 gramSize);

	inline int32 recv(void* gramData, int32 gramSize);
	bool recvAll(void* gramData, int32 gramSize);

	inline int32 closeEndPoint();

	inline int32 getInterfaceFlags(char* name, int32& flag);
	inline int32 getInterfaceAddress(const char* name, uint32& address);
	inline int32 getInterfaceNetmask(const char* name, uint32& netmask);
	bool getInterfaces(std::map<uint32, std::string>& interfaces);

	int32 findIndicatedInterface(const char* spec, uint32& address);
	int32 findDefaultInterface(char* name, int32 bufSize);

	int32 getInterfaceAddressByName(const char* name, uint32& address);
	int32 getInterfaceAddressByMAC(const char* mac, uint32& address){return 0;}
	int32 getDefaultInterfaceAddress(uint32& address){return 0;}

	int32 getBufferSize(int32 optname) const;
	bool setBufferSize(int32 optname, int32 size);

	inline int32 getlocaladdress(uint16* networkPort, uint32* networkAddr) const;
	inline int32 getremoteaddress(uint16* networkPort, uint32* networkAddr) const;

	network::Address getlocalAddress() const;
	network::Address getremoteAddress() const;

	bool getClosedPort(network::Address& closedPort);

	inline const char* c_str() const;
	inline int32 getremotehostname(std::string* name) const;

	inline int32 sendto(void* gramData, int32 gramSize, uint16 networkPort, uint32 networkAddr = BROADCAST);
	inline int32 sendto(void* gramData, int32 gramSize, struct sockaddr_in& sin);
	inline int32 recvfrom(void* gramData, int32 gramSize, uint16* networkPort, uint32* networkAddr);
	inline int32 recvfrom(void* gramData, int32 gramSize, struct sockaddr_in& sin);

	inline const Address& addr() const;
	inline void addr(const Address& newAddress);
	inline void addr(uint16 newNetworkPort, uint32 newNetworkAddress);

	bool waitSend();

private:
	friend sl::SLPool<EndPoint>;
	EndPoint(Address address);
	EndPoint(uint32 networkAddr, uint16 networkport);
	virtual ~EndPoint();

private:
	SLSOCKET		_socket;
	Address			_address;

	static sl::SLPool<EndPoint> s_pool;
};

}
}
#include "slendpoint.inl"
#endif
