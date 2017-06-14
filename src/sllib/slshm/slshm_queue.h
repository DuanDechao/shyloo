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
#include "slcode_queue.h"
#include "slshm.h"
namespace sl{
namespace shm{

//共享内存队列
class SLShmQueue: public ISLShmQueue{
protected:
	SLShm				m_stShm;				 ///< 共享内存
	CCodeQueue*			m_pstRecvQueue;			 ///< 接收队列
	CCodeQueue*			m_pstSendQueue;			 ///< 发送队列
	bool				m_bBackEnd;				 ///< 是共享内存队列的前端还是后端
	char				m_szBuffer[256];

public:
	SLShmQueue(bool bBackEnd, const char* pszShmKey, int iShmSize);
	virtual ~SLShmQueue(){}

	//初始化
	/*
	@param  [in] bBackEnd	是共享内存队列的前端还是后端
	@param  [in] pszShmKey	共享内存的Key
	@param  [in] iShmSize	共享内存的大小(这里是共享内存的总大小，单向队列的大小是1/2)
	@return 0表示成功
	*/
	int Init(bool bBackEnd, const char* pszShmKey, int iShmSize);

	virtual int SLAPI getRecvQueueFreeSize() const{ return m_pstRecvQueue->GetFreeSize(); }
	virtual int SLAPI getSendQueueFreeSize() const{ return m_pstSendQueue->GetFreeSize(); }

	//向发送队列中放入一个code
	virtual int SLAPI putData(const void* pszBuffer, int iLen){
		return m_pstSendQueue->Put((const char*)pszBuffer, iLen);

		////发送Notify消息
		//int iSendLen = 0;
		//m_stNotifySocket.SendEx("1", 1, iSendLen);
		//return iRet;
	}

	////把两块缓冲区合并成一个code放入队列中
	////这个函数的作用是为了减少一次内存拷贝
	//int PutOneCode(const char* p1, int len1, const char* p2, int len2){
	//	int iRet = 0;
	//	if (p1 == NULL || len1 <= 0){
	//		iRet = m_pstSendQueue->Put(p2, len2);
	//	}
	//	else if (p2 == NULL || len2 <= 0){
	//		iRet = m_pstSendQueue->Put(p1, len1);
	//	}
	//	else{
	//		iRet = m_pstSendQueue->Put(p1, len1, p2, len2);
	//	}

	//	////发送Notify消息
	//	//int iSendLen = 0;
	//	//m_stNotifySocket.SendEx("1", 1, iSendLen);
	//	return iRet;
	//}

	/*
	从接收队列中获取一个Code
	*/
	virtual int SLAPI getData(char* p, int buflen, int& len){ return m_pstRecvQueue->Get(p, buflen, len); }

	/*
	删除接收队列中的下一个Code
	*/
	virtual int SLAPI remove(){ return m_pstRecvQueue->Remove(); }

	//判断接收队列中有没有Code
	virtual bool SLAPI hasCode() const{ return m_pstRecvQueue->GetCodeLen() > 0; }

	virtual bool SLAPI getDataLen() const { return m_pstRecvQueue->GetCodeLen(); }

}; // class CShmQueue

}



}// namespace sl
#endif