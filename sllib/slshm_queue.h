//共享内存队列
#ifndef _SL_SHM_QUEUE_H_
#define _SL_SHM_QUEUE_H_
#include "net/slepoll.h"
#include "ipc/slshm.h"
#include "slcode_queue.h"
#include "net/slsocket.h"
namespace sl
{
	//共享内存队列
	template<typename EpollOwner>
	class CShmQueue
	{
	protected:
		typedef CEpollObject<EpollOwner> COwnerEpollObject;

		CShm				m_stShm;				 ///< 共享内存
		CCodeQueue*			m_pstRecvQueue;			 ///< 我的接收队列
		CCodeQueue*			m_pstSendQueue;			 ///< 我的发送队列
		CSocket				m_stNotifySocket;		 ///< 向对方发送通知的SOCKET
		CSocket				m_stListenSocket;		 ///< 我接收通知的SOCKET
		COwnerEpollObject	m_stEpollObject;		 ///< 用于注册到Epoll
		bool				m_bBackEnd;				 ///< 我是共享内存队列的前端还是后端
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

		//初始化
		/*
			@param  [in] bBackEnd	是共享内存队列的前端还是后端
			@param  [in] pszShmKey	共享内存的Key
			@param  [in] iShmSize	共享内存的大小(这里是共享内存的总大小，单向队列的大小是1/2)
			@param	[in] pszFrontEndSocket	前端的SOCKET
			@param	[in] pszBackEndSocket	后端的SOCKET
			@return 0表示成功
		*/

		int Init(bool bBackEnd, const char* pszShmKey, int iShmSize,
			     const char* pszFrontEndSocket, const char* pszBackEndSocket,
				 CEpoll& stEpoll, EpollOwner& stOwner,
				 typename COwnerEpollObject::PF_EPOLL_EVENT pfEvent)
		{
			SL_ASSERT( !(!pszShmKey || iShmSize <= 0 || !pszFrontEndSocket || !pszBackEndSocket));

			m_bBackEnd = bBackEnd;

			int iRet = 0;

			//初始化前端共享内存
			iRet = m_stShm.Create(pszShmKey, iShmSize);
			if(iRet)
			{
				return iRet;
			}

			//初始化消息队列
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

			//初始化和前端通讯的SOCKET
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

			//注册到Epoll
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

		//向发送队列中放入一个code
		int PutOneCode(const char* pszBuffer, int iLen)
		{
			int iRet = 0;
			iRet = m_pstSendQueue->Put(pszBuffer, iLen);

			//发送Notify消息
			int iSendLen = 0;
			m_stNotifySocket.SendEx("1", 1, iSendLen);
			return iRet;
		}

		//把两块缓冲区合并成一个code放入队列中
		//这个函数的作用是为了减少一次内存拷贝
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

			//发送Notify消息
			int iSendLen = 0;
			m_stNotifySocket.SendEx("1", 1, iSendLen);
			return iRet;
		}

		/*
			从接收队列中获取一个Code
		*/
		int GetOneCode(char* p, int buflen, int& len)
		{
			return m_pstRecvQueue->Get(p, buflen, len);
		}

		/*
			删除接收队列中的下一个Code
		*/
		int Remove()
		{
			return m_pstRecvQueue->Remove();
		}

		//判断接收队列中有没有Code
		bool HasCode() const
		{
			return m_pstSendQueue->GetCodeLen() > 0;
		}

		/*
			清除socket事件，避免重复被调用
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
				//直到把所有数据都收完才出来
				if(iRecvLen == 0)
				{
					break;
				}
			}
		}


	}; // class CShmQueue


}// namespace sl
#endif