#include "svrconn.h"
#include "../sllib/slasync_cmd_factory.h"
#include "../sllib/slasync_cmd.h"
#include "../sllib/slcheck_mac_def.h"
using namespace  sl;
int CSvrConnect::Init(CEpoll& stEpoll, const CSvrConnectParam& stSvrConnectParam)
{
	m_stCodeBuf.Attach(NULL, STREAM_BUFF_LENGTH, 0);
	int iRet = 0;
	m_pstEpoll = &stEpoll;
	m_SvrConnectParam = stSvrConnectParam;

	iRet = m_stRecvBuffer.Attach(NULL, m_SvrConnectParam.RecvBufferSize, 0);
	CHECK_RETURN(iRet);

	iRet = m_stSendBuffer.Attach(NULL, m_SvrConnectParam.SendBufferSize, 0);
	CHECK_RETURN(iRet);

	m_bInited = true;

	return 0;
}

void CSvrConnect::OnSvrEvent(CEpollObject<CSvrConnect>* pstObject, SOCKET iSocket, int iEvent)
{
	int iRet = 0;
	SL_TRACE("recv svr(%d) event %d, socket = %d, connect stat is %d",
		m_SvrConnectParam.Index, iEvent, iSocket, m_uchConnState);

	//连接成功
	if(CEpoll::IsOutputEvent(iEvent))
	{
		SL_TRACE("svr(%d) connect(%s:%hu) ok, socket=%d", m_SvrConnectParam.Index,
			m_SvrConnectParam.ServerIP.c_str(), m_SvrConnectParam.ServerPort, iSocket);

		m_uchConnState =  m_uchOldConnState = SL_CONNECTED;
		++m_iConnectCounts;
		m_iRequestConnectCount = 0;

		//改为读事件
		iRet = ModRegEvent(EPOLLIN);
		if(iRet)
		{
			SL_ERROR("ModRegEvent failed %d, errno=%d, socket=%d", iRet, SL_ERRNO, iSocket);
			return;
		}
	}

    //有数据返回（对端断开连接也会触发该事件）
	if(CEpoll::IsInputEvent(iEvent))
	{
		Recv();
	}

	///连接被断开
	if(CEpoll::IsCloseEvent(iEvent))
	{
		SL_WARNING("svr(%d) connect(%s:%hu) close, socket=%d event=%d",
			m_SvrConnectParam.Index, m_SvrConnectParam.ServerIP.c_str(), m_SvrConnectParam.ServerPort, iSocket, iEvent);
		
		//设置重连标示
		SetReconnect();
	}
}

int CSvrConnect::ConnectSvr()
{
	if(m_uchConnState != SL_CLOSED)
	{
		return -1;
	}

	//创建socket
	int iRet = m_stSocket.Create();
	if(iRet)
	{
		return -1;
	}

	int iSendBufferSize = m_SvrConnectParam.SendBufferSize;
	int iRecvBufferSize = m_SvrConnectParam.RecvBufferSize;
	int iNoDelay = 1;

	//设置接收缓冲区
	SOCKET stSocket = m_stSocket.GetSocket();
	iRet = setsockopt(stSocket, SOL_SOCKET, SO_RCVBUF, (char *)&iRecvBufferSize, sizeof(iRecvBufferSize));
	if(iRet)
	{
		SL_ERROR("Set Socket Recvive Buffer Fail： errno=%d", SL_ERRNO);
		return iRet;
	}

	//设置发送缓冲区
	iRet = setsockopt(stSocket, SOL_SOCKET, SO_SNDBUF,(char *)&iSendBufferSize, sizeof(iSendBufferSize));
	if (iRet)
	{
		SL_ERROR("Set Socket Send Buffer Fail: errno=%d", SL_ERRNO);
		return iRet;
	}

	//设置NoDelay
	iRet = setsockopt(stSocket, IPPROTO_TCP, TCP_NODELAY, (char *)&iNoDelay, sizeof(iNoDelay));
	if(iRet)
	{
		SL_ERROR("Set Socket No Delay Fail: errno=%d", SL_ERRNO);
		return iRet;
	}

	//注册Epoll
	iRet = Register(*this, &CSvrConnect::OnSvrEvent, *m_pstEpoll, m_stSocket.GetSocket(), EPOLLOUT); // 连接时是监听OUT事件
	if(iRet)
	{
		m_stSocket.Close();
		return -1;
	}

    //连接
	iRet = m_stSocket.Connect(m_SvrConnectParam.ServerIP.c_str(), m_SvrConnectParam.ServerPort);
	if(iRet)
	{
		SL_ERROR("connect server(%d) (%s:%hu) failed, ret=%d", m_SvrConnectParam.Index, m_SvrConnectParam.ServerIP.c_str(),
			m_SvrConnectParam.ServerPort, iRet);
		
		Unregister();
		m_stSocket.Close();

		return -1;
	}

	

	SL_TRACE("connect svr(%d) ok, socket = %d", m_SvrConnectParam.Index, m_stSocket.GetSocket());
	m_uchConnState = m_uchOldConnState = SL_CONNECTING;
	return 0;
}

void CSvrConnect::CheckSend()
{
	if(m_stSendBuffer.GetUsedLen() == 0)
	{
		return;
	}

	if(m_uchConnState != SL_CONNECTED || m_uchConnState != m_uchOldConnState)
	{
		SL_WARNING("svr(%d) not connect, now stat is %d, check send return", m_SvrConnectParam.Index, m_uchConnState);
		m_uchOldConnState = m_uchConnState;
		return;
	}

	int iRet = 0;
	int iSendLen = 0;
	iRet = m_stSocket.SendEx(m_stSendBuffer.GetUsedBuf(), m_stSendBuffer.GetUsedLen(), iSendLen);
	if(iRet && iRet != SL_EINPROGRESS && iRet != SL_EWOULDBLOCK)
	{
		SL_WARNING("socket send error, ret %d", iRet);
		SetReconnect();
		return;
	}

	SL_TRACE("svr(%d) need send %d bytes, actually send %d bytes", m_SvrConnectParam.Index,
		m_stSendBuffer.GetUsedLen(), iSendLen);

	m_stSendBuffer.Remove(iSendLen);
}

int CSvrConnect::Send(const char* pszBuf, int iBufLen)
{
	//将数据长度附在头部
	int iPackLen = htonl(iBufLen + sizeof(unsigned int));
	if(m_stSendBuffer.GetFreeLen() < iBufLen + static_cast<int>(sizeof(unsigned int)))
	{
		//发送缓冲区已满
		SL_WARNING("Send Buff Is Full!");
		return -1;
	}
	//讲数据长度附在头部
	m_stSendBuffer.Append((const char*)&iPackLen, sizeof(unsigned int));

	//附加数据
	int iRet = m_stSendBuffer.Append(pszBuf, iBufLen);
	if(iRet)
	{
		SL_ERROR("store %d bytes in send buffer failed, ret=%d, buffer freelen=%d",
			iBufLen, iRet, m_stSendBuffer.GetFreeLen());
	}

	SL_TRACE("send data size is (%d)!", m_stSendBuffer.GetUsedLen());

	CheckSend();
	
	return iRet;
}

void CSvrConnect::Recv()
{
	SL_TRACE("begin recv data!");

	bool bIsBuffFull = false;  //缓冲区是否已满
	do 
	{
		int iRecvLen = 0;
		int iFreeLen = m_stRecvBuffer.GetFreeLen();
		int iRet = m_stSocket.Recv(m_stRecvBuffer.GetFreeBuf(), m_stRecvBuffer.GetFreeLen() - 1, iRecvLen);

		if(iRet)
		{
			//接收数据错误
			SL_ERROR("svr(%d) socket recv error, ret %d", m_SvrConnectParam.Index, iRet);
			SetReconnect();
			return;
		}
		else
		{
			//读到了数据附加到缓冲区
			m_stRecvBuffer.Append(iRecvLen);
		}

		bIsBuffFull  = (iRecvLen == iFreeLen - 1);

		//调用处理函数将数据消化掉
		iRet = RecvdData();
		if(iRet < 0)
		{
			//接收buff中的数据
			SL_ERROR("svr(%d) buff recv data fail reconnect!", m_SvrConnectParam.Index);
			SetReconnect();
			return;
		}
	} while (bIsBuffFull);
	
	SL_TRACE("end recv Data");
}

void CSvrConnect::SetReconnect()
{
	SL_TRACE("SetReconnect");
	m_uchConnState = SL_CLOSED;

	//撤销注册
	int iRet = Unregister();
	if(iRet)
	{
		SL_ERROR("unregister svr(%d) connect failed", m_SvrConnectParam.Index);
	}
	m_stSocket.Close();
	++m_iRequestConnectCount;

}

int CSvrConnect::RecvdData()
{
	CBuffer& stData = m_stRecvBuffer;
	do 
	{
		if(stData.GetUsedLen() <= (int)sizeof(unsigned int))
		{
			//缓冲区里没有足够的数据
			return 0;
		}

		//判断Buffer里是不是已经有一个完成的Code
		int iDataLen = ntohl(*(unsigned int*) stData.GetUsedBuf());
		if(iDataLen > stData.GetBufferSize())
		{
			//当前数据出现错误， 数据包大小超过了buff的总长度
			SL_ERROR("the data length(%u) is, but buff size(%d) is!", iDataLen, stData.GetBufferSize());
			return -1;
		}

		if(stData.GetUsedLen() < iDataLen)
		{
			//当前数据不够一个数据包
			break;
		}
		HandleMsg(stData.GetUsedBuf() + sizeof(unsigned int), iDataLen - sizeof(unsigned int));
		
		//把这个数据包从缓冲区消去
		stData.Remove(iDataLen);
	} while (true);
}

void CSvrConnect::HandleMsg(char* pstBuff, int iDataLen)
{
	CCodeStream s;
	s.Append(pstBuff, iDataLen);
	s.InitConvert();
	
	CMsgHead stMsgHead;
	int iRet = CodeConvert(s, stMsgHead, NULL, bin_decode());
	if(iRet)
	{
		SL_ERROR("decode data fails!");
		return;
	}

	if(!stMsgHead.IsValid())
	{
		SL_ERROR("head one msg(%u) head is not valid!", stMsgHead.m_shMsgID);
		return;
	}
	SL_TRACE("svrconn recv msg, MsgId=%d, Type=%d", stMsgHead.m_shMsgID, stMsgHead.m_shMsgType);

	//判断消息类型
	//if(stMsgHead.m_shMsgID == MSG_)
	if(stMsgHead.m_shMsgType == EMT_ANSWER && stMsgHead.m_llMsgAct)
	{
		//根据ACT找到对应发出请求的异步命令
		CAsyncCmdInf* pstCmd = reinterpret_cast<CAsyncCmdInf*>(stMsgHead.m_llMsgAct);
		if(!SL_CMDFACTORY->IsValidCmd(pstCmd))
		{
			SL_WARNING("this cmd is invalid!");
			return;
		}

		//检查消息的Seq是否匹配
		if(pstCmd->GetCmdSeq() != stMsgHead.m_iCmdSeq)
		{
			SL_WARNING("the cmd is not match !");
			return;
		}

		CCmdOnAnswerParam stParam(stMsgHead, s);
		pstCmd->OnAnswer(&stParam);
	}
	else if(stMsgHead.m_shMsgType == EMT_NOTIFY)
	{
		//可能需要给用户一些简单的返回
		CAsyncCmdInf* pstCmd = SL_CMDFACTORY->CreateCmd(stMsgHead.m_shMsgID);
		if(pstCmd == NULL)
		{
			//创建命令失败
			SL_WARNING("Create cmd(%d) fails!", stMsgHead.m_shMsgID);
			return;
		}

		CCmdOnAnswerParam stParam(stMsgHead, s);
		pstCmd->Do(&stParam);
	}
}