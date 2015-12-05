///连接套接字Epoll对象类
/********************************************************************
	created:	2015/12/02
	created:	2:12:2015   16:09
	filename: 	e:\myproject\shyloo\netsvr\netclient.h
	file path:	e:\myproject\shyloo\netsvr
	file base:	netclient
	file ext:	h
	author:		ddc
	
	purpose:	连接套接字Epoll对象类
*********************************************************************/

#ifndef _NET_CLIENT_H_
#define _NET_CLIENT_H_
#include "netdef.h"

namespace sl
{
	class CNetCtrl;
	class CNetListen;
	class CNetClient: public CEpollObject<CNetCtrl>
	{
	protected:
		friend class CNetCtrl;
		friend class CNetClientFactory;

		CNetCtrl*			m_pstOwner;
		CNetListen*			m_pstListen;
		CSocket				m_stSocket;
		CNetHead			m_stNetHead;
		CBuffer				m_stRecvBuf;		///< 接收缓冲区
		CBuffer				m_stSendBuf;		///< 发送缓冲区
		int					m_iRecvBytes;		///< 接收的累积数据量
		int					m_iSendBytes;		///< 发送的累积数据量
		unsigned int		m_uiSeq;			///< 客户端序号，每个客户端连接分配一个
	
	public:
		CNetClient()
			:m_stSocket(PF_INET, SOCK_STREAM, false),
			 m_iRecvBytes(0),
			 m_iSendBytes(0)
			 {}

		virtual ~CNetClient()
		{
			Unregister();		///< 先Unregister从epoll注销，然后关闭socket
		}

		int Init(CNetCtrl& stOwner, CNetListen& stListen,
			SOCKET iSocket, unsigned int uiIP, unsigned int ushPort);
		
	}; //class CNetClient

} //namespace  sl

#endif