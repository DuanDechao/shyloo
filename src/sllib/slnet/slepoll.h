///Epoll��װ��
/********************************************************************
	created:	2015/11/27
	created:	27:11:2015   17:17
	filename: 	e:\myproject\shyloo\sllib\net\slepoll.h
	file path:	e:\myproject\shyloo\sllib\net
	file base:	slepoll
	file ext:	h
	author:		ddc
	
	purpose:	Epoll��װ��
*********************************************************************/
#ifndef _SL_EPOLL_H_
#define _SL_EPOLL_H_
#include "slsocket.h"
#include "slconfig.h"

#ifdef SL_OS_WINDOWS
	//Ϊ����windows�±���
	#define EPOLLIN		0x001
	#define EPOLLOUT	0x004
	#define EPOLLERR	0x008
	#define EPOLLHUP	0x010	
	#define EPOLLET	    (1<<31)
	
	#define EPOLL_CTL_ADD	1
	#define EPOLL_CTL_DEL	2
	#define EPOLL_CTL_MOD	3
	
	#define epoll_create(x) (0)
	#define epoll_ctl(epfd, op, fd, event) (0)
	#define epoll_wait(epfd, event, waitsize, timeout) (timeout)

	namespace sl
	{
		typedef union epoll_data
		{
			pointer ptr;
			int fd;
			uint u32;
			uint64 u64;
		} epoll_data_t;

		struct epoll_event
		{
			uint	events;		/*Epoll events*/
			epoll_data_t data;	/*User data variable*/
		};
	}
#else
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <arpa/inet.h>
	#include <sys/epoll.h>
#endif

namespace sl
{
	//Epoll������Ϣ��
	class CEpoll;
	class CEpollObjectInf
	{
		friend class CEpoll;
	protected:
		//��������������������Ϊ����Epoll��WaitAndEvent��У������Ƿ�Ϸ�
		CEpoll* m_pstEpoll;
		SOCKET	m_iSocket;

	public:
		CEpollObjectInf()
			:m_pstEpoll(NULL),
			 m_iSocket(SL_INVALID_SOCKET)
		{}
		virtual ~CEpollObjectInf() {}
	
	protected:
		virtual void OnEpollEvent(int iEvent) = 0;

	}; //CEpollObjectInf

	//Epoll��װ��
	class CEpoll
	{
	public:
		CEpoll()
			:m_kdpfd(0),
			 m_size(0),
			 m_iWaitSize(0),
			 m_astEvents(0)
		{}
		virtual ~CEpoll()
		{
			Exit();
		}
		typedef struct epoll_event TSocketEvent;
	
	public:
		//��ʼ��
		int Init(int iEpollSize, int iWaitSize)
		{
			m_size = iEpollSize;
			m_iWaitSize = iWaitSize;
			m_astEvents = new TSocketEvent[m_iWaitSize];
			if(!m_astEvents)
			{
				SL_ERROR("new TSocketEvent[%d] failed: %d", m_iWaitSize, SL_ERRNO);
				return -1;
			}

			m_kdpfd = epoll_create(m_size);
			if(m_kdpfd < 0)
			{
				SL_ERROR("epoll_create failed: %d", SL_ERRNO);
				return -2;
			}

			SL_TRACE("init epoll ok, epoll = %d", m_kdpfd);
			return 0;
		}

		/*
			�ȴ��¼�������ʱ
			@param [in] iTimeOut	�ȴ��ĳ�ʱʱ�ޣ���λ����
			@return <0 ��ʾ���� =0 ��ʾû��ʱ�� >0��ʾ�յ���������¼�����
		*/ 
		int Wait(int iTimeOut)
		{
			return epoll_wait(m_kdpfd, m_astEvents, m_iWaitSize, iTimeOut);
		}

		/*
			�ȴ��¼�������ʱ�������÷���
			@param [in]	iTimeOut	�ȴ��ĳ�ʱʱ�ޣ���λ����
			@return <0��ʾ���� =0��ʾû��ʱ�� >0��ʾ�յ���������¼�����
		*/
		int WaitAndEvent(int iTimeOut)
		{
			int iEventCount = Wait(iTimeOut);
			if(iEventCount < 0)
			{
				return iEventCount;
			}
			else if(iEventCount == 0)  ///< ��ʱ��
			{
				return 0;
			}

			//һ����ദ��1000�¼�
			for(int i = 0; i < iEventCount && i < 1000; ++i)
			{
				uint64 ullData = GetData(i); //fd

				//��64λϵͳ������ullDataֻ�ܴ��һ��ָ�룬���Դ˴����ڽ��и��ƺͺ����ļ�����
				//unsigned int uiSocket = SL_UINT64_HIGH(ullData)
#ifdef SL_BIT64
				CEpollObjectInf* pstObjectPos = (CEpollObjectInf*)ullData;
#else
				CEpollObjectInf* pstObjectPos = (CEpollObjectInf*)(SL_UINT64_LOW(ullData));
#endif
				uint uiEvent = GetEvent(i); //event

				//�ж϶����Ƿ�Ϸ�
				if(pstObjectPos == NULL || pstObjectPos->m_pstEpoll != this)
				{
					SL_WARNING("Epoll.Data(%p) invalid, event=%u", pstObjectPos, uiEvent);
					continue;
				}
				pstObjectPos->OnEpollEvent(uiEvent);
			}

			return iEventCount;
		}

		uint64 GetData(int i) const
		{
			SL_ASSERT(i < m_iWaitSize);
			return m_astEvents[i].data.u64;
		}

		uint GetEvent(int i) const
		{
			SL_ASSERT(i < m_iWaitSize);
			return m_astEvents[i].events;
		}

		static bool IsInputEvent(int iEvent)
		{
			return (iEvent & EPOLLIN) != 0;
		}

		static bool IsOutputEvent(int iEvent)
		{
			return (iEvent & EPOLLOUT) != 0;
		}

		static bool IsCloseEvent(int iEvent)
		{
			return (iEvent & (EPOLLHUP | EPOLLERR)) != 0;
		}

		int Add(SOCKET s, uint64 data, uint event)  
		{
			m_stEvent.events = event | EPOLLERR | EPOLLHUP;
			m_stEvent.data.u64 = data; 
			int iRet = epoll_ctl(m_kdpfd, EPOLL_CTL_ADD, s, &m_stEvent); //����ĳ��socket fd
			if(iRet)
			{
				SL_WARNING("epoll_ctl(add) failed: %d, epoll = %d, socket = %d", SL_ERRNO, m_kdpfd, s);
			}
			return iRet;
		}

		int Del(SOCKET s, uint64 data = 0, uint event = EPOLLIN)
		{
			m_stEvent.events = 0;
			m_stEvent.data.u64 = data;
			int iRet = epoll_ctl(m_kdpfd, EPOLL_CTL_DEL, s, &m_stEvent);
			if(iRet)
			{
				SL_WARNING("epoll_ctl(del) failed: %d, epoll = %d, socket = %d", SL_ERRNO, m_kdpfd, s);
			}
			return iRet;
		}

		int Mod(SOCKET s, uint64 data, int event)
		{
			m_stEvent.events = event | EPOLLERR | EPOLLHUP;
			m_stEvent.data.u64 = data;
			int iRet = epoll_ctl(m_kdpfd, EPOLL_CTL_MOD, s, &m_stEvent);
			if(iRet)
			{
				SL_WARNING("epoll_ctl(mod) failed: %d, epoll = %d, socket = %d", SL_ERRNO, m_kdpfd, s);
			}
			return iRet;
		}

		protected:
			void Exit()
			{
				if(m_astEvents)
				{
					delete [] m_astEvents;
					m_astEvents = 0;
				}
				if(m_kdpfd > 0)
				{
					close(m_kdpfd);
					m_kdpfd = 0;
				}
			}


	protected:
		int				m_kdpfd;
		int				m_size;
		int				m_iWaitSize;
		TSocketEvent*	m_astEvents;
		TSocketEvent	m_stEvent;
 
	};  //class CEpoll

	//Ҫʹ��Epoll����������̳�
	//��ģ��ķ�ʽ��ʵ������ί�еĹ���
	template<typename Owner>
	class CEpollObject: public CEpollObjectInf
	{
		friend class CEpoll;
	public:
		typedef void (Owner::*PF_EPOLL_EVENT) (CEpollObject* pstObject, SOCKET iSocket, int iEvent);
	protected:
		Owner*				m_pstOwner;
		PF_EPOLL_EVENT		m_pfEvent;
		unsigned int		m_iRegEvent;
	
	public:
		CEpollObject()
			:m_pstOwner(NULL),
			 m_pfEvent(NULL),
			 m_iRegEvent(0)
		{}
		virtual ~CEpollObject() {Unregister();}

		//ע�ᵽEpoll��
		int Register(Owner& stOwner, PF_EPOLL_EVENT pfEvent, CEpoll& stEpoll, SOCKET iSocket, unsigned int iRegEvent)
		{
			SL_ASSERT(iSocket != SL_INVALID_SOCKET && iRegEvent > 0 && pfEvent != NULL);
			
			int iRet = 0;
			iRet = Unregister();
			if(iRet)
			{
				return iRet;
			}

			m_pstEpoll	  =	  &stEpoll;
			m_pstOwner	  =   &stOwner;
			m_pfEvent	  =   pfEvent;
			m_iRegEvent	  =   iRegEvent;
			m_iSocket	  =   iSocket;

			uint64 ullData = CreateData(m_iSocket);
			iRet = m_pstEpoll->Add(m_iSocket, ullData, m_iRegEvent);
			
			return iRet;
		}

		//���Ĺ�ע���¼�
		int ModRegEvent(int iRegEvent)
		{
			m_iRegEvent = iRegEvent;
			if(m_pstEpoll)
			{
				uint64 ullData = CreateData(m_iSocket);
				return m_pstEpoll->Mod(m_iSocket, ullData, m_iRegEvent);
			}

			return 0;
		}

	protected:
		virtual void OnEpollEvent(int iEvent)
		{
			SL_ASSERT(m_pstOwner != NULL && m_pfEvent != NULL);
			(m_pstOwner->*m_pfEvent)(this, m_iSocket, iEvent);
		}

		int Unregister()
		{
			int iRet = 0;
			if(m_pstEpoll)
			{
				SL_TRACE("del %d from epoll", m_iSocket);
				iRet = m_pstEpoll->Del(m_iSocket);
				m_pstEpoll = NULL;
			}
			m_pstOwner  = NULL;
			m_pfEvent   = NULL;
			m_iRegEvent = 0;
			m_iSocket   = SL_INVALID_SOCKET;

			return iRet;
		}

		uint64 CreateData(SOCKET iSocket)
		{
#ifdef SL_BIT64
			return (uint64)this;
#else
			return SL_UINT64_MAKE(iSocket, (unsigned int) this);
#endif
		}

	}; // class CEpollObject


} //namespace sl

#endif