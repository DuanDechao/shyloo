#ifndef _SL_NETWORK_BUNDLE_H_
#define _SL_NETWORK_BUNDLE_H_
#include "slobjectpool.h"
#include "slbase.h"
#include "slpacket.h"
#include "slnetbase.h"
namespace sl
{
namespace network
{
class Channel;
class NetworkInterface;

#define PACKET_OUT_VALUE(v, expectSize)																		\
	SL_ASSERT(packetsLength() >= (int32)expectSize);														\
																											\
	size_t currSize = 0;																					\
	size_t reclaimCount = 0;																				\
																											\
	Packets::iterator iter = m_packets.begin();																\
	for (; iter != m_packets.end(); ++iter)																	\
	{																										\
		Packet* pPacket = (*iter);																			\
		size_t remainSize = (size_t)expectSize - currSize;													\
																											\
		if(pPacket->length() >= remainSize)																	\
		{																									\
			memcpy(((uint8*)&v) + currSize, pPacket->data() + pPacket->rpos(), remainSize);					\
			pPacket->rpos((int)(pPacket->rpos() + remainSize));												\
																											\
			if(pPacket->length() == 0)																		\
			{																								\
				RECLAIM_PACKET(pPacket->isTCPPacket(), pPacket);											\
				++reclaimCount;																				\
			}																								\
																											\
			break;																							\
		}																									\
		else																								\
		{																									\
			memcpy(((uint8*)&v) + currSize, pPacket->data() + pPacket->rpos(), pPacket->length());			\
			currSize += pPacket->length();																	\
			pPacket->done();																				\
			RECLAIM_PACKET(pPacket->isTCPPacket(), pPacket);												\
			++reclaimCount;																					\
		}																									\
	}																										\
																											\
	if(reclaimCount > 0)																					\
		packets_.erase(m_packets.begin(), m_packets.begin() + reclaimCount);								\
																											\
	return *this;																							\

	
	
	//从对象池中创建和回收
#define MALLOC_BUNDLE() network::Bundle::createPoolObject()
#define DELETE_BUNDLE(obj) {network::Bundle::reclaimPoolObject(obj); obj = NULL;}
#define RECLAIM_BUNDLE(obj) {network::Bundle::reclaimPoolObject(obj);}

class Bundle: public PoolObject
{
public:
	typedef SLShared_ptr<SmartPoolObject<Bundle>> SmartPoolObjectPtr;
	static SmartPoolObjectPtr createSmartPoolObj();
	static CObjectPool<Bundle>& ObjPool();
	static Bundle* createPoolObject();
	static void reclaimPoolObject(Bundle* obj);
	static void destoryObjPool();
	virtual void onReclaimObject();
	virtual size_t getPoolObjectBytes();

	typedef std::vector<Packet*> Packets;

	Bundle(Channel* pChannel = NULL, ProtocolType pt = PROTOCOL_TCP);
	Bundle(const Bundle& bundle);
	virtual ~Bundle();

	void newMessage(/*const MessageHandler& msgHandler*/ const MessageID msgID);
	void finiMessage(bool isSend = true);

	void clearPackets();

//	inline void setCurrMsgHandler(const network::MessageHandler* pMsgHandler);
	//inline const network::MessageHandler* getCurrMsgHandler() const;

	///計算所有包包括當前還未寫完的包得總長度
	int32 packetsLength(bool calccurr = true);

	inline bool isTCPPacket() const {return m_isTCPPacket;}
	inline void isTCPPacket(bool v) {m_isTCPPacket = v;}

	void clear(bool isRecl);
	bool empty() const;

	inline int32 packetMaxSize() const;
	int packetsSize() const;

	//撤銷一些消息字節
	bool revokeMessage(int32 size);

	//計算packetMaxSize最後一個包得length后剩餘的可用空間
	inline int32 lastPacketSpace();
	inline bool packetHaveSpace();

	inline Packets& packets();
	inline Packet* getCurrPacket() const;
	inline void setCurrPacket(Packet* p);

	inline void finiCurrPacket();

	Packet* newPacket();

	inline void setChannel(Channel* pChannel);
	inline Channel* getChannel() const;

	inline MessageID getMessageID() const;
	inline void setMessageID(MessageID id);

	inline int32 numMessages() const;

	inline void setCurrMsgPacketCount(uint32 v);
	inline uint32 getCurrMsgPacketCount() const;

	inline void setCurrMsgLength(MessageLength1 v);
	inline MessageLength1 getCurrMsgLength() const;

	inline void setCurrMsgLengthPos(size_t v);
	inline size_t getCurrMsgLengthPos() const;

	/*static void debugCurrentMessages(MessageID currMsgID, const network::MessageHandler* pCurrMsgHandler,
		network::Packet* pCurrPacket, Bundle::Packets& packets, MessageLength1 currMsgLength,
		network::Channel* pChannel);*/

protected:
	void calcPacketMaxSize();
	int32 onPacketAppend(int32 addsize, bool inseparable = true);

public:
	Bundle& operator<<(uint8 value)
	{
		onPacketAppend(sizeof(uint8));
		(*m_pCurrPacket) << value;
		return *this;
	}

	Bundle& operator<<(uint16 value)
	{
		onPacketAppend(sizeof(uint16));
		(*m_pCurrPacket) << value;
		return *this;
	}

	Bundle& operator<<(uint32 value)
	{
		onPacketAppend(sizeof(uint32));
		(*m_pCurrPacket)<<value;
		return *this;
	}

	Bundle& operator<<(uint64 value)
	{
		onPacketAppend(sizeof(uint64));
		(*m_pCurrPacket)<<value;
		return *this;
	}

	Bundle& operator<<(int8 value)
	{
		onPacketAppend(sizeof(int8));
		(*m_pCurrPacket) << value;
		return *this;
	}

	Bundle& operator<<(int16 value)
	{
		onPacketAppend(sizeof(int16));
		(*m_pCurrPacket) << value;
		return *this;
	}

	Bundle& operator<<(int32 value)
	{
		onPacketAppend(sizeof(int32));
		(*m_pCurrPacket)<<value;
		return *this;
	}

	Bundle& operator<<(int64 value)
	{
		onPacketAppend(sizeof(int64));
		(*m_pCurrPacket)<<value;
		return *this;
	}

	Bundle& operator<<(float value)
	{
		onPacketAppend(sizeof(float));
		(*m_pCurrPacket)<<value;
		return *this;
	}

	Bundle& operator<<(double value)
	{
		onPacketAppend(sizeof(double));
		(*m_pCurrPacket)<<value;
		return *this;
	}

	//Bundle& operator<<(COMPONENT_TYPE value)
	//{
	//	onPacketAppend(sizeof(COMPONENT_TYPE));
	//	(*m_pCurrPacket) << value;
	//	return *this;
	//}

	/*Bundle& operator<<(ENTITY_MAILBOX_TYPE value)
	{
		onPacketAppend(sizeof(ENTITY_MAILBOX_TYPE));
		(*m_pCurrPacket) << value;
		return *this;
	}*/

	Bundle& operator<<(bool value)
	{
		onPacketAppend(sizeof(int8));
		(*m_pCurrPacket)<<value;
		return *this;
	}

	Bundle& operator<<(const std::string& value)
	{
		int32 len = (int32)value.size() + 1;
		int32 addtotalsize = 0;

		while(len > 0)
		{
			int32 ilen = onPacketAppend(len, false);
			m_pCurrPacket->append(value.c_str() + addtotalsize, ilen);
			addtotalsize += ilen;
			len -= ilen;
		}
		return *this;
	}

	Bundle& operator <<(const char* str)
	{
		int32 len = (int32)strlen(str) + 1;
		int32 addtotalsize = 0;
		while(len > 0)
		{
			int32 iLen = onPacketAppend(len, false);
			m_pCurrPacket->append(str + addtotalsize, iLen);
			addtotalsize += iLen;
			len -= iLen;
		}

		return *this;
	}

	Bundle& append(Bundle* pBundle)
	{
		SL_ASSERT(pBundle != NULL);
		return append(*pBundle);
	}

	Bundle& append(Bundle& bundle)
	{
		Packets::iterator iter = bundle.m_packets.begin();
		for(; iter != bundle.m_packets.end(); ++iter)
		{
			append((*iter)->data() + (*iter)->rpos(), (int)(*iter)->length());
		}

		if(bundle.m_pCurrPacket == NULL)
			return *this;

		return append(bundle.m_pCurrPacket->data() + bundle.m_pCurrPacket->rpos(), (int)bundle.m_pCurrPacket->length());
	}

	Bundle& append(MemoryStream* s)
	{
		SL_ASSERT(s != NULL);
		return append(*s);
	}

	Bundle& append(MemoryStream& s)
	{
		if(s.length() > 0)
			return append(s.data() + s.rpos(), (int)s.length());

		return *this;
	}

	Bundle& appendBlob(const std::string& str)
	{
		return appendBlob((const uint8*)str.data(), (uint32)str.size());
	}

	Bundle& appendBlob(const char* str, uint32 n)
	{
		return appendBlob((const uint8*)str, n);
	}

	Bundle& appendBlob(const uint8* str, uint32 n)
	{
		(*this) << n;
		return assign((char*)str, n);
	}

	Bundle& append(const uint8* str, int n)
	{
		return assign((char*)str, n);
	}
	Bundle& append(const char* str, int n)
	{
		return assign(str, n);
	}

	Bundle& assign(const char* str, int n)
	{
		int32 len = (int32)n;
		int32 addtotalsize = 0;

		while(len > 0)
		{
			int32 ilen = onPacketAppend(len, false);
			m_pCurrPacket->append((uint8*)(str + addtotalsize), ilen);
			addtotalsize += ilen;
			len -= ilen;
		}
		return *this;
	}

private:
	Channel*			m_pChannel;
	int32				m_numMessages;

	Packet*				m_pCurrPacket;
	MessageID			m_currMsgID;
	uint32				m_currMsgPacketCount;
	MessageLength1		m_currMsgLength;
	int32				m_currMsgHandlerLength;
	size_t				m_currMsgLengthPos;

	Packets				m_packets;

	bool				m_isTCPPacket;
	int32				m_packetMaxSize;
	
	//const network::MessageHandler* m_pCurrMsgHandler;
};
}
}
#include "slbundle.inl"
#endif