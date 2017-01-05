#ifndef _SL_ENDPOINT_H_
#define _SL_ENDPOINT_H_

#include "slobjectpool.h"
#include "sladdress.h"
#include "slnetbase.h"
namespace sl
{
namespace network
{
#pragma comment(lib,"Iphlpapi.lib")
class Bundle;
class EndPoint
{
public:
	EndPoint(Address address);
	EndPoint(uint32 networkAddr = 0, uint16 networkport = 0);
	virtual ~EndPoint();

	inline operator SLSOCKET() const;

	static void initNetwork();
	inline bool good() const;

	void socket(int type);
	inline SLSOCKET socket() const;

	inline void setFileDescriptor(int fd);

	inline int32 joinMulticastGroup(uint32 networkAddr);
	inline int32 quitMulticastGroup(uint32 networkAddr);

	inline int32 close();

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
	void send(Bundle* pBundle);
	void sendto(Bundle* pBundle, uint16 networkPort, uint32 networkAddr = BROADCAST);

	inline int32 recv(void* gramData, int32 gramSize);
	bool recvAll(void* gramData, int32 gramSize);

	inline uint32 getRTT();

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

protected:
	SLSOCKET		m_socket;
	Address			m_address;

};
CREATE_OBJECT_POOL(EndPoint);

}
}
#include "slendpoint.inl"
#endif