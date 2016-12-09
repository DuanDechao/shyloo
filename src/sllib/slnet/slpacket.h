#ifndef _SL_SOCKETPACKET_H_
#define _SL_SOCKETPACKET_H_
#include "slnetbase.h"
#include "slmemorystream.h"
#include "slsmartpointer.h"
#include "slobjectpool.h"
namespace sl
{
namespace network
{
class Bundle;
class Packet: public MemoryStream
{
public:
	Packet(bool isTCPPacket = true, size_t res = 200)
		:MemoryStream(res),
		 m_bIsTCPPacket(isTCPPacket),
		 m_pBundle(NULL),
		 m_sentSize(0)
	{}

	virtual ~Packet(void){}

	virtual void onReclaimObject()
	{
		MemoryStream::onReclaimObject();
		resetPacket();
	}

	virtual size_t getPoolObjectBytes()
	{
		size_t bytes = sizeof(m_bIsTCPPacket) + sizeof(m_pBundle)
			+ sizeof(m_sentSize);
		
		return MemoryStream::getPoolObjectBytes() + bytes;
	}

	Bundle* GetBundle() const {return m_pBundle;}
	void SetBundle(Bundle* b) {m_pBundle = b;}

	void resetPacket(void)
	{
		wpos(0);
		rpos(0);
		m_sentSize = 0;
		m_pBundle = NULL;
	}

	void IsTCPPacket(bool is) {m_bIsTCPPacket = is;}
	bool IsTCPPacket() const {return m_bIsTCPPacket;}

public:
	uint32			m_sentSize;
protected:
	bool			m_bIsTCPPacket;
	Bundle*			m_pBundle;
};
}
}
#endif