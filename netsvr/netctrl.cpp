/********************************************************************
	created:	2015/12/02
	created:	2:12:2015   20:48
	filename: 	e:\myproject\shyloo\netsvr\netctrl.cpp
	file path:	e:\myproject\shyloo\netsvr
	file base:	netctrl
	file ext:	cpp
	author:		ddc
	
	purpose:	
*********************************************************************/

#include "netctrl.h"
#include <string.h>
using namespace sl;

bool CNetCtrl::s_bReloadConfig	=	false;
bool CNetCtrl::s_bExit			=	false;

CNetCtrl::CNetCtrl()
{
	m_uiNow	=	static_cast<unsigned int>(time(0));
}

void CNetCtrl::sigusr1_handle(int SL_UNUSED(iSigVal))
{
	s_bReloadConfig = true;
	signal(SIGUSR1, CNetCtrl::sigusr1_handle);
}

void CNetCtrl::sigusr2_handle(int SL_UNUSED(iSigVal))
{
	s_bExit = true;
	signal(SIGUSR2, CNetCtrl::sigusr2_handle);
}

int CNetCtrl::Init()
{
	int iRet = 0;

	//��װ�źŴ�����
	signal(SIGUSR1, CNetCtrl::sigusr1_handle);			///< kill -USR1 pid ���ó����ض�����
	signal(SIGUSR2, CNetCtrl::sigusr2_handle);			///< kill -USR2 pid ���ó���ȫ���˳�

	//��ȡ����
	CONF->LoadConfig();

	//����Epoll
	iRet = m_stEpoll.Init(CONF->SocketMaxCount, NET_WAIT_SIZE);

	///���������ڴ����
	SL_INFO("Init NetShm Key = %s", CONF->FrontEndShmKey());
	iRet = m_stShmQueue.Init(false,
		CONF->FrontEndShmKey(), CONF->FrontEndShmSize,
		CONF->FrontEndSocket(), CONF->BackEndSocket(),
		m_stEpoll,
		*this,
		&CNetCtrl::OnShmQueueEvent);
	CHECK_RETURN(iRet);

	///���������ڴ�
	const size_t iClientSize	=	m_stClientFactory.CountSize(CONF->SocketMaxCount);
	const size_t iRecvBufSize	=	m_stRecvBufMgr.CountSize(CONF->BufferCount, CONF->RecvBufferSize);
	const size_t iSendBufSize	=	m_stSendBufMgr.CountSize(CONF->BufferCount, CONF->SendBufferSize);
	const size_t iShmSize		=	iClientSize	+ iRecvBufSize + iSendBufSize;
	iRet = m_stHandleMgrShm.Create(CONF->MgrShmKey(), iShmSize);
	CHECK_RETURN(iRet);

	memset((void*)m_stHandleMgrShm.GetBuffer(), 0, m_stHandleMgrShm.GetSize());
	SL_INFO("create buffer shm's shm at %p ok. key=%s size=%d clientsize=%d recvbufsize=%d sendbufsize=%d",
		m_stHandleMgrShm.GetBuffer(), CONF->MgrShmKey(), iShmSize, iClientSize, iRecvBufSize, iSendBufSize);
	//��ʼ������
	iRet = m_stClientFactory.Init(m_stHandleMgrShm.GetBuffer(),
		iClientSize, CONF->SocketMaxCount);
	CHECK_RETURN(iRet);

	///��ʼ�����ջ�����
	iRet = m_stRecvBufMgr.Init(m_stHandleMgrShm.GetBuffer() + iClientSize,
		CONF->BufferCount, CONF->RecvBufferSize);
	CHECK_RETURN(iRet);

	///��ʼ�����ͻ�����
	iRet = m_stSendBufMgr.Init(m_stHandleMgrShm.GetBuffer() + iClientSize + iRecvBufSize,
		CONF->BufferCount, CONF->SendBufferSize);
	CHECK_RETURN(iRet);
	
	
	//�����˿�
	SL_INFO("NET init: start listening");
	m_astListen.m_iUsedCount = CONF->ListenArray.m_iUsedCount;
	for (int i = 0; i < CONF->ListenArray.m_iUsedCount; ++i)
	{
		iRet = m_astListen[i].Init(*this, CONF->ListenArray[i]);
		SL_INFO("Listen:%s port:%d", CONF->ListenArray[i].m_szListenIP(), CONF->ListenArray[i].m_unListenPort);
		CHECK_RETURN(iRet);
	}

	///��ʱ������
	iRet = m_stTempBuf.Attach(NULL, CONF->SendBufferSize + sizeof(CNetHead), 0);
	CHECK_RETURN(iRet);


	SL_INFO("NET init ok");

	return 0;

}

int CNetCtrl::Run()
{
	int iRet = 0;
	int iLoop = 0;

	m_uiNow = static_cast<unsigned int>(time(0));
	int iPrevNow = m_uiNow;
	while(true)
	{
		///�źż��
		if(s_bReloadConfig)
		{
			s_bReloadConfig = false;
			//CONF->ReloadConfig();
		}

		if(s_bExit)
		{
			s_bExit = false;
			return 0;
		}
		
		iRet = m_stEpoll.WaitAndEvent(10);
		if(iRet == 0)
		{

		}

		// ����δ���͵����ݵļ��

		//�ӷ��Ͷ�����ȡ��Ϣ������
		HandleShmQueueMsg();

		if(++iLoop >= 50)
		{
			iLoop = 0;
			CheckIdle();
			CheckRemain();
		}
	}
	return 0;
}

int CNetCtrl::Exit()
{
	SL_INFO("%s exit", );
	return 0;
}

int CNetCtrl::CheckIdle()
{
	int iIndex,iNewIdx;
	for (iIndex = m_stClientFactory.Head(USED_LIST, ALLOC_INDEX);
		iIndex >= 0;
		iIndex = iNewIdx)
	{
		iNewIdx = m_stClientFactory.Next(iIndex, ALLOC_INDEX);
		CNetClient* pstClient = m_stClientFactory.CalcObject(iIndex);
		SL_ASSERT(pstClient);

		if(!pstClient->m_stSocket.IsValid())
		{
			SL_WARNING("CheckIdle handle(%d) socket invalid", iIndex);
			continue;
		}

		if((m_uiNow - pstClient->m_stNetHead.m_LastTime) >
			pstClient->m_pstListen->m_stListenInfo.m_uiIdleTimeOut)
		{
			SL_TRACE("CheckIdle handle(%d) socket(%d) close",
				iIndex, pstClient->m_stSocket.GetSocket());
			CloseSocket(pstClient, ENF_NET_FLAG_IDLECLOSE);
		}
	}
}

void CNetCtrl::OnShmQueueEvent(CNetEpollObject* pstObject, SOCKET iSocket, int iEvent)
{
	SL_TRACE("OnShmQueueEvent socket=%d event=%d", iSocket, iEvent);
	m_stShmQueue.CleaeNotify();   ///< ����Notify���ݣ������һֱ�յ��¼�

	if(!CEpoll::IsInputEvent(iEvent))
	{
		SL_WARNING("ShmQueue recv not match event(%d) socket=%d",iEvent, iSocket);
		return;
	}
	HandleShmQueueMsg();
}


void CNetCtrl::HandleShmQueueMsg()
{
	///���䷢�ͻ�����
	///��������
	///������ͻ�����Ϊ�վ��ͷ�

	int iRet = 0;
	for (;;)
	{
		if(!m_stShmQueue.HasCode())
		{
			return;
		}

		//������Ϣ
		int iCodeLen = 0;

		//�����α�
		m_stTempBuf.Clear();
		iRet = m_stShmQueue.GetOneCode(m_stTempBuf.GetFreeBuf(), m_stTempBuf.GetFreeLen(), iCodeLen);
		if(iRet || iCodeLen < (int) sizeof(CNetHead))
		{
			SL_WARNING("get code from shmqueue failed ret=%d, codelen=%d", iRet, iCodeLen);
			continue;
		}

		//�ƶ��α굽ָ��λ��
		m_stTempBuf.Append(iCodeLen);

		//�Ȼ�ȡNetHead��ַ
		CNetHead& stHead = *(CNetHead*)m_stTempBuf.GetUsedBuf();
		if(iCodeLen != (int)sizeof(CNetHead) + stHead.m_iDataLength)  /// ���Ȳ�һ��
		{
			SL_WARNING("code length invalid. %d != %d + %d", iCodeLen, sizeof(CNetHead), stHead.m_iDataLength);
			continue;
		}

		///�ڴ˼�¼һ�½��յ������ݰ�

		///Ѱ�Ҷ�Ӧ��Client
		int iIndex = stHead.m_Handle;
		//ͨ���������ҿ��е�client
		CNetClient* pstClient = m_stClientFactory.CalcObject(iIndex);
		if(!pstClient)
		{
			SL_WARNING("nethead not match client: handle = %u", iIndex);
			continue;
		}

		if(m_stClientFactory.IsFree(pstClient))
		{
			SL_WARNING("client(%d) has been freed, maybe client disconnected", iIndex);
			continue;
		}

		///������ݳ����Ƿ�Ϸ�
		if((int)stHead.m_iDataLength > CONF->SendBufferSize)
		{
			SL_WARNING("nethead(%d) datalen(%d > %d) invalid", iIndex,
				stHead.m_iDataLength, CONF->SendBufferSize);
			continue;
		}

		///���IP�Ͷ˿��Ƿ�ƥ��
		if(pstClient->m_stNetHead.m_RemoteIP != stHead.m_RemoteIP ||
			pstClient->m_stNetHead.m_RemotePort != stHead.m_RemotePort ||
			stHead.m_RemoteIP == 0||
			stHead.m_RemotePort == 0)
		{
			SL_WARNING("nethead(%d) ip invalid (%d:%d != %d:%d || %d:%d == 0)",
				iIndex,
				pstClient->m_stNetHead.m_RemoteIP, pstClient->m_stNetHead.m_RemotePort,
				stHead.m_RemoteIP, stHead.m_RemotePort,
				stHead.m_RemoteIP, stHead.m_RemotePort);
			continue;
		}

		///��ȡ�ϲ��flag��ֻ�е���8λ��Ϊ0ʱ������
		if(stHead.m_LiveFlag & 0xFF00)
		{
			pstClient->m_stNetHead.m_LiveFlag = stHead.m_LiveFlag & 0xFF00;
		}

		//����ϲ�״̬
		if(pstClient->m_stNetHead.m_LiveFlag & ENF_NET_FLAG_BUSY)  ///�ϲ���ַ�æ
		{

		}

		pstClient->m_stNetHead.m_Act1 = stHead.m_Act1;
		pstClient->m_stNetHead.m_Act2 = stHead.m_Act2;
		memcpy(pstClient->m_stNetHead.m_Key, stHead.m_Key, sizeof(pstClient->m_stNetHead.m_Key));
		
		SL_TRACE("get msg from shmqueue. handle=%d datalen=%d seq=%u", iIndex, stHead.m_iDataLength, stHead.m_HandleSeq);
		iRet = AllocSendBuffer(pstClient);
		if(iRet)
		{
			SL_FATAL("AllocSendBuffer fail %d", iRet);
			return;
		}

		if(stHead.m_iDataLength > 0)
		{
			if(pstClient->m_pstListen->m_stListenInfo.m_ucEncodeMethod == CODE_BIN ||
				pstClient->m_pstListen->m_stListenInfo.m_ucEncodeMethod == CODE_JSON)
			{
				if(pstClient->m_stSendBuf.GetFreeLen() < (int)(sizeof(unsigned int) + stHead.m_iDataLength))
				{
					SL_WARNING("handle(%d) ip(%d:%d) send buff is not enough(%d < %d)",
						iIndex, pstClient->m_stNetHead.m_RemoteIP, pstClient->m_stNetHead.m_RemotePort,
						pstClient->m_stSendBuf.GetFreeLen(), (int)(sizeof(unsigned int) + stHead.m_iDataLength));
					continue;
				}

				//�����ĸ��ֽڵĳ���
				unsigned int uiLen = htonl((unsigned int)(stHead.m_iDataLength + sizeof(int)));
				pstClient->m_stSendBuf.Append((char*)&uiLen, sizeof(unsigned int));

			}
			
			iRet = pstClient->m_stSendBuf.Append(m_stTempBuf.GetUsedBuf() + sizeof(CNetHead),stHead.m_iDataLength);
			if(iRet)
			{
				if(pstClient->m_stSendBuf.GetUsedLen() == 0)
				{
					FreeSendBuffer(pstClient);
				}

				SL_WARNING("append data failed, ret=%d, datalen=%d, freelen=%d",
					iRet, stHead.m_iDataLength, m_stTempBuf.GetFreeLen());
				continue;
			}

			SL_TRACE("handle(%d) has %d bytes to send", iIndex,
				pstClient->m_stSendBuf.GetUsedLen());
		}

		if(pstClient->m_stSendBuf.GetUsedLen() == 0)
		{
			SL_TRACE("no data to send");
			FreeSendBuffer(pstClient);
			continue;
		}

		SL_TRACE("---- handle(%d) begin send data, len=%d ----", iIndex,
			pstClient->m_stSendBuf.GetUsedLen());

		//��������
		iRet = SendSocket(pstClient);
		if(iRet)
		{
			SL_ERROR("send data failed %d", iRet);
			continue;
		}

	}

	
}

void CNetCtrl::OnListenEvent(CNetEpollObject* pstObject, SOCKET iSocket, int iEvent)
{
	SL_TRACE("recv listen event, event = %d", iEvent);
	if(!CEpoll::IsInputEvent(iEvent))
	{
		SL_WARNING("listen socket recv not match event(%d) socket=%d", iEvent, iSocket);
		return;
	}

	int iRet = 0;
	CNetListen* pstListen = (CNetListen*) pstObject;
	pstListen->m_uiLastTime = m_uiNow;

	///��������
	struct sockaddr_in clientaddr;
	socklen_t clilen = sizeof(clientaddr);
	SOCKET s = SL_INVALID_SOCKET;
	iRet = pstListen->m_stSocket.Accept(s, (sockaddr*)&clientaddr, &clilen);
	if(iRet)
	{
		///�ͻ����������Ժ������ر���
		SL_WARNING("remote connect and close. listen=%d ret=%d errno=%d",
			iSocket, iRet, SL_WSA_ERRNO);
		return;
	}

	///�ӹ�������CNetClient
	CNetClient* pstClient = m_stClientFactory.Alloc();
	if(pstClient == NULL)
	{
		SL_WARNING("m_stClientFactory Alloc failed");
		return;
	}

	iRet = pstClient->Init(*this, *pstListen, s,
		clientaddr.sin_addr.s_addr, ntohs(clientaddr.sin_port));
	pstClient->m_uiSeq = m_stClientFactory.GetClientSeq();
	CHECK_RETURN(iRet);

	SL_TRACE("Client %s:%d connected port %d at sock %d", 
		inet_ntoa(clientaddr.sin_addr),
		ntohs(clientaddr.sin_port),
		pstListen->m_stListenInfo.m_unListenPort,
		s);

}

void CNetCtrl::OnClientEvent(CNetEpollObject* pstObject, SOCKET iSocket, int iEvent)
{
	CNetClient* pstClient = (CNetClient*) pstClient;
	SL_TRACE("client socket %d handle %d event=%d",
		iSocket, pstClient->m_stNetHead.m_Handle, iEvent);

	if(CEpoll::IsCloseEvent(iEvent))
	{
		SL_ERROR("netctrl 445 set live flag ENF_NET_FLAG_REMOTECLOSE %d", ENF_NET_FLAG_REMOTECLOSE);
		CloseSocket(pstClient, ENF_NET_FLAG_REMOTECLOSE);
	}

	if(CEpoll::IsInputEvent(iEvent))
	{
		// ������ջ�����
		// ��������
		// ����������İ��ͷ���CTshmQueue
		// ������ջ�����Ϊ�վ��ͷ�
		RecvSocket(pstClient);
	}

	if(CEpoll::IsOutputEvent(iEvent))
	{
		SL_WARNING("client socket recv not match event(%d) socket=%d", iEvent, iSocket);
		return;
	}

}

int CNetCtrl::CheckRemain()
{
	int iIndex, iNewIdx;
	for (iIndex = m_stClientFactory.Head(REMAIN_LIST, REMAIN_INDEX);
		iIndex >= 0; iIndex = iNewIdx)
	{
		iNewIdx = m_stClientFactory.Next(iIndex, REMAIN_INDEX);
		CNetClient* pstClient = m_stClientFactory.CalcObject(iIndex);
		SL_ASSERT(pstClient);

		if(pstClient->m_stSendBuf.GetUsedLen() > 0)
		{
			SendSocket(pstClient);
		}
	}

	return 0;
}

int CNetCtrl::AfterRecvSocket(CNetClient* pstClient)
{
	/// �ж�Buf���Ƿ�����Ϣ���Էŵ����ն�����
	int iLen = 0;
	int iRet = 0;
	char* szMsgBuf = NULL;
	int iMsgLen = 0;
	bool bBreak = false;

	while(true)
	{
		iRet = 0;
		iMsgLen = 0;
		szMsgBuf = NULL;
		iLen = 0;

		if(pstClient->m_stRecvBuf.GetUsedLen() == 0)
		{
			SL_TRACE("recvbuf len = 0, PutOneMessage over");
			break;
		}

		iRet = BufferHasMessage(pstClient, iLen, szMsgBuf, iMsgLen, bBreak);
		if(iRet || iMsgLen < 0 || szMsgBuf == NULL)
		{
			if(iRet == 0 && iMsgLen != 0 && iMsgLen > pstClient->m_stRecvBuf.GetBufferSize())
			{
				SL_WARNING("recv a msg len greater than recv buff, msglen=%d, bufmax=%d", iMsgLen, pstClient->m_stRecvBuf.GetBufferSize());
				CloseSocket(pstClient, ENF_NET_FLAG_MSG_TOO_BIG);
				return 0;
			}
			else if(iRet == -2 || iMsgLen < 0)
			{
				SL_WARNING("BufferHasMessage return %d, or iMsgLen(%d) < 0,close this client instance",iRet, iMsgLen);
				CloseSocket(pstClient, ENF_NET_FALG_ERROR);
				return 0;
			}

			SL_TRACE("no msg in buf, ret = %d, msglen = %d, buf = %p", iRet, iMsgLen, szMsgBuf);
			break;
		}
		if(bBreak)
		{
			break;
		}
		SL_TRACE("Has one message len=%d msglen=%d, put to RecvQueue", iLen, iMsgLen);
		iRet = PutOneMessage(pstClient, iLen, szMsgBuf, iMsgLen);
		if(iRet)
		{
			SL_ERROR("PutOneMessage failed %d", iRet);
			break;
		}
	}

	//����ľ��ͷ�
	if(pstClient->m_stRecvBuf.GetUsedLen() == 0)
	{
		FreeRecvBuffer(pstClient);
	}

	return iRet;
}

int CNetCtrl::AfterSendSocket(CNetClient* pstClient)
{
	SL_TRACE("---- handle(%d) data send finished. left %d bytes ----",
		m_stClientFactory.CalcObjectPos(pstClient),
		pstClient->m_stSendBuf.GetUsedLen());

	if(pstClient->m_stSendBuf.GetUsedLen() == 0)
	{
		if(pstClient->m_stNetHead.m_LiveFlag & ENF_NET_FLAG_ASKCLOSE)
		{
			return CloseSocket(pstClient, ENF_NET_FLAG_ASKCLOSE);
		}
		else  //�ͷ�
		{
			m_stClientFactory.SetEmpty(pstClient);
			return FreeSendBuffer(pstClient);
		}
	}
	else
	{
		//�в�������, �ŵ��������ݶ���
		return m_stClientFactory.SetRemain(pstClient);
	}

}

int CNetCtrl::PutOneMessage(CNetClient* pstClient, int iLen, const char* szMsgBuf, int iMsgLen)
{
	int iRet = 0;
	
	SL_TRACE("put one message to RecvQueue");
	//�ȷ�����Ϣͷ
	pstClient->m_stNetHead.m_HandleSeq		=	pstClient->m_uiSeq;
	pstClient->m_stNetHead.m_iDataLength	=	iMsgLen;
	iRet = m_stShmQueue.PutOneCode(
		(const char*)(&pstClient->m_stNetHead), sizeof(pstClient->m_stNetHead),
		szMsgBuf, iMsgLen);
	if(iRet)
	{
		SL_ERROR("PutOneMessage call m_stShmQueue. PutOneCode fail, len1 %d len2 %d return %d", sizeof(pstClient->m_stNetHead), iMsgLen, iRet);
		return -3;
	}
	pstClient->m_iRecvBytes += iLen;
	pstClient->m_stRecvBuf.Remove(iLen);

	return 0;
}

int CNetCtrl::PutOneNotify(CNetClient* pstClient, unsigned short unLiveFlag)
{
	int iRet = 0;

	SL_TRACE("PutOneNotify flag=%d", unLiveFlag);
	pstClient->m_stNetHead.m_LiveFlag = unLiveFlag;
	pstClient->m_stNetHead.m_iDataLength = 0;

	iRet = m_stShmQueue.PutOneCode((char*)(&pstClient->m_stNetHead), sizeof(pstClient->m_stNetHead));
	if(iRet)
	{
		SL_ERROR("PutOneNotify failed %d",iRet);
	}
	return iRet;
}

int CNetCtrl::BufferHasMessage(CNetClient* pstClient, int& iLen, char*& pszMsgBuf, int& iMsgLen, bool& bBreak)
{
	iLen = 0;
	pszMsgBuf = NULL;
	iMsgLen = 0;
	bBreak = false;

	if(pstClient->m_pstListen->m_stListenInfo.m_stPackageType == NET_PACKAGE_STRING)
	{

	}
	else if(pstClient->m_pstListen->m_stListenInfo.m_stPackageType == NET_PACKAGE_BINARY)
	{
		if(pstClient->m_stRecvBuf.GetUsedLen() < static_cast<int>(sizeof(int)))
		{
			iMsgLen = 0;
			return 0;
		}

		int iPackageLen = ntohl( *(unsigned int*)(pstClient->m_stRecvBuf.GetUsedBuf()));
		if(iPackageLen < 0)
		{
			SL_WARNING("Invalid package length %d", iPackageLen);
			return -2;
		}
		else if(iPackageLen > pstClient->m_stRecvBuf.GetBufferSize())
		{
			SL_ERROR("packet length(%d) > recv buffer size(%d)", iPackageLen, pstClient->m_stRecvBuf.GetBufferSize());
			return -2;
		}
		else if(iPackageLen <= pstClient->m_stRecvBuf.GetUsedLen())
		{
			iLen = iPackageLen;
			pszMsgBuf = pstClient->m_stRecvBuf.GetUsedBuf() + sizeof(int);
			iMsgLen = iLen - sizeof(int);

			if(iMsgLen <= 0)
			{
				SL_WARNING("Invalid package length (%d) and message length (%d)", iLen, iMsgLen);
				return -2;
			}
		}

		if(iMsgLen <= 0)
		{
			return -1;
		}
		return 0;
	}

	return -1;
}

int CNetCtrl::AllocRecvBuffer(CNetClient* pstClient)
{
	if(pstClient->m_stRecvBuf.IsAttached())
	{
		return 0;
	}

	int iIndex = m_stRecvBufMgr.InfoListAlloc(BUFF_INDEX, USED_LIST);
	if(iIndex < 0)
	{
		return iIndex;
	}

	return pstClient->m_stRecvBuf.Attach(m_stRecvBufMgr[iIndex],
		m_stRecvBufMgr.GetObjectSize(), iIndex);
}

int CNetCtrl::AllocSendBuffer(CNetClient* pstClient)
{
	if(pstClient->m_stSendBuf.IsAttached())
	{
		return 0;
	}
	int iIndex = m_stSendBufMgr.InfoListAlloc(BUFF_INDEX, USED_LIST);
	if(iIndex < 0)
	{
		return iIndex;
	}

	return pstClient->m_stSendBuf.Attach(m_stSendBufMgr[iIndex],
		m_stSendBufMgr.GetObjectSize(), iIndex);
}

int CNetCtrl::FreeRecvBuffer(CNetClient* pstClient)
{
	if(!pstClient->m_stRecvBuf.IsAttached())
	{
		return 0;
	}
	m_stRecvBufMgr.InfoListMove(pstClient->m_stRecvBuf.Act(),
		BUFF_INDEX, FREE_LIST);

	pstClient->m_stRecvBuf.Detach();
	return 0;
}

int CNetCtrl::FreeSendBuffer(CNetClient* pstClient)
{
	if(!pstClient->m_stSendBuf.IsAttached())
	{
		return 0;
	}

	m_stSendBufMgr.InfoListMove(pstClient->m_stSendBuf.Act(),
		BUFF_INDEX, FREE_LIST);

	pstClient->m_stSendBuf.Detach();
	return 0;
}

int CNetCtrl::RecvSocket(CNetClient* pstClient)
{
	int iRet = 0;
	int iIndex = m_stClientFactory.CalcObjectPos(pstClient);

	if(m_stClientFactory.IsFree(pstClient))
	{
		SL_WARNING("client is invalid");
		return 0;
	}

	iRet = AllocRecvBuffer(pstClient);
	if(iRet)
	{
		SL_FATAL("handle(%d) alloc recv buffer failed ret=%d", iIndex, iRet);
		return -1;
	}

	pstClient->m_stNetHead.m_LastTime = m_uiNow;
	if(pstClient->m_stRecvBuf.GetFreeLen() <= 0)
	{
		SL_WARNING("handle(%d) recv buffer no space to recv", iIndex);
		return AfterRecvSocket(pstClient);
	}

	int iRecvLen = 0;
	iRet = pstClient->m_stSocket.Recv(pstClient->m_stRecvBuf.GetFreeBuf(),
		pstClient->m_stRecvBuf.GetFreeLen(), iRecvLen);

	if(iRecvLen > 0)
	{
		pstClient->m_stRecvBuf.Append(iRecvLen);
	}

	SL_TRACE("recv len=%d, ret=%d", iRecvLen, iRet);

	if(iRet)
	{
		if(iRet == SL_ECONNRESET)  //Զ�������ѹر�
		{
			AfterRecvSocket(pstClient);			// ���ܻ���notify��Ϣ
			SL_ERROR("netctrl 915 set live flag ENF_NET_FLAG_REMOTECLOSE %d", ENF_NET_FLAG_REMOTECLOSE);
			return CloseSocket(pstClient, ENF_NET_FLAG_REMOTECLOSE);
		}
		else if(iRet == SL_EWOULDBLOCK || iRet == SL_EINPROGRESS)
		{
			return AfterRecvSocket(pstClient);
		}
		else
		{
			SL_ERROR("handle(%d) recv failed %d", iIndex, iRet);
			CloseSocket(pstClient, ENF_NET_FALG_ERROR);
			return -1;
		}
	}

	return AfterRecvSocket(pstClient);
}

int CNetCtrl::SendSocket(CNetClient* pstClient)
{
	if(!pstClient->m_stSendBuf.IsAttached())
	{
		return 0;
	}

	if(pstClient->m_stSendBuf.GetUsedLen() <= 0)
	{
		SL_TRACE("no data to send");
		FreeSendBuffer(pstClient);
		return 0;
	}

	pstClient->m_stNetHead.m_LastTime = m_uiNow;

	int iRet = 0;
	int iIndex = m_stClientFactory.CalcObjectPos(pstClient);
	int iSendLen = 0;

	//����listen�˿ڵ�������������Ϣ�����Ա�ʶ
	//binary���;��Ƿ���2�ֽڵĳ���
	//string���;��Ǳ�֤�����\n��β
	// 
	if(pstClient->m_pstListen->m_stListenInfo.m_stPackageType == NET_PACKAGE_BINARY)
	{
		;
	}

	//����ʵ������
	//ԭ������Send��������Ͳ���ȥ�ᵼ��ѭ������ΪSendEx
	iRet = pstClient->m_stSocket.SendEx(pstClient->m_stSendBuf.GetUsedBuf(),
		pstClient->m_stSendBuf.GetUsedLen(), iSendLen);
	if(iRet && !(iRet == SL_EWOULDBLOCK || iRet == SL_EINPROGRESS))
	{
		//����ʧ��
		if(iRet == SL_ECONNRESET || iRet == EPIPE)
		{
			SL_WARNING("handle(%d) write msg size failed, client disconnected", iIndex);
		}else
		{
			SL_ERROR("handle(%d) write failed socket=%d err=%d", iIndex, pstClient->m_stSocket.GetSocket(), iRet);
		}
		CloseSocket(pstClient, ENF_NET_FALG_ERROR);
		return iRet;
	}

	if(iSendLen == 0)
	{
		SL_WARNING("handle(%d) ip(%d:%d) send 0 bytes", iIndex, pstClient->m_stNetHead.m_RemoteIP,
			pstClient->m_stNetHead.m_RemotePort);
		//����ϵͳ�ķ��ͻ������������Ǿ��ڻ���һ�£��Ժ���
		return AfterSendSocket(pstClient);
	}
	pstClient->m_iSendBytes += iSendLen;
	pstClient->m_stSendBuf.Remove(iSendLen);
	return AfterSendSocket(pstClient);
}
int CNetCtrl::CloseSocket(CNetClient* pstClient, unsigned short unLiveFlag)
{
	SL_TRACE("handle(%d) close and clear, liveflag=%hu, has %d bytes not send to client",
		m_stClientFactory.CalcObjectPos(pstClient), unLiveFlag, pstClient->m_stSendBuf.GetUsedLen());
	// ����3��������֪ͨ�ϲ�
	// 1.socket��Ч
	// 2.����netsvr�����رյ�
	// ���ӽ��������ϲ�ɹ���������
	if(pstClient->m_stSocket.IsValid() && !(unLiveFlag & ENF_NET_FLAG_ASKCLOSE) &&
		pstClient->m_iRecvBytes > 0)
	{
		PutOneNotify(pstClient, unLiveFlag);
	}

	FreeRecvBuffer(pstClient);
	FreeSendBuffer(pstClient);
	m_stClientFactory.Free(pstClient);

	//��¼���ӹر�

	return 0;
	
}

