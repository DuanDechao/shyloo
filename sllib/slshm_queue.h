//�����ڴ����
#ifndef _SL_SHM_QUEUE_H_
#define _SL_SHM_QUEUE_H_
#include "net/slepoll.h"
#include "ipc/slshm.h"
#include "slcode_queue.h"
#include "net/slsocket.h"
namespace sl
{
	//�����ڴ����
	template<typename EpollOwner>
	class CShmQueue
	{
	protected:
		typedef CEpollObject<EpollOwner> COwnerEpollObject;

		CShm				m_stShm;				 ///< �����ڴ�
		CCodeQueue*			m_pstRecvQueue;			 ///< �ҵĽ��ն���
		CCodeQueue*			m_pstSendQueue;			 ///< �ҵķ��Ͷ���
		CSocket				m_stNotifySocket;		 ///< ��Է�����֪ͨ��SOCKET
		CSocket				m_stListenSocket;		 ///< �ҽ���֪ͨ��SOCKET
		COwnerEpollObject	m_stEpollObject;		 ///< ����ע�ᵽEpoll
		bool				m_bBackEnd;				 ///< ���ǹ����ڴ���е�ǰ�˻��Ǻ��
		char				m_szBuffer[256];

	public:
		CShmQueue()
			:m_pstRecvQueue(NULL),
			 m_pstSendQueue(NULL),
			 m_stNotifySocket(PF_INET, SOCK_DGRAM, false),
			 m_stListenSocket(PF_INET, SOCK_DGRAM, false),
			 m_bBackEnd(false)
		{}

		virtual ~CShmQueue(){}

		//��ʼ��
		/*
			@param  [in] bBackEnd	�ǹ����ڴ���е�ǰ�˻��Ǻ��
			@param  [in] pszShmKey	�����ڴ��Key
			@param  [in] iShmSize	�����ڴ�Ĵ�С(�����ǹ����ڴ���ܴ�С��������еĴ�С��1/2)
			@param	[in] pszFrontEndSocket	ǰ�˵�SOCKET
			@param	[in] pszBackEndSocket	��˵�SOCKET
			@return 0��ʾ�ɹ�
		*/

		int Init(bool bBackEnd, const char* pszShmKey, int iShmSize,
			     const char* pszFrontEndSocket, const char* pszBackEndSocket,
				 CEpoll& stEpoll, EpollOwner& stOwner,
				 typename COwnerEpollObject::PF_EPOLL_EVENT pfEvent)
		{
			SL_ASSERT( !(!pszShmKey || iShmSize <= 0 || !pszFrontEndSocket || !pszBackEndSocket));

			m_bBackEnd = bBackEnd;

			int iRet = 0;

			//��ʼ��ǰ�˹����ڴ�
			iRet = m_stShm.Create(pszShmKey, iShmSize);
			if(iRet)
			{
				return iRet;
			}

			//��ʼ����Ϣ����
			const int iQueueSize = m_stShm.GetSize() / 2;
			m_pstRecvQueue = (CCodeQueue*)(m_stShm.GetBuffer() + (m_bBackEnd ? 0 : iQueueSize));
			if(m_pstRecvQueue->CheckSum())
			{
				iRet = m_pstRecvQueue->Init(iQueueSize);
				if(iRet)
				{
					return iRet;
				}
				
			}

			m_pstSendQueue = (CCodeQueue*)(m_stShm.GetBuffer() + (m_bBackEnd ? iQueueSize : 0));
			if(m_pstSendQueue->CheckSum())
			{
				iRet = m_pstSendQueue->Init(iQueueSize);
				if(iRet)
				{
					return iRet;
				}
			}

			//��ʼ����ǰ��ͨѶ��SOCKET
			const char* pszNotifySocket = m_bBackEnd ? pszFrontEndSocket : pszBackEndSocket;
			const char* pszListenSocket = m_bBackEnd ? pszBackEndSocket : pszFrontEndSocket;

			iRet = m_stNotifySocket.Connect(pszNotifySocket);
			if(iRet)
			{
				return iRet;
			}

			iRet = m_stListenSocket.Listen(pszListenSocket);
			if(iRet)
			{
				return iRet;
			}

			//ע�ᵽEpoll
			iRet = m_stEpollObject.Register(stOwner, pfEvent, stEpoll, m_stListenSocket.GetSocket(), EPOLLIN);
			return iRet;
		}

		bool IsMyEpollObj(COwnerEpollObject* pstEpollObj)
		{
			return &m_stEpollObject == pstEpollObj;
		}

		int GetRecvQueueFreeSize() const
		{
			return m_pstRecvQueue->GetFreeSize();
		}

		int GetSendQueueFreeSize() const
		{
			return m_pstSendQueue->GetFreeSize();
		}

		//���Ͷ����з���һ��code
		int PutOneCode(const char* pszBuffer, int iLen)
		{
			int iRet = 0;
			iRet = m_pstSendQueue->Put(pszBuffer, iLen);

			//����Notify��Ϣ
			int iSendLen = 0;
			m_stNotifySocket.SendEx("1", 1, iSendLen);
			return iRet;
		}

		//�����黺�����ϲ���һ��code���������
		//���������������Ϊ�˼���һ���ڴ濽��
		int PutOneCode(const char* p1, int len1, const char* p2, int len2)
		{
			int iRet = 0;
			if(p1 == NULL || len1 <= 0)
			{
				iRet = m_pstSendQueue->Put(p2, len2);
			}
			else if(p2 == NULL || len2 <= 0)
			{
				iRet = m_pstSendQueue->Put(p1, len1);
			}
			else
			{
				iRet = m_pstSendQueue->Put(p1, len1, p2, len2);
			}

			//����Notify��Ϣ
			int iSendLen = 0;
			m_stNotifySocket.SendEx("1", 1, iSendLen);
			return iRet;
		}

		/*
			�ӽ��ն����л�ȡһ��Code
		*/
		int GetOneCode(char* p, int buflen, int& len)
		{
			return m_pstRecvQueue->Get(p, buflen, len);
		}

		/*
			ɾ�����ն����е���һ��Code
		*/
		int Remove()
		{
			return m_pstRecvQueue->Remove();
		}

		//�жϽ��ն�������û��Code
		bool HasCode() const
		{
			return m_pstSendQueue->GetCodeLen() > 0;
		}

		/*
			���socket�¼��������ظ�������
		*/
		void CleaeNotify()
		{
			int iRet = 0;
			int iRecvLen = 0;
			while(true)
			{
				iRet = m_stListenSocket.Recv(m_szBuffer, sizeof(m_szBuffer), iRecvLen);
				if(iRet)
				{
					if(iRet != SL_EWOULDBLOCK && iRet != SL_EINPROGRESS)
					{

					}
					return;
				}
				//ֱ�����������ݶ�����ų���
				if(iRecvLen == 0)
				{
					break;
				}
			}
		}


	}; // class CShmQueue


}// namespace sl
#endif