#include "slbundle.h"
#include "sltcp_packet.h"
#include "sludp_packet.h"
namespace sl
{
namespace network
{

static CObjectPool<Bundle> g_objPool("Bundle");
CObjectPool<Bundle>& Bundle::ObjPool()
{
	return g_objPool;
}

Bundle* Bundle::createPoolObject()
{
	return g_objPool.FetchObj();
}

void Bundle::reclaimPoolObject(Bundle* obj)
{
	g_objPool.ReleaseObj(obj);
}

void Bundle::destoryObjPool()
{
	g_objPool.Destroy();
}

size_t Bundle::getPoolObjectBytes()
{
	size_t bytes = sizeof(m_pCurrMsgHandler) + sizeof(m_isTCPPacket) + 
		sizeof(m_currMsgLengthPos) + sizeof(m_currMsgHandlerLength) + sizeof(m_currMsgLength)+
		sizeof(m_currMsgPacketCount) + sizeof(m_currMsgID) + sizeof(m_numMessages) + sizeof(m_pChannel)+
		(m_packets.size() * sizeof(Packet*));
	return bytes;
}

Bundle::SmartPoolObjectPtr Bundle::createSmartPoolObj()
{
	return SmartPoolObjectPtr(new SmartPoolObject<Bundle>(ObjPool().FetchObj(), g_objPool));
}

Bundle::Bundle(Channel* pChannel /* = NULL */, ProtocolType pt /* = PROTOCOL_TCP */)
	:m_pChannel(pChannel),
	 m_numMessages(0),
	 m_pCurrPacket(NULL),
	 m_currMsgID(0),
	 m_currMsgPacketCount(0),
	 m_currMsgLength(0),
	 m_currMsgHandlerLength(0),
	 m_currMsgLengthPos(0),
	 m_packets(),
	 m_isTCPPacket(pt == PROTOCOL_TCP),
	 m_packetMaxSize(0),
	 m_pCurrMsgHandler(NULL)
{
	calcPacketMaxSize();
	newPacket();
}

Bundle::Bundle(const Bundle& bundle)
{
	m_isTCPPacket = bundle.m_isTCPPacket;
	m_pChannel = bundle.m_pChannel;
	m_pCurrMsgHandler = bundle.m_pCurrMsgHandler;
	m_currMsgID = bundle.m_currMsgID;
	
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
	m_currMsgPacketCount = bundle.m_currMsgPacketCount;
	m_currMsgLength = bundle.m_currMsgLength;
	m_currMsgHandlerLength = bundle.m_currMsgHandlerLength;
	m_currMsgLengthPos = bundle.m_currMsgLengthPos;
	calcPacketMaxSize();
}

Bundle::~Bundle()
{
	clear(false);
}

void Bundle::onReclaimObject()
{
	clear(true);
}

void Bundle::calcPacketMaxSize()
{
	/*if(g_channelExternalEncryptType == 1)
	{
		m_packetMaxSize = m_isTCPPacket ? (int)(TCPPacket::maxBufferSize() - ENCRYPTTION_WASTAGE_SIZE):
			(PACKET_MAX_SIZE_UDP - ENCRYPTTION_WASTAGE_SIZE);
	}
	else
	{*/
		m_packetMaxSize = m_isTCPPacket ? (int)TCPPacket::maxBufferSize(): PACKET_MAX_SIZE_UDP;
	//}
}

int32 Bundle::packetsLength(bool calccurr /* = true */)
{
	int32 len = 0;
	Packets::iterator iter = m_packets.begin();
	for (; iter != m_packets.end(); ++iter)
	{
		len += (int)(*iter)->length();

	}
	if(calccurr && m_pCurrPacket)
		len += (int)m_pCurrPacket->length();
	return len;
}

int32 Bundle::onPacketAppend(int32 addsize, bool inseparable /* = true */)
{
	if(m_pCurrPacket == NULL)
	{
		newPacket();
	}

	int32 totalsize = (int32)m_pCurrPacket->length();
	int fwpos = (int32)m_pCurrPacket->wpos();

	if(inseparable)
		fwpos += addsize;

	//�����ǰ���b���±���append�Ĕ�����������䵽�°���
	if(fwpos > m_packetMaxSize)
	{
		m_packets.push_back(m_pCurrPacket);
		m_currMsgPacketCount++;
		newPacket();
		totalsize = 0;
	}

	int32 remainSize = m_packetMaxSize - totalsize;
	int32 taddsize = addsize;

	//�����ǰ��ʣ�N���gС�Ҫ��ӵ��ֹ��t������M�˰�
	if(remainSize < addsize)
		taddsize = remainSize;

	m_currMsgLength += taddsize;
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

	m_currMsgID  = 0;
	m_currMsgPacketCount = 0;
	m_currMsgLength = 0;
	m_currMsgLengthPos = 0;
	m_pCurrMsgHandler = NULL;
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

void Bundle::newMessage(const MessageHandler& msgHandler)
{
	m_pCurrMsgHandler = &msgHandler;

	if(m_pCurrPacket == NULL)
	{
		if(m_packets.size() > 0)
		{
			if(packetHaveSpace())
			{
				m_pCurrPacket = m_packets.back();
				m_packets.pop_back();
			}
			else
			{
				this->newPacket();
			}
		}
		else
		{
			this->newPacket();
		}
	}

	finiMessage(false);
	SL_ASSERT(m_pCurrPacket != NULL);

	(*this)<<msgHandler.msgID;
	m_pCurrPacket->SetMessageID(msgHandler.msgID);

	//��̎��춷ǹ̶��L�ȵ���Ϣ���f��Ҫ�O��������Ϣ�L��λ��0����������Ҫ����L��
	if(msgHandler.msgLen == NETWORK_VARIABLE_MESSAGE)
	{
		MessageLength msgLen = 0;
		m_currMsgLengthPos = m_pCurrPacket->wpos();
		(*this) << msgLen;
	}

	++m_numMessages;
	m_currMsgID = msgHandler.msgID;
	m_currMsgPacketCount = 0;
	m_currMsgHandlerLength = msgHandler.msgLen;
}

void Bundle::finiMessage(bool isSend /* = true */)
{
	SL_ASSERT(m_pCurrPacket != NULL);
	m_pCurrPacket->SetBundle(this);

	if(isSend)
	{
		++m_currMsgPacketCount;
		m_packets.push_back(m_pCurrPacket);
	}

	//����Ϣ���и���
	if(m_pCurrMsgHandler){
		if(isSend || m_numMessages > 1)
		{
			//Ne
		}
	}

	//�˴����ڷǹ̶����ȵ���Ϣ��˵��Ҫ�����������ճ�����Ϣ
	if(m_currMsgID > 0 && (m_currMsgHandlerLength < 0))
	{
		Packet* pPacket = m_pCurrPacket;
		if(m_currMsgPacketCount > 0)
			pPacket = m_packets[m_packets.size() - m_currMsgPacketCount];
		m_currMsgLength -= NETWORK_MESSAGE_ID_SIZE;
		m_currMsgLength -= NETWORK_MESSAGE_LENGTH_SIZE;

		//�������һ��������ܳ���NETWORK_MESSAGE_MAX_SIZE
		/*if(g_componentType == BOTS_TYPE || g_componentType == CLIENT_TYPE)
		{
			SL_ASSERT(m_currMsgLength <= NETWORK_MESSAGE_MAX_SIZE);
		}*/

		//�����Ϣ���ȴ��ڵ���NETWORK_MESSAGE_MAX_SIZE
		//ʹ����չ��Ϣ���Ȼ��ƣ�����Ϣ���Ⱥ��������4�ֽ�
		//������������ĳ���
		if(m_currMsgLength >= NETWORK_MESSAGE_MAX_SIZE)
		{
			MessageLength1 ex_msg_length = m_currMsgLength;
			sl::EndianConvert(ex_msg_length);

			MessageLength msgLen = NETWORK_MESSAGE_MAX_SIZE;
			sl::EndianConvert(msgLen);

			memcpy(&pPacket->data()[m_currMsgLengthPos],(uint8*)&msgLen, NETWORK_MESSAGE_LENGTH_SIZE);

			pPacket->insert(m_currMsgLengthPos + NETWORK_MESSAGE_LENGTH_SIZE, (uint8*)&ex_msg_length,
				NETWORK_MESSAGE_LENGTH1_SIZE);
		}
		else
		{
			MessageLength msgLen = (MessageLength)m_currMsgLength;
			sl::EndianConvert(msgLen);

			memcpy(&pPacket->data()[m_currMsgLengthPos], (uint8*)&msgLen, NETWORK_MESSAGE_LENGTH_SIZE);
		}
	}

	//if()

	if(isSend)
	{
		m_currMsgHandlerLength = 0;
		m_pCurrPacket = NULL;
	}

	m_currMsgID = 0;
	m_currMsgPacketCount = 0;
	m_currMsgLength = 0;
	m_currMsgLengthPos = 0;
}

void Bundle::debugCurrentMessages(MessageID currMsgID, const network::MessageHandler* pCurrMsgHandler, 
								  network::Packet* pCurrPacket, Bundle::Packets& packets, 
								  MessageLength1 currMsgLength, network::Channel* pChannel)
{
	if(currMsgID == 0)
		return;

	if(!pCurrMsgHandler || currMsgID != pCurrMsgHandler->msgID || !pCurrMsgHandler->pMessageHandlers)
		return;

	if(pCurrMsgHandler->msgLen == NETWORK_VARIABLE_MESSAGE)
	{
		currMsgLength += NETWORK_MESSAGE_ID_SIZE;
		currMsgLength += NETWORK_MESSAGE_LENGTH_SIZE;
		if(currMsgLength -  NETWORK_MESSAGE_ID_SIZE - NETWORK_MESSAGE_LENGTH_SIZE >= NETWORK_MESSAGE_MAX_SIZE)
			currMsgLength += NETWORK_MESSAGE_LENGTH1_SIZE;
	}

	MemoryStream* pMemoryStream = MemoryStream::createPoolObject();

	//ͨ����Ϣ�����ҵ���Ϣͷ��Ȼ����Ϣ�������
	int msglen = currMsgLength;
	if(pCurrPacket)
	{
		//�����ǰ��Ϣ�������ݶ��ڵ�ǰ���У�ֱ��������ݼ���
		msglen -=pCurrPacket->length();
		if(msglen <= 0)
		{
			pMemoryStream->append(pCurrPacket->data() + pCurrPacket->wpos() - currMsgLength, currMsgLength);
		}
		else
		{
			int idx = 0;
			Bundle::Packets::reverse_iterator packIter = packets.rbegin();
			for (; packIter != packets.rend(); ++packIter)
			{
				++idx;
				network::Packet* pPacket = (*packIter);

				///��ǰ�������Ѿ������
				if(pCurrPacket == pPacket)
					continue;

				//����������ݶ��ڰ���
				if((int)pPacket->length() >= msglen)
				{
					int wpos = pPacket->length() - msglen;
					pMemoryStream->append(pPacket->data() + wpos, msglen);

					for (size_t i = packets.size() - idx; i < packets.size(); ++i)
					{
						network::Packet* pPacket1 = packets[i];
						//������Ѿ������洦�����
						if(pPacket1 == pPacket || pCurrPacket == pPacket1)
							continue;

						//�ڼ�İ�����ȫ������
						pMemoryStream->append(pCurrPacket->data() + pCurrPacket->rpos(), pCurrPacket->length());
					}

					//�ѵ�ǰ�İ����ݼ���
					pMemoryStream->append(pCurrPacket->data() + pCurrPacket->rpos(), pCurrPacket->length());
					break;
				}
				else
				{
					msglen -= pPacket->length();
				}
			}
		}
	}

	//һЩsendto�����İ����£�
	if(pMemoryStream->length() < NETWORK_MESSAGE_ID_SIZE)
	{
		MemoryStream::reclaimPoolObject(pMemoryStream);
		return;
	}

	SL_ASSERT(currMsgLength == pMemoryStream->length());

	//TRACE_MESSAGE_COMPONENTID

	MemoryStream::reclaimPoolObject(pMemoryStream);
}

bool Bundle::revokeMessage(int32 size)
{
	if(m_pCurrPacket)
	{
		if(size >= (int32)m_pCurrPacket->wpos())
		{
			size -= m_pCurrPacket->wpos();
			RECLAIM_PACKET(m_isTCPPacket, m_pCurrPacket);
			m_pCurrPacket = NULL;
		}
		else
		{
			m_pCurrPacket->wpos(m_pCurrPacket->wpos()- size);
			size = 0;
		}
	}

	while(size > 0 && m_packets.size() > 0)
	{
		network::Packet* pPacket = m_packets.back();
		if(pPacket->wpos() > (size_t)size)
		{
			pPacket->wpos(pPacket->wpos() - size);
			size = 0;
			break;
		}
		else
		{
			size -= pPacket->length();
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
	m_currMsgHandlerLength = 0;

	m_currMsgID = 0;
	m_currMsgPacketCount = 0;
	m_currMsgLength = 0;
	m_currMsgLengthPos = 0;
	m_pCurrMsgHandler = NULL;

	return size == 0;
}

}
}