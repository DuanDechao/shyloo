///�����׽���Epoll������
/********************************************************************
	created:	2015/12/02
	created:	2:12:2015   16:09
	filename: 	e:\myproject\shyloo\netsvr\netclient.h
	file path:	e:\myproject\shyloo\netsvr
	file base:	netclient
	file ext:	h
	author:		ddc
	
	purpose:	�����׽���Epoll������
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
		CBuffer				m_stRecvBuf;		///< ���ջ�����
		CBuffer				m_stSendBuf;		///< ���ͻ�����
		int					m_iRecvBytes;		///< ���յ��ۻ�������
		int					m_iSendBytes;		///< ���͵��ۻ�������
		unsigned int		m_uiSeq;			///< �ͻ�����ţ�ÿ���ͻ������ӷ���һ��
	
	public:
		CNetClient()
			:m_stSocket(PF_INET, SOCK_STREAM, false),
			 m_iRecvBytes(0),
			 m_iSendBytes(0)
			 {}

		virtual ~CNetClient()
		{
			Unregister();		///< ��Unregister��epollע����Ȼ��ر�socket
		}

		int Init(CNetCtrl& stOwner, CNetListen& stListen,
			SOCKET iSocket, unsigned int uiIP, unsigned int ushPort);
		
	}; //class CNetClient

} //namespace  sl

#endif