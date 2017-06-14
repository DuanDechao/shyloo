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
protected:
	SLShm				m_stShm;				 ///< �����ڴ�
	CCodeQueue*			m_pstRecvQueue;			 ///< ���ն���
	CCodeQueue*			m_pstSendQueue;			 ///< ���Ͷ���
	bool				m_bBackEnd;				 ///< �ǹ����ڴ���е�ǰ�˻��Ǻ��
	char				m_szBuffer[256];

public:
	SLShmQueue(bool bBackEnd, const char* pszShmKey, int iShmSize);
	virtual ~SLShmQueue(){}

	//��ʼ��
	/*
	@param  [in] bBackEnd	�ǹ����ڴ���е�ǰ�˻��Ǻ��
	@param  [in] pszShmKey	�����ڴ��Key
	@param  [in] iShmSize	�����ڴ�Ĵ�С(�����ǹ����ڴ���ܴ�С��������еĴ�С��1/2)
	@return 0��ʾ�ɹ�
	*/
	int Init(bool bBackEnd, const char* pszShmKey, int iShmSize);

	virtual int SLAPI getRecvQueueFreeSize() const{ return m_pstRecvQueue->GetFreeSize(); }
	virtual int SLAPI getSendQueueFreeSize() const{ return m_pstSendQueue->GetFreeSize(); }

	//���Ͷ����з���һ��code
	virtual int SLAPI putData(const void* pszBuffer, int iLen){
		return m_pstSendQueue->Put((const char*)pszBuffer, iLen);

		////����Notify��Ϣ
		//int iSendLen = 0;
		//m_stNotifySocket.SendEx("1", 1, iSendLen);
		//return iRet;
	}

	////�����黺�����ϲ���һ��code���������
	////���������������Ϊ�˼���һ���ڴ濽��
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

	//	////����Notify��Ϣ
	//	//int iSendLen = 0;
	//	//m_stNotifySocket.SendEx("1", 1, iSendLen);
	//	return iRet;
	//}

	/*
	�ӽ��ն����л�ȡһ��Code
	*/
	virtual int SLAPI getData(char* p, int buflen, int& len){ return m_pstRecvQueue->Get(p, buflen, len); }

	/*
	ɾ�����ն����е���һ��Code
	*/
	virtual int SLAPI remove(){ return m_pstRecvQueue->Remove(); }

	//�жϽ��ն�������û��Code
	virtual bool SLAPI hasCode() const{ return m_pstRecvQueue->GetCodeLen() > 0; }

	virtual bool SLAPI getDataLen() const { return m_pstRecvQueue->GetCodeLen(); }

}; // class CShmQueue

}



}// namespace sl
#endif