#ifndef _SL_LIB_NET_ADDRESS_H_
#define _SL_LIB_NET_ADDRESS_H_
#include "slnetbase.h"
#include "slpool.h"

namespace sl{
namespace network{
class Address{
public:
	inline static Address* create(uint32 ip, uint16 port){
		return CREATE_FROM_POOL(s_pool, ip, port);
	}

	inline static Address* create(const char* ip, uint16 port){
		return CREATE_FROM_POOL(s_pool, ip, port);
	}

	inline void release(){
		s_pool.recover(this);
	}

	Address();
	Address(uint32 ip, uint16 port);
	Address(const char* ip, uint16 port);
	virtual ~Address();

	int32 writeToString(char* str, int32 length) const;
	operator char*() const {return this->c_str();}
	char* c_str() const;
	const char* ipAsString() const;
	bool isNone() const {return _ip == 0;}

	inline uint32 ip() const { return _ip; }
	inline uint16 port() const { return _port; }

	static int32 string2ip(const char* str, uint32& address);
	static int32 ip2string(uint32 address, char* str);

public:
	uint32		_ip;
	uint16		_port;

protected:
	static char* nextStringBuf();

private:
	static sl::SLPool<Address> s_pool;
	static char s_stringBuf[2][32];
	static int s_curStringBuf;
};

inline bool operator==(const Address& a, const Address& b){
	return (a.ip() == b.ip()) && (a.port() == b.port());
}

inline bool operator!=(const Address& a, const Address& b){
	return (a.ip() != b.ip()) && (a.port() != b.port());
}

inline bool operator < (const Address& a, const Address& b){
	return (a.ip() < b.ip()) || (a.ip() == b.ip() && (a.port() < b.port()));
}


}
}
#endif