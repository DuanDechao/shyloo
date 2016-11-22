#include "slbundle_broadcast.h"
#include "slnetwork_interface.h"
#include "slevent_dispatcher.h"
namespace sl
{
namespace network
{
BundleBroadcast::BundleBroadcast(NetworkInterface& networkInterface, uint16 bindPort /* = SL_PORT_BROADCAST_DISCOVERY */, 
								 uint32 recvWindowSize /* = PACKET_MAX_SIZE_TCP */):
	Bundle(NULL, network::PROTOCOL_UDP),
	m_epListen(),
	m_networkInterface(networkInterface),
	m_recvWindowSize(recvWindowSize),
	m_good(false),
	m_itry(5),
    m_machine_addresses()
{
	m_epListen.socket(SOCK_DGRAM);
	m_epBroadcast.socket(SOCK_DGRAM);

	if(!m_epListen.good() || !m_epBroadcast.good())
	{
		m_networkInterface.getDispatcher().breakProcessing();
	}
	else
	{
		int count = 0;
		while(true)
		{
			if(m_epListen.bind(htons(bindPort), htonl(INADDR_ANY)) != 0)
			{
				m_good = false;
				Sleep(10);
				count++;

				if(count > 30)
				{
					break;
				}
			}
			else
			{
				m_epListen.addr(htons(bindPort), htonl(INADDR_ANY));
				m_good = true;
				break;
			}
		}
	}

	getCurrPacket()->data_resize(recvWindowSize);
}

BundleBroadcast::~BundleBroadcast()
{
	close();
}

void BundleBroadcast::close()
{
	m_epListen.close();
	m_epBroadcast.close();
}

EventDispatcher& BundleBroadcast::dispatcher()
{
	return m_networkInterface.getDispatcher();
}

void BundleBroadcast::addBroadCastAddress(string addr)
{
	m_machine_addresses.push_back(addr);
}

bool BundleBroadcast::broadcast(uint16 port /* = 0 */)
{
	if(!m_epBroadcast.good())
		return false;

	if(port == 0)
		port = SL_MACHINE_BROADCAST_SEND_PORT;

	m_epBroadcast.addr(port, network::BROADCAST);

	if(m_epBroadcast.setbroadcast(true) != 0)
	{
		m_networkInterface.getDispatcher().breakProcessing();
		return false;
	}

	this->finiMessage();
	SL_ASSERT(packets().size() == 1);

	m_epBroadcast.sendto(packets()[0]->data(), packets()[0]->length(), htons(port), network::BROADCAST);

	//如果制定了地址池，则向所有地址发送消息
	std::vector<std::string>::iterator addr_iter = m_machine_addresses.begin();
	for (; addr_iter != m_machine_addresses.end(); ++addr_iter)
	{
		network::EndPoint ep;
		ep.socket(SOCK_DGRAM);
		if(!ep.good())
		{
			break;
		}

		uint32 uaddress;
		network::Address::string2ip((*addr_iter).c_str(), uaddress);
		ep.sendto(packets()[0]->data(), packets()[0]->length(), htons(SL_MACHINE_BROADCAST_SEND_PORT), uaddress);
	}

	return true;
}

bool BundleBroadcast::receive(/*MessageArgs* recvArgs, */sockaddr_in* psin /* = NULL */, int32 timeout /* = 100000 */, bool showerr /* = true */)
{
	if(!m_epListen.good())
		return false;

	struct timeval tv;
	fd_set fds;

	int icount = 1;
	tv.tv_sec = 0;
	tv.tv_usec = timeout;

	if(!getCurrPacket())
		newPacket();

	while(1)
	{
		FD_ZERO(&fds);
		FD_SET((int)m_epListen, &fds);
		int selgot = select(m_epListen + 1, &fds, NULL, NULL, &tv);

		if(selgot == 0)
		{
			if(icount > m_itry)
			{
				if(showerr)
				{

				}

				return false;
			}
			else
			{

			}
			icount++;
			continue;
		}
		else if(selgot == -1)
		{
			if(showerr)
			{

			}
			return false;
		}
		else
		{
			sockaddr_in sin;
			getCurrPacket()->resetPacket();

			if(psin == NULL)
				psin = &sin;

			getCurrPacket()->data_resize(m_recvWindowSize);

			int len = m_epListen.recvfrom(getCurrPacket()->data(), m_recvWindowSize, *psin);
			if(len == -1)
			{
				if(showerr)
				{

				}
				continue;
			}

			getCurrPacket()->wpos(len);
			/*if(recvArgs != NULL)
			{
				try{
					recvArgs->createFromStream(*getCurrPacket());
				}
				catch(MemoryStreamException &)
				{
					continue;
				}
			}*/
			break;
		}
	}
	return true;
}
}
}