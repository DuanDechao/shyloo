#include "sladdress.h"
namespace sl
{
namespace network
{
char Address::s_stringBuf[2][32] = {{0},{0}};

int Address::s_curStringBuf = 0;
const Address Address::NONE(0,0);

static CObjectPool<Address> g_objPool("Address");
CObjectPool<Address>& Address::ObjPool()
{
	return g_objPool;
}

Address* Address::createPoolObject()
{
	return g_objPool.FetchObj();
}

void Address::reclaimPoolObject(Address* obj)
{
	g_objPool.ReleaseObj(obj);
}

void Address::destroyObjPool()
{
	g_objPool.Destroy();
}

Address::SmartPoolObjectPtr Address::createSmartPollObj()
{
	return SmartPoolObjectPtr(new SmartPoolObject<Address>(ObjPool().FetchObj(), g_objPool));
}
void Address::onReclaimObject()
{
	m_ip = 0;
	m_port = 0;
}

Address::Address(std::string ip, uint16 port)
	:m_ip(0),
	 m_port(htons(port))
{
	uint32 addr;
	network::Address::string2ip(ip.c_str(), addr);
	m_ip = addr;
}

int Address::writeToString(char* str, int length) const
{
	uint32 uip = ntohl(m_ip);
	uint16 uport = ntohs(m_port);
	return CPlatForm::_sl_snprintf(str, length, "%d.%d.%d.%d:%d",
		(int)(uint8)(uip >> 24),
		(int)(uint8)(uip >> 16),
		(int)(uint8)(uip >> 8),
		(int)(uint8)(uip),
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
	CPlatForm::_sl_snprintf(buf, 32, "%d.%d.%d.%d",
		(int)(uint8)(uip >> 24),
		(int)(uint8)(uip >> 16),
		(int)(uint8)(uip >> 8),
		(int)(uint8)(uip));
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

	return sprintf(str, "%d.%d.%d.%d",
		(int)(uint8)(address>>24),
		(int)(uint8)(address>>16),
		(int)(uint8)(address>>8),
		(int)(uint8)address);
}

}
}