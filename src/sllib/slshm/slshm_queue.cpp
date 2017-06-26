#include "slshm_queue.h"

namespace sl{
namespace shm{
SLShmQueue::SLShmQueue(bool bBackEnd, const char* pszShmKey, int sendSize, int recvSize, bool clear)
	:m_pstRecvQueue(NULL),
	m_pstSendQueue(NULL),
	m_bBackEnd(bBackEnd),
	m_shmSendSize(sendSize),
	m_shmRecvSize(recvSize)
{
	Init(m_bBackEnd, pszShmKey, sendSize, recvSize, clear);
}

int SLShmQueue::Init(bool bBackEnd, const char* pszShmKey, int sendSize, int recvSize, bool clear){
	SLASSERT(pszShmKey && sendSize > 0 && recvSize > 0, "wtf");

	m_bBackEnd = bBackEnd;

	int iRet = 0;

	//初始化前端共享内存
	iRet = m_stShm.Create(pszShmKey, sendSize + recvSize);
	if (iRet){
		SLASSERT(false, "m_stShm.Create failed %d. key=%s size=%d", iRet, pszShmKey, sendSize + recvSize);
		return iRet;
	}

	ECHO_TRACE("create shmqueue's shm at %p ok, key=%s size=%d", m_stShm.GetBuffer(), pszShmKey, sendSize + recvSize);

	//初始化消息队列
	m_pstRecvQueue = (CCodeQueue*)(m_stShm.GetBuffer() + (m_bBackEnd ? 0 : sendSize));
	if (m_pstRecvQueue->CheckSum()){
		ECHO_TRACE("m_pstRecvQueue need create");
		iRet = m_pstRecvQueue->Init(recvSize, clear);
		if (iRet){
			SLASSERT(false, "m_pstRecvQueue init failed %d", iRet);
			return iRet;
		}
	}

	m_pstSendQueue = (CCodeQueue*)(m_stShm.GetBuffer() + (m_bBackEnd ? recvSize : 0));
	if (m_pstSendQueue->CheckSum()){
		ECHO_TRACE("m_pstSendQueue need create");
		iRet = m_pstSendQueue->Init(sendSize, clear);
		if (iRet){
			SLASSERT(false, "m_pstSendQueue init failed %d", iRet);
			return iRet;
		}
	}
	return iRet;
}
}
}