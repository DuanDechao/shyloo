#include "slbundle.h"
#include "sltcp_packet.h"
#include "sludp_packet.h"
namespace sl
{
namespace network
{

Bundle::Bundle(Channel* pChannel /* = NULL */, ProtocolType pt /* = PROTOCOL_TCP */)
	:m_pChannel(pChannel),
	 m_numMessages(0),
	 m_pCurrPacket(NULL),
	 m_packets(),
	 m_isTCPPacket(pt == PROTOCOL_TCP),
	 m_packetMaxSize(0)
{
	calcPacketMaxSize();
	newPacket();
}

Bundle::Bundle(const Bundle& bundle)
{
	m_isTCPPacket = bundle.m_isTCPPacket;
	m_pChannel = bundle.m_pChannel;
	
	Packets::const_iterator iter= bundle.m_packets.begin();
	for (; iter != bundle.m_packets.end(); ++iter)
	{
		newPacket();
		m_pCurrPacket->append(*static_cast<MemoryStream*>((*iter)));
		m_packets.push_back(m_pCurrPacket);
	}

	m_pCurrPacket= NULL;
	if(bundle.m_pCurrPacket)
	{
		newPacket();
		m_pCurrPacket->append(*static_cast<MemoryStream*>(bundle.m_pCurrPacket));
	}

	m_numMessages = bundle.m_numMessages;

	calcPacketMaxSize();
}

Bundle::~Bundle()
{
	clear(true);
}

void Bundle::calcPacketMaxSize()
{
	m_packetMaxSize = m_isTCPPacket ? (int32)TCPPacket::maxBufferSize(): PACKET_MAX_SIZE_UDP;
}

int32 Bundle::packetsLength(bool calccurr)
{
	int32 len = 0;
	Packets::iterator iter = m_packets.begin();
	for (; iter != m_packets.end(); ++iter)
	{
		len += (int32)(*iter)->length();

	}
	if(calccurr && m_pCurrPacket)
		len += (int32)m_pCurrPacket->length();
	return len;
}

int32 Bundle::onPacketAppend(int32 addsize, bool inseparable)
{
	if(m_pCurrPacket == NULL)
	{
		newPacket();
	}

	int32 totalsize = (int32)m_pCurrPacket->length();
	int fwpos = (int32)m_pCurrPacket->wpos();

	if(inseparable)
		fwpos += addsize;

	//如果當前包裝不下本次append的數據，將其填充到新包中
	if(fwpos > m_packetMaxSize)
	{
		m_packets.push_back(m_pCurrPacket);
		newPacket();
		totalsize = 0;
	}

	int32 remainSize = m_packetMaxSize - totalsize;
	int32 taddsize = addsize;

	//如果當前包剩餘空間小於要添加的字節則本次填滿此包
	if(remainSize < addsize)
		taddsize = remainSize;

	return taddsize;
}

Packet* Bundle::newPacket()
{
	MALLOC_PACKET(m_pCurrPacket, m_isTCPPacket);
	m_pCurrPacket->SetBundle(this);
	return m_pCurrPacket;
}

void Bundle::clear(bool isRecl)
{
	if(m_pCurrPacket != NULL)
	{
		m_packets.push_back(m_pCurrPacket);
		m_pCurrPacket = NULL;
	}
	if (m_packets.empty())
		return;

	Packets::iterator iter = m_packets.begin();
	for (; iter != m_packets.end(); ++iter)
	{
		if(!isRecl)
		{
			delete (*iter);
		}
		else
		{
			RECLAIM_PACKET(m_isTCPPacket, (*iter));
		}
	}
	m_packets.clear();

	m_pChannel = NULL;
	m_numMessages = 0;

	calcPacketMaxSize();
}

void Bundle::clearPackets()
{
	if(m_pCurrPacket != NULL)
	{
		m_packets.push_back(m_pCurrPacket);
		m_pCurrPacket = NULL;
	}

	Packets::iterator iter = m_packets.begin();
	for (; iter != m_packets.end(); ++iter)
	{
		RECLAIM_PACKET(m_isTCPPacket, (*iter));
	}
	m_packets.clear();
}

void Bundle::newMessage()
{
	if(m_pCurrPacket == NULL)
	{
		if(m_packets.size() > 0 && packetHaveSpace())
		{
			m_pCurrPacket = m_packets.back();
			m_packets.pop_back();
		}
		else
		{
			this->newPacket();
		}
	}

	finiMessage(false);
	SLASSERT(m_pCurrPacket != NULL, "wtf");

	++m_numMessages;
}

void Bundle::finiMessage(bool isSend)
{
	SLASSERT(m_pCurrPacket != NULL, "wtf");
	m_pCurrPacket->SetBundle(this);

	if(isSend)
	{
		m_packets.push_back(m_pCurrPacket);
		m_pCurrPacket = NULL;
	}
}

bool Bundle::revokeMessage(int32 size)
{
	if(m_pCurrPacket)
	{
		if(size >= (int32)m_pCurrPacket->wpos())
		{
			size -= (int32)m_pCurrPacket->wpos();
			RECLAIM_PACKET(m_isTCPPacket, m_pCurrPacket);
			m_pCurrPacket = NULL;
		}
		else
		{
			m_pCurrPacket->wpos((int32)m_pCurrPacket->wpos()- size);
			size = 0;
		}
	}

	while(size > 0 && m_packets.size() > 0)
	{
		network::Packet* pPacket = m_packets.back();
		if(pPacket->wpos() > (int32)size)
		{
			pPacket->wpos((int32)pPacket->wpos() - size);
			size = 0;
			break;
		}
		else
		{
			size -= (int32)pPacket->length();
			RECLAIM_PACKET(m_isTCPPacket, pPacket);
			m_packets.pop_back();
		}
	}

	if(m_pCurrPacket)
	{
		m_packets.push_back(m_pCurrPacket);
		m_pCurrPacket = NULL;
	}

	--m_numMessages;

	return size == 0;
}

}
}