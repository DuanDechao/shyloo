//�����ڴ����
/********************************************************************
	created:	2015/12/05
	created:	5:12:2015   21:50
	filename: 	e:\myproject\shyloo\sllib\slshm_queue.h
	file path:	e:\myproject\shyloo\sllib
	file base:	slshm_queue
	file ext:	h
	author:		ddc
	
	purpose:	�����ڴ����
*********************************************************************/
#ifndef _SL_LIB_SHM_QUEUE_H_
#define _SL_LIB_SHM_QUEUE_H_

#include "slshm_create.h"
#include "slcode_queue.h"
#include "slshm.h"
namespace sl{
namespace shm{

//�����ڴ����
class SLShmQueue: public ISLShmQueue{
public:
	SLShmQueue(bool bBackEnd, const char* pszShmKey, int sendSize, int recvSize, bool clear);
	virtual ~SLShmQueue(){}

	//��ʼ��
	/*
	@param  [in] bBackEnd	�ǹ����ڴ���е�ǰ�˻��Ǻ��
	@param  [in] pszShmKey	�����ڴ��Key
	@param  [in] iShmSize	�����ڴ�Ĵ�С(�����ǹ����ڴ���ܴ�С��������еĴ�С��1/2)
	@return 0��ʾ�ɹ�
	*/
	int Init(bool bBackEnd, const char* pszShmKey, int sendSize, int recvSize, bool clear);

	virtual int SLAPI getRecvQueueFreeSize() const { return m_pstRecvQueue->GetFreeSize(); }
	virtual int SLAPI getSendQueueFreeSize() const { return m_pstSendQueue->GetFreeSize(); }
	virtual int SLAPI getRecvQueueDataSize() const { return m_pstRecvQueue->GetDataSize(); }
	virtual int SLAPI getSendQueueDataSize() const { return m_pstSendQueue->GetDataSize(); }
	virtual int SLAPI putData(const void* pszBuffer, int iLen){ return m_pstSendQueue->Put((const char*)pszBuffer, iLen); }
	virtual const char* SLAPI getData(char* p, int readLen){ return m_pstRecvQueue->Get(p, readLen); }
	virtual const char* SLAPI peekData(char *p, int peekLen){ return m_pstRecvQueue->Get(p, peekLen, true); }
	virtual void SLAPI skip(const int len) { m_pstRecvQueue->skip(len); }

private:
	SLShm				m_stShm;				 ///< �����ڴ�
	CCodeQueue*			m_pstRecvQueue;			 ///< ���ն���
	CCodeQueue*			m_pstSendQueue;			 ///< ���Ͷ���
	bool				m_bBackEnd;				 ///< �ǹ����ڴ���е�ǰ�˻��Ǻ��
	char				m_szBuffer[256];
	int					m_shmSendSize;
	int					m_shmRecvSize;

}; // class CShmQueue

}



}// namespace sl
#endif