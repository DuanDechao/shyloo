///监听套接字epoll对象类
/********************************************************************
	created:	2015/12/02
	created:	2:12:2015   16:44
	filename: 	e:\myproject\shyloo\netsvr\netlisten.h
	file path:	e:\myproject\shyloo\netsvr
	file base:	netlisten
	file ext:	h
	author:		ddc
	
	purpose:	监听套接字epoll对象类
*********************************************************************/
#ifndef _NET_LISTEN_H_
#define	_NET_LISTEN_H_

#include "netdef.h"

namespace sl
{
	class CNetCtrl;
	class CNetListen: public CEpollObject<CNetCtrl>
	{
	public:
		CNetListen()
			:m_stSocket(PF_INET, SOCK_STREAM, false)
		{}

		virtual ~CNetListen()
		{
			Unregister();
		}

		int Init(CNetCtrl& stOwner, const CNetListenInfo& stListenInfo);

		const CNetListenInfo& GetListenInfo() const
		{
			return m_stListenInfo;
		}
	protected:
		friend class CNetCtrl;

		CNetCtrl*			m_pstOwner;
		CSocket				m_stSocket;
		CNetListenInfo		m_stListenInfo;
		unsigned int		m_uiCreateTime;
		unsigned int		m_uiLastTime;
	};
}
#endif