#include "slpacket_reader.h"
#include "slchannel.h"
namespace sl
{
namespace network
{

PacketReader::PacketReader(Channel* pChannel)
	:m_pFragmentDatas(NULL),
	 m_pFragmentDatasWpos(0),
	 m_pFragmentDatasRemain(0),
	 m_pFragmentDatasType(FRAGMENT_DATA_UNKNOW),
	 m_pFragmentStream(NULL),
	 m_currMsgID(0),
	 m_currMsgLen(0),
	 m_pChannel(pChannel)
{}

PacketReader::~PacketReader()
{
	reset();
	m_pChannel = NULL;
}

void PacketReader::reset()
{
	m_pFragmentDatasType = FRAGMENT_DATA_UNKNOW;
	m_pFragmentDatasWpos = 0;
	m_pFragmentDatasRemain = 0;
	m_currMsgID = 0;
	m_currMsgLen = 0;

	MemoryStream::reclaimPoolObject(m_pFragmentStream);
	m_pFragmentStream = NULL;
}

void PacketReader::processMessages(/*sl::network::MessageHandlers* pMsgHandlers,*/ Packet* pPacket)
{
	while(pPacket->length() > 0 || m_pFragmentStream != NULL)
	{
		if(m_pFragmentDatasType == FRAGMENT_DATA_UNKNOW)
		{
			//���û��ID��Ϣ �Ȼ�ȡOD
			if(m_currMsgID == 0)
			{
				if(NETWORK_MESSAGE_ID_SIZE > 1 && pPacket->length() < NETWORK_MESSAGE_ID_SIZE)
				{
					writeFragmentMessage(FRAGMENT_DATA_MESSAGE_ID, pPacket, NETWORK_MESSAGE_ID_SIZE);
					break;
				}

				(*pPacket) >> m_currMsgID;
				pPacket->SetMessageID(m_currMsgID);
			}

			//network::MessageHandler* pMsgHandler = pMsgHandlers->find(m_currMsgID);

			//if(pMsgHandler == NULL)
			//{
			//	MemoryStream* pPacket1 = m_pFragmentStream != NULL ? m_pFragmentStream : pPacket;
			//	//TRACE_MESSAGE_PACKET

			//	//���ڵ���ʱ�ȶ�
			//	uint32 rpos = pPacket1->rpos();
			//	pPacket1->rpos(0);
			//	//TRACE_MESSAGE_PACKET
			//	pPacket1->rpos(rpos);

			//	m_currMsgID = 0;
			//	m_currMsgLen = 0;
			//	m_pChannel->condemn();
			//	break;
			//}

			//���û�пɲ������������˳��ȴ���һ������
			//������һ���޲������ݰ�
			
			//���������Ϣû�л�ã���ȴ���ȡ������Ϣ
			if(m_currMsgLen == 0)
			{
				//���������Ϣ�ǿɱ�Ļ�����������Զ����������Ϣѡ��ʱ�������з�����������
				/*if(pMsgHandler->msgLen == NETWORK_VARIABLE_MESSAGE)
				{*/
					//���������Ϣ����������ȴ���һ������
					if(pPacket->length() < NETWORK_MESSAGE_LENGTH_SIZE)
					{
						writeFragmentMessage(FRAGMENT_DATA_MESSAGE_LENGTH, pPacket, NETWORK_MESSAGE_LENGTH_SIZE);
						break;
					}
					else
					{
						//�˴�����˳�����Ϣ
						network::MessageLength currlen;
						(*pPacket) >> currlen;
						m_currMsgLen = currlen;

						//�������ռ��˵����ʹ����չ���ȣ����ǻ���Ҫ�ȴ���չ������Ϣ
						if(m_currMsgLen == NETWORK_MESSAGE_MAX_SIZE)
						{
							if(pPacket->length() < NETWORK_MESSAGE_LENGTH1_SIZE)
							{
								//���������Ϣ����������ȴ���һ������
								writeFragmentMessage(FRAGMENT_DATA_MESSAGE_LENGTH1, pPacket, NETWORK_MESSAGE_LENGTH1_SIZE);
								break;
							}
							else
							{
								//�˴��������չ������Ϣ
								(*pPacket) >> m_currMsgLen;

							}
						}
					}
				/*}
				else
				{
					m_currMsgLen = pMsgHandler->msgLen;
				}*/
			}

			if(this->m_pChannel->isExternal()/* && 
				g_componentType != BOTS_TYPE &&
				g_componentType != CLIENT_TYPE &&*/
				&& m_currMsgLen > NETWORK_MESSAGE_MAX_SIZE)
			{
				MemoryStream* pPacket1 = m_pFragmentStream != NULL ? m_pFragmentStream : pPacket;
				//TRACE_MESSAGE_PACKET

				//��������ʱ�ȶ�
				uint32 rpos = pPacket1->rpos();
				pPacket1->rpos(0);
				//TRACE_MESSAGE_PACKET
				pPacket1->rpos(rpos);

				m_currMsgLen = 0;
				m_pChannel->condemn();
				break;
			}

			if(m_pFragmentStream != NULL)
			{
				//TRACE_MESSAGE_PACKET
				//pMsgHandler->handle(m_pChannel, *m_pFragmentStream);
				ISLSession* poSession = this->m_pChannel->getSession();
				if(NULL == poSession){
					m_pChannel->condemn();
					break;
				}
				poSession->onRecv((const char*)m_pFragmentStream->data(), m_pFragmentStream->length());
				MemoryStream::reclaimPoolObject(m_pFragmentStream);
				m_pFragmentStream = NULL;
			}
			else
			{
				if(pPacket->length() < m_currMsgLen)
				{
					writeFragmentMessage(FRAGMENT_DATA_MESSAGE_BODY, pPacket, m_currMsgLen);
					break;
				}

				//��ʱ������Ч��ȡλ����ֹ�ӿ����������
				size_t wpos = pPacket->wpos();
				size_t frpos = pPacket->rpos() + m_currMsgLen;
				pPacket->wpos(frpos);

				//TRACE_MESSAGE_PACKET
				ISLSession* poSession = this->m_pChannel->getSession();
				if(NULL == poSession){
					m_pChannel->condemn();
					break;
				}
				poSession->onRecv((const char*)m_pFragmentStream->data(), m_pFragmentStream->length());

				//���handlerû�д��������������һ������
				if(m_currMsgLen > 0)
				{
					if(frpos != pPacket->rpos())
					{
						pPacket->rpos(frpos);
					}
				}
				pPacket->wpos(wpos);
			}

			m_currMsgID = 0;
			m_currMsgLen = 0;
		}
		else
		{
			mergeFragmentMessage(pPacket);
		}
	}
}

void PacketReader::writeFragmentMessage(FragmentDataTypes fragmentDataFlag, Packet* packet, uint32 datasize)
{
	SL_ASSERT(m_pFragmentDatas == NULL);

	size_t opsize = packet->length();
	m_pFragmentDatasRemain = datasize - opsize;
	m_pFragmentDatas = new uint8[opsize + m_pFragmentDatasRemain + 1];

	m_pFragmentDatasType = fragmentDataFlag;
	m_pFragmentDatasWpos = opsize;

	if(packet->length() > 0)
	{
		memcpy(m_pFragmentDatas, packet->data() + packet->rpos(), opsize);
		packet->done();
	}
}

void PacketReader::mergeFragmentMessage(Packet* pPacket)
{
	size_t opsize = pPacket->length();
	if(opsize == 0)
		return;

	if(pPacket->length() >= m_pFragmentDatasRemain)
	{
		memcpy(m_pFragmentDatas + m_pFragmentDatasWpos, pPacket->data() + pPacket->rpos(), m_pFragmentDatasRemain);
		pPacket->rpos(pPacket->rpos() + m_pFragmentDatasRemain);

		SL_ASSERT(m_pFragmentStream == NULL);

		switch(m_pFragmentDatasType)
		{
		case FRAGMENT_DATA_MESSAGE_ID:		///��ϢID��Ϣ��ȫ
			memcpy(&m_currMsgID, m_pFragmentDatas, NETWORK_MESSAGE_ID_SIZE);
			break;
			
		case FRAGMENT_DATA_MESSAGE_LENGTH:	///< ��Ϣ������Ϣ��ȫ
			memcpy(&m_currMsgLen, m_pFragmentDatas, NETWORK_MESSAGE_LENGTH_SIZE);
			break;

		case FRAGMENT_DATA_MESSAGE_LENGTH1:	///< ��Ϣ������Ϣ��ȫ
			memcpy(&m_currMsgLen, m_pFragmentDatas, NETWORK_MESSAGE_LENGTH1_SIZE);
			break;

		case FRAGMENT_DATA_MESSAGE_BODY:	///< ��Ϣ������Ϣ��ȫ
			m_pFragmentStream = MemoryStream::createPoolObject();
			m_pFragmentStream->append(m_pFragmentDatas, m_currMsgLen);
			break;

		default:
			break;
		}

		m_pFragmentDatasType = FRAGMENT_DATA_UNKNOW;
		m_pFragmentDatasRemain = 0;
		//SAFE_RELEASE_ARRAY
	}
	else
	{
		memcpy(m_pFragmentDatas + m_pFragmentDatasWpos, pPacket->data(), opsize);
		m_pFragmentDatasRemain -= opsize;
		m_pFragmentDatasWpos += opsize;
		pPacket->rpos(pPacket->rpos() + opsize);
	}
}

}
}