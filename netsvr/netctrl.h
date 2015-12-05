//net�ؼ���
/********************************************************************
	created:	2015/12/02
	created:	2:12:2015   16:30
	filename: 	e:\myproject\shyloo\netsvr\netctrl.h
	file path:	e:\myproject\shyloo\netsvr
	file base:	netctrl
	file ext:	h
	author:		ddc
	
	purpose:	net�ؼ���
*********************************************************************/

#ifndef _NET_CTRL_H_
#define _NET_CTRL_H_
#include "netdef.h"
#include "netclientfactory.h"
#include "netlisten.h"
#include "netconfig.h"
//===================================

#define CNetEpollObject CEpollObject<CNetCtrl>

namespace sl
{
	class CNetCtrl
	{
		enum
		{
			BUFF_INDEX	=	0,
			FREE_LIST	=	0,
			USED_LIST	=	1
		};

	public:
		friend class CNetListen;
		friend class CNetClient;

		CNetCtrl();

		//��ʼ��
		int Init();

		//��ѭ��
		int Run();

		int Exit();

		void OnShmQueueEvent(CNetEpollObject* pstObject, SOCKET iSocket, int iEvent);
		void OnClientEvent(CNetEpollObject* pstObject, SOCKET iSocket, int iEvent);
		void OnListenEvent(CNetEpollObject* pstObject, SOCKET iSocket, int iEvent);

	protected:
		void HandleShmQueueMsg();

		int AllocRecvBuffer(CNetClient* pstClient);
		int AllocSendBuffer(CNetClient* pstClient);
		int FreeRecvBuffer(CNetClient* pstClient);
		int FreeSendBuffer(CNetClient* pstClient);

		//�������
		int CheckIdle();

		//���Ͳ�������
		int CheckRemain();

		//��RecvBuf�е���Ϣ������ն���
		int PutOneMessage(CNetClient* pstClient, int iLen, const char* szMsgBuf, int iMsgLen);

		//���ϲ㷢��Notify
		int PutOneNotify(CNetClient* pstClient, unsigned short unLiveFlag);

		/*
			�ж�RecvBuf���Ƿ�����������Ϣ
			@param	[out]	iLen			��������Ϣ�ĳ���
			@param	[out]	pszMsgBuf		����Ϣ����Ч���ݵ���ʼλ��
			@param	[out]	iMsgLen			����Ϣ����Ч���ݵĳ���
			@param	[out]	bBreak			

		*/
		int BufferHasMessage(CNetClient* pstClient, int& iLen, char*& pszMsgBuf, int& iMsgLen, bool& bBreak);

		int RecvSocket(CNetClient* pstClient);
		int SendSocket(CNetClient* pstClient);
		int CloseSocket(CNetClient* pstClient, unsigned short unLiveFlag);

		int AfterRecvSocket(CNetClient* pstClient);
		int AfterSendSocket(CNetClient* pstClient);

		static void sigusr1_handle(int iSigVal);
		static void sigusr2_handle(int iSigVal);


	private:
		CBuffer					m_stTempBuf;			///< ��ʱ������
		CEpoll					m_stEpoll;
		CShmQueue<CNetCtrl>		m_stShmQueue;

		CShm					m_stHandleMgrShm;
		CNetClientFactory		m_stClientFactory;
		CObjectMgr<char*>		m_stRecvBufMgr;
		CObjectMgr<char*>		m_stSendBufMgr;

		///�����˿�
		CArray<CNetListen, NET_MAX_LISTEN>		m_astListen;

		
		static bool				s_bReloadConfig;
		static bool				s_bExit;

		unsigned int			m_uiNow;
	};
}
#endif