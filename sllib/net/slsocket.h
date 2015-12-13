//SOCKET封装类
/********************************************************************
	created:	2015/12/01
	created:	1:12:2015   17:18
	filename: 	e:\myproject\shyloo\sllib\net\slsocket.h
	file path:	e:\myproject\shyloo\sllib\net
	file base:	slsocket
	file ext:	h
	author:		ddc
	
	purpose:	SOCKET封装类
*********************************************************************/
#ifndef _SL_SOCKET_H_
#define _SL_SOCKET_H_
#include "../slbase.h"
#include "slsocket_utils.h"
#include "../slsize_string.h"
#ifndef SL_OS_WINDOWS 
#include "stddef.h"
#endif

namespace sl
{
	class CSocket
	{
	private:
		int				m_iAF;			///< Address family
		int				m_iType;		///< TCP/UDP/RAW
		bool			m_bListen;		///< 是监听还是主动连接
		bool			m_bUseSelect;	///< 是在函数内调用select来控制超时，还是有外部epoll处理

		SOCKET			m_iSocket;
		char			m_szAddr[128];
		socklen_t		m_iAddrLen;
		int				m_iConnTimeOut;
		int				m_iSendTimeOut;
		int				m_iRecvTimeOut;
	
	public:
		enum
		{
			SLLIB_SOCK_TIMEOUT	=	5000,		//5s	
		};

	public:
		//超时时间的单位是毫秒
		CSocket(int iAF = PF_INET,
				int iType = SOCK_STREAM,
				bool bUseSelect = true,				///< 默认使用select控制超时
				int iConnTimeOut = SLLIB_SOCK_TIMEOUT,
				int iSendTimeOut = SLLIB_SOCK_TIMEOUT,
				int iRecvTimeOut = SLLIB_SOCK_TIMEOUT)
			:m_iAF(iAF),
			 m_iType(iType),
			 m_bListen(false),
			 m_bUseSelect(bUseSelect),
			 m_iSocket(SL_INVALID_SOCKET),
			 m_iAddrLen(0),
			 m_iConnTimeOut(iConnTimeOut),
			 m_iSendTimeOut(iSendTimeOut),
			 m_iRecvTimeOut(iRecvTimeOut)
		{
			memset(m_szAddr, 0, sizeof(m_szAddr));
		}
		virtual ~CSocket()
		{
			Close();
		}
	public:
		bool IsTCP() const {return m_iType == SOCK_STREAM;}
		bool IsValid() const {return (m_iSocket != SL_INVALID_SOCKET);}
		bool IsListen() const {return m_bListen;}
		int GetAF() const {return m_iAF;}
		int GetType() const {return m_iType;}
		bool IsUseSelect() const {return m_bUseSelect;}
		SOCKET GetSocket() const {return m_iSocket;}
		void SetSocket(SOCKET s) {m_iSocket = s;}


	public:
		//创建socket
		int Create(bool bIsBlock = false)
		{
			if(IsValid())
			{
				Close();
			}
			m_iSocket = socket(m_iAF, m_iType, 0);
			if(!IsValid())
			{
				return SL_WSA_ERRNO;
			}

			//设为非阻塞模式
			if(!bIsBlock)
			{
				int iRet = CSocketUtils::NonblockSocket(m_iSocket);
				if(iRet)
				{
					Close();
					return SL_WSA_ERRNO;
				}
			}

			return 0;
		}

		//连接
		int Connect(const sockaddr* pstAddr, socklen_t iAddrLen)
		{
			SL_ASSERT(iAddrLen <= sizeof(m_szAddr));
			memcpy(m_szAddr, pstAddr, iAddrLen);
			m_iAddrLen = iAddrLen;

			int iRet = 0;
			if(!IsValid()) //如果没有创建socket
			{
				iRet = Create();
				if(iRet)
				{
					return iRet;
				}
			}

			if(!IsTCP())
			{
				//UDP不用connect
				return 0;
			}
			
			//连接
			iRet = connect(m_iSocket, pstAddr, iAddrLen);
			if(iRet < 0)
			{
				int iErrNo = SL_WSA_ERRNO;
				if(iErrNo != SL_EINPROGRESS && iErrNo != SL_EWOULDBLOCK)
				{
					Close();
					return iErrNo;
				}
			}

			if(IsUseSelect())
			{
				fd_set bsReadSet;
				fd_set bsWriteSet;
				while(1)
				{
					FD_ZERO(&bsReadSet);
					FD_ZERO(&bsWriteSet);
					FD_SET(m_iSocket, &bsReadSet);
					FD_SET(m_iSocket, &bsWriteSet);
					int iMaxFD = static_cast<int>(m_iSocket) + 1;

					struct timeval tv;
					struct timeval *pTv;
					if(0 >= m_iConnTimeOut)
					{
						pTv = NULL;
					}
					else
					{
						tv.tv_sec	=	m_iConnTimeOut / 1000;
						tv.tv_usec	=	(m_iConnTimeOut % 1000) * 1000;
						pTv = &tv;
					}
					iRet = select(iMaxFD, &bsReadSet, &bsWriteSet, NULL, pTv);
					if(SL_WSA_ERRNO == EINTR)
					{
						continue;
					}
					else if(iRet == 0)
					{
						//说明此时超时，就返回超时
						return SL_ETIME;
					}
					else if(0 >iRet)
					{
						iRet = SL_WSA_ERRNO;
						break;
					}
					if(FD_ISSET(m_iSocket, &bsReadSet) && FD_ISSET(m_iSocket, &bsWriteSet))
					{
						int iErr = 0;
#ifndef SL_OS_WINDOWS
						socklen_t iLen = sizeof(iErr);
						int iSockOptRet = getsockopt(m_iSocket, SOL_SOCKET, SO_ERROR, &iErr, &iLen);
#else
						int iSockOptRet = 0;
#endif
						if(iSockOptRet < 0 || (iErr != 0 && iSockOptRet == 0) )
						{
							iRet = ECONNREFUSED;
						}
						else
						{
							iRet = 0;
						}
						break;
					}
					else if(FD_ISSET(m_iSocket, &bsWriteSet))
					{
						iRet = 0;
						break;
					}
				}
				if(iRet)
				{
					Close();
					return iRet;
				}
			}
			return 0;
		}

		//PF_INET连接
		int Connect(const char* pszIPAddr, unsigned short ushPort)
		{
			if(!pszIPAddr)
			{
				return -1;
			}

			sockaddr_in stTempAddr;
			memset((void *)&stTempAddr, 0, sizeof(sockaddr_in));

			stTempAddr.sin_family = PF_INET;
			stTempAddr.sin_port = htons(ushPort);
			stTempAddr.sin_addr.s_addr = inet_addr(pszIPAddr);
			if(stTempAddr.sin_addr.s_addr == INADDR_NONE)
			{
				return -2;
			}
			return Connect((struct sockaddr*)&stTempAddr, sizeof(stTempAddr));
		}

		//PF_LOCAL连接
#ifndef SL_OS_WINDOWS
		int Connect(const char* pszPathName)
		{
			if(!pszPathName)
			{
				return -1;
			}
			struct sockaddr_un stTempAddr;
			memset((void*)&stTempAddr, 0, sizeof(stTempAddr));

			stTempAddr.sun_family = PF_LOCAL;
			snprintf(SL_STRSIZE(stTempAddr.sun_path), "%s", pszPathName);

			return Connect( (struct sockaddr*)&stTempAddr, sizeof(stTempAddr));
		}
#else
		int Connect(const char* SL_UNUSED(pszPathName))
		{
			return 0;
		}
#endif

		//监听
		int Listen(const struct sockaddr* pstAddr, socklen_t iAddrLen)
		{
			SL_ASSERT(iAddrLen <= sizeof(m_szAddr));
			memcpy(m_szAddr, pstAddr, iAddrLen);
			m_iAddrLen = iAddrLen;

			m_bListen = true;

			int iRet = 0;
			if(!IsValid())  //如果没有创建socket
			{
				iRet = Create();
				if(iRet)
				{
					return iRet;
				}
			}

			//bind
			iRet = 1;
			setsockopt(m_iSocket, SOL_SOCKET, SO_REUSEADDR, (const char*)&iRet, sizeof(iRet));

			//只有TCP才要这样
			if(IsTCP())
			{
				setsockopt(m_iSocket, SOL_SOCKET, SO_KEEPALIVE, (const char*)&iRet, sizeof(iRet));
				struct linger ling = {0, 0};
				setsockopt(m_iSocket, SOL_SOCKET, SO_LINGER, (const char*)&ling, sizeof(ling));
#ifndef SL_OS_WINDOWS

#endif
			}

			if(bind(m_iSocket, pstAddr, iAddrLen) < 0)
			{
				Close();
				return SL_WSA_ERRNO;
			}

			//listen
			if(IsTCP())
			{
				if(listen(m_iSocket, 1024) < 0)
				{
					Close();
					return SL_WSA_ERRNO;
				}
			}
			return 0;
		}


		//PF_INET监听
		int Listen(const char*pszIPAddr, unsigned short ushPort)
		{
			if(!pszIPAddr)
			{
				return -1;
			}
			sockaddr_in stTempAddr;
			memset((void*)&stTempAddr, 0, sizeof(sockaddr_in));

			stTempAddr.sin_family = PF_INET;
			stTempAddr.sin_port = htons(ushPort);
			stTempAddr.sin_addr.s_addr = inet_addr(pszIPAddr);
			if(stTempAddr.sin_addr.s_addr == INADDR_NONE)
			{
				return -2;
			}

			return Listen((struct sockaddr*)&stTempAddr, sizeof(stTempAddr));
		}

		//PF_LOCAL监听
#ifndef SL_OS_WINDOWS
		int Listen(const char* pszPathName)
		{
			if(!pszPathName)
			{
				return -1;
			}
			struct sockaddr_un stTempAddr;
			memset((void*)&stTempAddr, 0, sizeof(stTempAddr));
			stTempAddr.sun_family = AF_LOCAL;
			strncpy(stTempAddr.sun_path, pszPathName, sizeof(stTempAddr.sun_path)-1);
			
			unlink(pszPathName);
			return Listen((struct sockaddr*)&stTempAddr, sizeof(stTempAddr));
		}
#else
		int Listen(const char* SL_UNUSED(pszPathName))
		{
			return 0;
		}
#endif

		//接收连接
		int Accept(SOCKET& iAcceptSocket, sockaddr* pstAddr = NULL, socklen_t* pAddrLen = NULL)
		{
			if(!IsListen())
			{
				return -1;
			}

			int iRet = 0;
			if(IsUseSelect())
			{
				iRet = CSocketUtils::Select(m_iSocket, true, m_iConnTimeOut);
				if(iRet)
				{
					return iRet;
				}
			}
			iAcceptSocket = accept(m_iSocket, pstAddr, pAddrLen);
			if(iAcceptSocket == SL_INVALID_SOCKET)
			{
				return SL_WSA_ERRNO;
			}

			iRet = CSocketUtils::NonblockSocket(iAcceptSocket);
			if(iRet)
			{
				closesocket(iAcceptSocket);
				return SL_WSA_ERRNO;
			}
			return 0;
			
		}

		/*
			发送数据，直到所有数据发送完或出错才返回
			@param [out] sendlen 已发送的数据的长度
			@return 0=发送完成并成功
		*/
		int Send(const char* buf, const int bufLen, int& sendlen)
		{
			int iRet = 0;
			sendlen  = 0;
			
			while(sendlen < bufLen)
			{
				if(IsUseSelect())
				{
					iRet = CSocketUtils::Select(m_iSocket, false, m_iSendTimeOut);
					if(iRet)
					{
						return iRet;
					}
				}

				if(IsTCP())
				{
					iRet = send(m_iSocket, buf + sendlen, bufLen - sendlen, 0);
				}
				else
				{
					iRet = sendto(m_iSocket, buf, bufLen, 0, (const struct sockaddr*)m_szAddr, m_iAddrLen);
				}

				if(iRet > 0)
				{
					sendlen += iRet;
				}
				else
				{
					int iErrNo = SL_WSA_ERRNO;
					if(iErrNo == EINTR)
					{
						continue;
					}
					else if(iErrNo == SL_EINPROGRESS || iErrNo == SL_EWOULDBLOCK)
					{
						continue;
					}
					else
					{
						return iErrNo;
					}
				}
			}

			return 0;
		}

		/*
			与Send()不同的是在SL_EINPROGRESS SL_EWOULDBLOCK的处理
		*/
		int SendEx(const char* buf, const int buflen, int& sendlen)
		{
			int iRet = 0;
			sendlen = 0;

			while (sendlen < buflen)
			{
				if (IsUseSelect())
				{
					iRet = CSocketUtils::Select(m_iSocket, false, m_iSendTimeOut);
					if (iRet)
					{
						return iRet;
					}
				}

				if (IsTCP())
				{
					iRet = send(m_iSocket, buf + sendlen, buflen - sendlen, 0);
				}
				else
				{
					iRet = sendto(m_iSocket, buf, buflen, 0,
						(const struct sockaddr*)m_szAddr, m_iAddrLen);
				}

				if (iRet > 0)
				{
					sendlen += iRet;
				}
				else
				{
					int iErrNo = SL_WSA_ERRNO;
					if (iErrNo == EINTR)
					{
						continue;
					}
					else if (iErrNo == SL_EINPROGRESS || iErrNo == SL_EWOULDBLOCK)
					{
						return iErrNo;
					}
					else
					{
						return iErrNo;
					}
				}
			}

			return 0;
		}

		//接收数据
		int Recv(char* buf, const int buflen, int& recvlen)
		{
			int iRet =  0;
			recvlen = 0;

			if(IsUseSelect())
			{
				iRet = CSocketUtils::Select(m_iSocket, true, m_iRecvTimeOut);
				if(iRet)
				{
					if(iRet != SL_ETIME)
					{

					}
					return iRet;
				}
			}

			while(recvlen < buflen)
			{
				if(IsTCP())
				{
					iRet = recv(m_iSocket, buf + recvlen, buflen - recvlen, 0);
				}
				else
				{
					iRet = recvfrom(m_iSocket, buf + recvlen, buflen - recvlen, 0, 0, 0);
				}

				if(iRet > 0)
				{
					recvlen += iRet;
				}
				else if(iRet == 0)
				{
					return SL_ECONNRESET;
				}
				else
				{
					int iErrNo = SL_WSA_ERRNO;
					if(iErrNo == EINTR)
					{
						continue;
					}
					else if(iErrNo == SL_EINPROGRESS || iErrNo == SL_EWOULDBLOCK)
					{
						return 0;
					}
					else
					{
						return iErrNo;
					}
				}
			}
			return 0;
		}

		void Close()
		{
			if(IsValid())
			{
				closesocket(m_iSocket);
				m_iSocket	=	SL_INVALID_SOCKET;
#ifndef SL_OS_WINDOWS
				if(GetAF() == PF_LOCAL && IsListen())
				{
					struct sockaddr_un* pstTempAddr = (struct sockaddr_un*)m_szAddr;
					unlink(pstTempAddr->sun_path);
				}
#endif
			}
		}

	};//class CSocket

} // namespace sl
#endif