#include "slshm_queue.h"

namespace sl{
namespace shm{
SLShmQueue::SLShmQueue(bool bBackEnd, const char* pszShmKey, int iShmSize)
	:m_pstRecvQueue(NULL),
	m_pstSendQueue(NULL),
	m_bBackEnd(bBackEnd)
{
	Init(m_bBackEnd, pszShmKey, iShmSize);
}

int SLShmQueue::Init(bool bBackEnd, const char* pszShmKey, int iShmSize){
	SLASSERT(pszShmKey && iShmSize > 0, "wtf");

	m_bBackEnd = bBackEnd;

	int iRet = 0;

	//��ʼ��ǰ�˹����ڴ�
	iRet = m_stShm.Create(pszShmKey, iShmSize);
	if (iRet){
		SLASSERT(false, "m_stShm.Create failed %d. key=%s size=%d", iRet, pszShmKey, iShmSize);
		return iRet;
	}

	ECHO_TRACE("create shmqueue's shm at %p ok, key=%s size=%d", m_stShm.GetBuffer(), pszShmKey, iShmSize);

	//��ʼ����Ϣ����
	const int iQueueSize = m_stShm.GetSize() / 2;
	m_pstRecvQueue = (CCodeQueue*)(m_stShm.GetBuffer() + (m_bBackEnd ? 0 : iQueueSize));
	if (m_pstRecvQueue->CheckSum()){
		ECHO_TRACE("m_pstRecvQueue need create");
		iRet = m_pstRecvQueue->Init(iQueueSize);
		if (iRet){
			SLASSERT(false, "m_pstRecvQueue init failed %d", iRet);
			return iRet;
		}
	}

	m_pstSendQueue = (CCodeQueue*)(m_stShm.GetBuffer() + (m_bBackEnd ? iQueueSize : 0));
	if (m_pstSendQueue->CheckSum()){
		ECHO_TRACE("m_pstSendQueue need create");
		iRet = m_pstSendQueue->Init(iQueueSize);
		if (iRet){
			SLASSERT(false, "m_pstSendQueue init failed %d", iRet);
			return iRet;
		}
	}
	return iRet;
}
}
}