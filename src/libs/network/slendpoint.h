#ifndef _SL_ENDPOINT_H_
#define _SL_ENDPOINT_H_

#include "../common/slobjectpool.h"
#include "sladdress.h"
#include "slcommon.h"
namespace sl
{
namespace network
{
class Bundle;
class EndPoint: public PoolObject
{
public:
	typedef SLShared_ptr<SmartPoolObject<EndPoint>> SmartPoolObjectPtr;
	static SmartPoolObjectPtr createSmartPoolObj();
	static CObjectPool<EndPoint>& ObjPool();
	static EndPoint* createPoolObject();
	static void reclaimPoolObject(EndPoint* obj);
	static void destroyObjPool();
	void onReclaimObject();

	virtual size_t getPoolObjectBytes()
	{
		size_t bytes = sizeof(SLSOCKET) + m_address.getPoolObjectBytes();
		return bytes;
	}

	EndPoint(Address address);
	EndPoint(uint32 networkAddr = 0, uint16 networkport = 0);
	virtual ~EndPoint();

	inline operator SLSOCKET() const;

	static void initNetwork();
	inline bool good() const;

	void socket(int type);
	inline SLSOCKET socket() const;

	inline void setFileDescriptor(int fd);

	inline int joinMulticastGroup(uint32 networkAddr);
	inline int quitMulticastGroup(uint32 networkAddr);

	inline int close();

	inline int setnonblocking(bool nonblocking);
	inline int setbroadcast(bool broadcast);
	inline int setreuseaddr(bool reuseaddr);
	inline int setkeepalive(bool keepalive);
	inline int setnodelay(bool nodelay = true);
	inline int setlinger(uint16 onoff, uint16 linger);

	inline int bind(uint16 networkPort = 0, uint32 networkAddr = INADDR_ANY);

	inline int listen(int backlog = 5);

	inline int connect(uint16 networkport, uint32 networkAddr = INADDR_BROADCAST, bool autosetflags = true);
	inline int connect(bool autosetflags = true);

	inline EndPoint* accept(uint16* networkPort = NULL, uint32* networkAddr = NULL, bool autosetflags = true);

	inline int send(const void* gramData, int gramSize);
	void send(Bundle* pBundle);
	void sendto(Bundle* pBundle, uint16 networkPort, uint32 networkAddr = BROADCAST);

	inline int recv(void* gramData, int gramSize);
	bool recvAll(void* gramData, int gramSize);

	inline uint32 getRTT();

	inline int getInterfaceFlags(char* name, int& flag);
	inline int getInterfaceAddress(const char* name, uint32& address);
	inline int getInterfaceNetmask(const char* name, uint32& netmask);
	bool getInterfaces(std::map<uint32, std::string>& interfaces);

	int findIndicatedInterface(const char* spec, uint32& address);
	int findDefaultInterface(char* name, int bufSize);

	int getInterfaceAddressByName(const char* name, uint32& address);
	int getInterfaceAddressByMAC(const char* mac, uint32& address);
	int getDefaultInterfaceAddress(uint32& address);

	int getBufferSize(int optname) const;
	bool setBufferSize(int optname, int size);

	inline int getlocaladdress(uint16* networkPort, uint32* networkAddr) const;
	inline int getremoteaddress(uint16* networkPort, uint32* networkAddr) const;

	network::Address getlocalAddress() const;
	network::Address getremoteAddress() const;

	bool getClosedPort(network::Address& closedPort);

	inline const char* c_str() const;
	inline int getremotehostname(std::string* name) const;

	inline int sendto(void* gramData, int gramSize, uint16 networkPort, uint32 networkAddr = BROADCAST);
	inline int sendto(void* gramData, int gramSize, struct sockaddr_in& sin);
	inline int recvfrom(void* gramData, int gramSize, uint16* networkPort, uint32* networkAddr);
	inline int recvfrom(void* gramData, int gramSize, struct sockaddr_in& sin);

	inline const Address& addr() const;
	inline void addr(const Address& newAddress);
	inline void addr(uint16 newNetworkPort, uint32 newNetworkAddress);

	bool waitSend();

protected:
	SLSOCKET		m_socket;
	Address			m_address;

};
}
}
#endif