#ifndef _SL_ADDRESS_H_
#define _SL_ADDRESS_H_

#include "slobjectpool.h"
#include "slbase.h"
namespace sl
{
namespace network
{
class Address: public PoolObject
{
public:
	static const Address NONE;

	typedef SLShared_ptr<SmartPoolObject<Address>> SmartPoolObjectPtr;
	static SmartPoolObjectPtr createSmartPollObj();
	static CObjectPool<Address>& ObjPool();
	static Address* createPoolObject();
	static void reclaimPoolObject(Address* obj);
	static void destroyObjPool();
	void onReclaimObject();


	virtual size_t getPoolObjectBytes()
	{
		size_t bytes = sizeof(m_port) + sizeof(m_ip);
		return bytes;
	}

	Address();
	Address(uint32 ip, uint16 port);
	Address(std::string ip, uint16 port);

	virtual ~Address(){};

	int writeToString(char* str, int length) const;

	operator char*() const {return this->c_str();}

	char* c_str() const;
	const char* ipAsString() const;
	bool isNone() const {return this->m_ip == 0;}

	static int string2ip(const char* str, uint32& address);
	static int ip2string(uint32 address, char* str);


public:
	uint32			m_ip;
	uint16			m_port;
private:
	static char s_stringBuf[2][32];
	static int s_curStringBuf;
	static char* nextStringBuf();

};

inline Address::Address()
	:m_ip(0),
	 m_port(0)
{}

inline Address::Address(uint32 ip, uint16 port)
	:m_ip(ip),m_port(port)
{}

inline bool operator==(const Address& a, const Address& b)
{
	return (a.m_ip == b.m_ip) && (a.m_port == b.m_port);
}

inline bool operator!=(const Address& a, const Address& b)
{
	return (a.m_ip != b.m_ip) && (a.m_port != b.m_port);
}

inline bool operator < (const Address& a, const Address& b)
{
	return (a.m_ip < b.m_ip) || (a.m_ip == b.m_ip && (a.m_port < b.m_port));
}


}
}
#endif