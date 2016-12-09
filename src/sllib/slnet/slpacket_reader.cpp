#include "slpacket_reader.h"
#include "slchannel.h"
namespace sl
{
namespace network
{

PacketReader::PacketReader(Channel* pChannel, ISLPacketParser* poPacketParser)
	:m_pFragmentStream(NULL),
	 m_pChannel(pChannel),
	 m_pPacketParser(poPacketParser),
	 m_pFragmentStreamLength(0)
{}

PacketReader::~PacketReader()
{
	reset();
	m_pChannel = NULL;
}

void PacketReader::reset()
{
	MemoryStream::reclaimPoolObject(m_pFragmentStream);
	m_pFragmentStream = NULL;
	m_pFragmentStreamLength = 0;
}

void PacketReader::processMessages( Packet* pPacket)
{
	ISLSession* poSession = this->m_pChannel->getSession();
	if(NULL == poSession){
		m_pChannel->condemn();
		return;
	}

	while(pPacket->length() > 0 || m_pFragmentStream != NULL)
	{
		if(m_pFragmentStream != NULL)
		{
			const char* pDataBuf = (const char* )m_pFragmentStream->data() + m_pFragmentStream->rpos();
			int32 parserLen = m_pPacketParser->parsePacket(pDataBuf, (int32)m_pFragmentStream->length());
			if(parserLen < 0){
				m_pChannel->condemn();
				return;
			}

			if(parserLen == 0){
				int32 mergeLen = mergeFragmentMessage(pPacket);
				if(mergeLen < 0){
					m_pChannel->condemn();
					return;
				}
				if(mergeLen == 0){
					m_pFragmentStreamLength += (uint32)pPacket->length();
					pPacket->done();
					break;
				}
			}
			if(parserLen > 0){
				poSession->onRecv((const char*)(m_pFragmentStream->data()+m_pFragmentStream->rpos()), (uint32)parserLen);
				MemoryStream::reclaimPoolObject(m_pFragmentStream);
				m_pFragmentStream = NULL;
				pPacket->read_skip(parserLen);
				m_pFragmentStreamLength = 0;
			}
		}
		else
		{
			const char* pDataBuf = (const char* )pPacket->data() + pPacket->rpos();
			int32 parserLen = m_pPacketParser->parsePacket(pDataBuf, (int32)pPacket->length());
			if(parserLen < 0){
				m_pChannel->condemn();
				return;
			}
			if(parserLen == 0){
				int32 mergeLen = mergeFragmentMessage(pPacket);
				if(mergeLen < 0){
					m_pChannel->condemn();
					return;
				}

				if(mergeLen == 0){
					m_pFragmentStreamLength += (uint32)pPacket->length();
					pPacket->done();
					break;
				}
			}
			if(parserLen > 0){
				poSession->onRecv((const char*)(pPacket->data()+ pPacket->rpos()), (uint32)parserLen);
				pPacket->read_skip(parserLen);
				m_pFragmentStreamLength = 0;
			}
		}

	}
}

int32 PacketReader::mergeFragmentMessage(Packet* pPacket)
{
	size_t opsize = pPacket->length();
	if(opsize == 0)
		return 0;

	if(m_pFragmentStream == nullptr)
		m_pFragmentStream = MemoryStream::createPoolObject();
	
	m_pFragmentStream->append(pPacket->data() + pPacket->rpos(), pPacket->length());
	
	const char* pDataBuf = (const char*)(m_pFragmentStream->data() + m_pFragmentStream->rpos());
	return m_pPacketParser->parsePacket(pDataBuf, (int32)m_pFragmentStream->length());
}

}
}