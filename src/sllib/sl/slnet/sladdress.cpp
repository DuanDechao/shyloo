#include "sladdress.h"
namespace sl
{
namespace network
{
char Address::s_stringBuf[2][32] = {{0},{0}};

int Address::s_curStringBuf = 0;
const Address Address::NONE(0,0);

Address::Address(std::string ip, uint16 port)
	:m_ip(0),
	 m_port(htons(port))
{
	network::Address::string2ip(ip.c_str(), m_ip);
}

int32 Address::writeToString(char* str, int32 length) const
{
	uint32 uip = ntohl(m_ip);
	uint16 uport = ntohs(m_port);
	return SafeSprintf(str, length, "%d.%d.%d.%d:%d",
		(int32)(uint8)(uip >> 24),
		(int32)(uint8)(uip >> 16),
		(int32)(uint8)(uip >> 8),
		(int32)(uint8)(uip),
		uport);
}

char* Address::c_str() const
{
	char* buf = Address::nextStringBuf();
	this->writeToString(buf, 32);
	return buf;
}

const char* Address::ipAsString() const
{
	uint32 uip = ntohl(m_ip);
	char* buf = Address::nextStringBuf();
	SafeSprintf(buf, 32, "%d.%d.%d.%d",
		(int32)(uint8)(uip >> 24),
		(int32)(uint8)(uip >> 16),
		(int32)(uint8)(uip >> 8),
		(int32)(uint8)(uip));
	return buf;
}

char* Address::nextStringBuf()
{
	s_curStringBuf = (s_curStringBuf + 1) % 2;
	return s_stringBuf[s_curStringBuf];
}

int Address::string2ip(const char* str, uint32& address)
{
	uint32 trial;
#ifdef SL_OS_WINDOWS
	if((trial = inet_addr(str)) != INADDR_NONE)
#else
	if(inet_aton(str, (struct in_addr*)&trial) != 0)
#endif
	{
		address = trial;
		return 0;
	}

	struct hostent* hosts = gethostbyname(str);
	if(hosts != NULL)
	{
		address = *(uint32*)(hosts->h_addr_list[0]);
		return 0;
	}

	return -1;
}

int Address::ip2string(uint32 address, char* str)
{
	address = ntohl(address);

	return SafeSprintf(str, (int32)strlen(str), "%d.%d.%d.%d",
		(int32)(uint8)(address>>24),
		(int32)(uint8)(address>>16),
		(int32)(uint8)(address>>8),
		(int32)(uint8)address);
}

}
}