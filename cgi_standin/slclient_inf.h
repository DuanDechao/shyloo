#ifndef __SLCLIENT_INF_H__
#define __SLCLIENT_INF_H__
#include "../sllib/slbase.h"
#include "../sllib/net/slsocket.h"
#include "../sllib/slcheck_mac_def.h"
#include "../sllib/slasync_cmd_factory.h"
#include "../sllib/slasync_cmd.h"
namespace sl
{
	#define CODE_STREAM_LENGTH 1024
	#define SERVER_IP "59.71.135.252"
	#define SERVER_PORT 18912
	class CClientInf
	{
	protected:
		CBuffer					m_stSendBuffer;						///< Server编码缓冲区
		CBuffer					m_stRecvBuffer;						///< 接受缓冲区
		CSocket					m_stSocket;							///< 连接对象
	public:
		CClientInf():m_stSocket(PF_INET, SOCK_STREAM, 0) 
		{

		}
		
		~CClientInf()
		{
			m_stSocket.Close();
		}

		template<typename T>
		int SendToServer(CMsgHead& stHead, T& stData)
		{
			CCodeStream s(m_stSendBuffer);
			s.InitConvert();
			int iRet = CodeConvert(s, stHead, NULL, bin_encode());

			CHECK_RETURN(iRet);

			iRet = CodeConvert(s, stData, NULL, bin_encode());
			CHECK_RETURN(iRet);

			iRet = m_stSendBuffer.Append(s.m_iPos);
			CHECK_RETURN(iRet);
			
			return SendToServer(m_stSendBuffer.GetUsedBuf(), m_stSendBuffer.GetUsedLen());
		}

		int Init()
		{
			int iRet;
			iRet = m_stRecvBuffer.Attach(NULL, CODE_STREAM_LENGTH, 0);
			CHECK_RETURN(iRet);

			iRet = m_stSendBuffer.Attach(NULL,CODE_STREAM_LENGTH, 0);
			CHECK_RETURN(iRet);

			return ConnectSvr();
		}

		bool Recv()
		{
			//SL_TRACE("begin recv data!");
			bool bIsRecvData = false;
			bool bIsBuffFull = false;  //缓冲区是否已满
			do 
			{
				int iRecvLen = 0;
				int iFreeLen = m_stRecvBuffer.GetFreeLen();
				int iRet = m_stSocket.Recv(m_stRecvBuffer.GetFreeBuf(), m_stRecvBuffer.GetFreeLen() - 1, iRecvLen);

				if(iRet)
				{
					//接收数据错误
					//SL_ERROR("svr(%d) socket recv error, ret %d", m_SvrConnectParam.Index, iRet);
					//SetReconnect();
					return false;
				}
				else
				{
					//读到了数据附加到缓冲区
					m_stRecvBuffer.Append(iRecvLen);
				}

				bIsBuffFull  = (iRecvLen == iFreeLen - 1);
				if(iRecvLen > 0)
				{
					bIsRecvData = true;
				}
				//调用处理函数将数据消化掉
				iRet = RecvdData();
				if(iRet < 0)
				{
					//接收buff中的数据
					//SL_ERROR("svr(%d) buff recv data fail reconnect!", m_SvrConnectParam.Index);
					//SetReconnect();
					return false;
				}
			} while (bIsBuffFull);

			//SL_TRACE("end recv Data");
			return bIsRecvData;
		}

	protected:
		int ConnectSvr()
		{
			int iRet;
			iRet = m_stSocket.Create();
			if(iRet)
			{
				return iRet;
			}
			SOCKET stSocket = m_stSocket.GetSocket();
			
			int iRecvBufferSize = 65534;
			iRet = setsockopt(stSocket, SOL_SOCKET, SO_RCVBUF, (char *)&iRecvBufferSize, sizeof(iRecvBufferSize));
			if(iRet)
			{
				SL_ERROR("Set Socket Recvive Buffer Fail： errno=%d", SL_ERRNO);
				return iRet;
			}

			//设置发送缓冲区
			int iSendBufferSize = 65534;
			iRet = setsockopt(stSocket, SOL_SOCKET, SO_SNDBUF,(char *)&iSendBufferSize, sizeof(iSendBufferSize));
			if (iRet)
			{
				SL_ERROR("Set Socket Send Buffer Fail: errno=%d", SL_ERRNO);
				return iRet;
			}

			int sndbuf = 0;
			socklen_t optlen = sizeof(sndbuf);
			iRet = getsockopt(stSocket,SOL_SOCKET, SO_SNDBUF, &sndbuf, &optlen);
			int iNoDelay = 1;
			//设置NoDelay
			iRet = setsockopt(stSocket, IPPROTO_TCP, TCP_NODELAY, (char *)&iNoDelay, sizeof(iNoDelay));
			if(iRet)
			{
				//SL_ERROR("Set Socket No Delay Fail: errno=%d", SL_ERRNO);
				return iRet;
			}

			//连接
			iRet = m_stSocket.Connect(SERVER_IP, SERVER_PORT);
			if(iRet)
			{
				//SL_ERROR("connect server(%d) (%s:%hu) failed, ret=%d", m_SvrConnectParam.Index, m_SvrConnectParam.ServerIP.c_str(),
					//m_SvrConnectParam.ServerPort, iRet);
				m_stSocket.Close();
				return -1;
			}
		}

		int SendToServer(const char* pszBuf, int iBufLen)
		{
			//将数据长度附在头部
			int iPackLen = htonl(iBufLen + sizeof(unsigned int));
			if(m_stSendBuffer.GetFreeLen() < iBufLen + static_cast<int>(sizeof(unsigned int)))
			{
				//发送缓冲区已满
				//SL_WARNING("Send Buff Is Full!");
				return -1;
			}
			//讲数据长度附在头部
			m_stSendBuffer.Append((const char*)&iPackLen, sizeof(unsigned int));

			//附加数据
			int iRet = m_stSendBuffer.Append(pszBuf, iBufLen);
			if(iRet)
			{
				//SL_ERROR("store %d bytes in send buffer failed, ret=%d, buffer freelen=%d",
				//	iBufLen, iRet, m_stSendBuffer.GetFreeLen());
				return iRet;
			}
			return CheckSend();
		}

		int CheckSend()
		{
			if(m_stSendBuffer.GetUsedLen() == 0)
			{
				return -1;
			}
			int iRet = 0;
			int iSendLen = 0;
			iRet = m_stSocket.Send(m_stSendBuffer.GetUsedBuf(), m_stSendBuffer.GetUsedLen(), iSendLen);
			m_stSendBuffer.Remove(iSendLen);

			if(iRet)
			{
				printf("error:%d",iRet);
				return iRet;
			}
		}

		

		int RecvdData()
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
					//SL_ERROR("the data length(%u) is, but buff size(%d) is!", iDataLen, stData.GetBufferSize());
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

		void HandleMsg(char* pstBuff, int iDataLen)
		{
			CCodeStream s;
			s.Attach(pstBuff, iDataLen);
			s.InitConvert();

			CMsgHead stMsgHead;
			int iRet = CodeConvert(s, stMsgHead, NULL, bin_decode());
			
			if(iRet)
			{
				return;
			}
			if(!stMsgHead.IsValid())
			{
				//SL_ERROR("head one msg(%u) head is not valid!", stMsgHead.m_shMsgID);
				return;
			}
			//SL_TRACE("svrconn recv msg, MsgId=%d, Type=%d", stMsgHead.m_shMsgID, stMsgHead.m_shMsgType);

			//判断消息类型
			//if(stMsgHead.m_shMsgID == MSG_)
			if(stMsgHead.m_shMsgType == EMT_ANSWER && stMsgHead.m_llMsgAct)
			{
				//根据ACT找到对应发出请求的异步命令
				CAsyncCmdInf* pstCmd = reinterpret_cast<CAsyncCmdInf*>(stMsgHead.m_llMsgAct);
				//if(!SL_CMDFACTORY->IsValidCmd(pstCmd))
				//{
				//	SL_WARNING("this cmd is invalid!");
				//	return;
				//}

				////检查消息的Seq是否匹配
				//if(pstCmd->GetCmdSeq() != stMsgHead.m_iCmdSeq)
				//{
				//	SL_WARNING("the cmd is not match !");
				//	return;
				//}

				CCmdOnAnswerParam stParam(stMsgHead, s);
				pstCmd->OnAnswer(&stParam);
			}
			//else if(stMsgHead.m_shMsgType == EMT_NOTIFY)
			//{
			//	//可能需要给用户一些简单的返回
			//	CAsyncCmdInf* pstCmd = SL_CMDFACTORY->CreateCmd(stMsgHead.m_shMsgID);
			//	if(pstCmd == NULL)
			//	{
			//		//创建命令失败
			//		SL_WARNING("Create cmd(%d) fails!", stMsgHead.m_shMsgID);
			//		return;
			//	}

			//	CCmdOnAnswerParam stParam(stMsgHead, s);
			//	pstCmd->Do(&stParam);
			//}
			//else
			//{
			//	SL_WARNING("cmd type is error %d", stMsgHead.m_shMsgType);
			//}
		}
	};

#define SL_CLIENT   (CSingleton<CClientInf>::Instance())
}
#endif
