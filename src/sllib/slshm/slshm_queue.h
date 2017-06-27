//共享内存队列
/********************************************************************
	created:	2015/12/05
	created:	5:12:2015   21:50
	filename: 	e:\myproject\shyloo\sllib\slshm_queue.h
	file path:	e:\myproject\shyloo\sllib
	file base:	slshm_queue
	file ext:	h
	author:		ddc
	
	purpose:	共享内存队列
*********************************************************************/
#ifndef _SL_LIB_SHM_QUEUE_H_
#define _SL_LIB_SHM_QUEUE_H_

#include "slshm_create.h"
#include "slshm.h"
#include "slring_buffer.h"
namespace sl{
namespace shm{

//共享内存队列
class SLShmQueue: public ISLShmQueue{
public:
	SLShmQueue(bool bBackEnd, const char* pszShmKey, int sendSize, int recvSize, bool clear);
	virtual ~SLShmQueue(){}

	//初始化
	/*
	@param  [in] bBackEnd	是共享内存队列的前端还是后端
	@param  [in] pszShmKey	共享内存的Key
	@param  [in] iShmSize	共享内存的大小(这里是共享内存的总大小，单向队列的大小是1/2)
	@return 0表示成功
	*/
	int Init(bool bBackEnd, const char* pszShmKey, int sendSize, int recvSize, bool clear);

	virtual int SLAPI getRecvQueueFreeSize() const { return m_pstRecvQueue->getFreeSize(); }
	virtual int SLAPI getSendQueueFreeSize() const { return m_pstSendQueue->getFreeSize(); }
	virtual int SLAPI getRecvQueueDataSize() const { return m_pstRecvQueue->getDataSize(); }
	virtual int SLAPI getSendQueueDataSize() const { return m_pstSendQueue->getDataSize(); }
	virtual int SLAPI putData(const void* pszBuffer, int iLen){ return m_pstSendQueue->put((const char*)pszBuffer, iLen); }
	virtual const char* SLAPI getData(char* p, int readLen){ return m_pstRecvQueue->get(p, readLen); }
	virtual const char* SLAPI peekData(char *p, int peekLen){ return m_pstRecvQueue->get(p, peekLen, true); }
	virtual void SLAPI skip(const int len) { m_pstRecvQueue->readOut(len); }

private:
	SLShm				m_stShm;				 ///< 共享内存
	sl::SLRingBuffer*   m_pstRecvQueue;			 ///< 接收队列
	sl::SLRingBuffer*	m_pstSendQueue;			 ///< 发送队列
	bool				m_bBackEnd;				 ///< 是共享内存队列的前端还是后端
	char				m_szBuffer[256];
	int					m_shmSendSize;
	int					m_shmRecvSize;

}; // class CShmQueue

}



}// namespace sl
#endif