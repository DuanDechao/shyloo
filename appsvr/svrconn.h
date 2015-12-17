//与其他服务器连接的类
/********************************************************************
	created:	2015/12/17
	created:	17:12:2015   10:28
	filename: 	d:\workspace\shyloo\appsvr\svrconn.h
	file path:	d:\workspace\shyloo\appsvr
	file base:	svrconn
	file ext:	h
	author:		ddc
	
	purpose:	与其他服务器连接的类
*********************************************************************/

#ifndef __SVR_CONNECT_H__
#define __SVR_CONNECT_H__

#include "../sllib/net/slepoll.h"
#include "../sllib/sllog.h"
#include "../sllib/slsingleton.h"
#include "../sllib/slcode_convert.h"
#include "../sllib/slmsg_base.h"

namespace sl
{
	enum
	{
		STREAM_BUFF_LENGTH = 0x1FFFF,
	};

	enum
	{
		SL_CLOSED = 0,      ///连接已关闭
		SL_CONNECTING = 1,  ///正在尝试建立连接,连接还没建立
		SL_CONNECTED = 2,   ///连接已经成功建立
	};

	//连接配置信息
	class CSvrConnectParam
	{
	public:
		int				Index;				///< 连接的序号，必须从0开始序号
		string			ServerIP;			///< 服务器IP地址
		ushort			ServerPort;			///< 服务器端口号
		int				RecvBufferSize;		///< 接收区Buff大小
		int				SendBufferSize;		///< 发送区Buff大小
		string			Desc;				///< 服务器的描述
		CSvrConnectParam(): Index(-1), ServerPort(0), RecvBufferSize(0), SendBufferSize(0){}
	};

	//与服务器的连接类
	class CSvrConnect: public CEpollObject<CSvrConnect>
	{
	public:
		//构造函数
		CSvrConnect()
			:m_bInited(false),
			 m_stSocket(PF_INET, SOCK_STREAM, false),
			 m_uchConnState(SL_CLOSED),
			 m_uchOldConnState(0),
			 m_iConnectCounts(0),
			 m_iRequestConnectCount(0)
		{}

	public:
		//初始化连接
		int Init(CEpoll& stEpoll, const CSvrConnectParam& stSvrConnectParam);

		bool IsInited() {return m_bInited;}

		//服务器事件处理函数
		void OnSvrEvent(CEpollObject<CSvrConnect>* pstObject, SOCKET iSocket, int iEvent);

		//连接服务器
		int ConnectSvr();

		void CloseConnect()
		{
			SL_TRACE("CloseConnect!");
			//if()
		}

		//发送残留的请求
		void CheckSend();

		//发送数据
		int Send(const char* pszBuf, int iBufLen);

		//获取本次请求不成功次数
		int GetRequestConnectCount() const
		{
			return m_iRequestConnectCount;
		}

		///连接是否关闭
		bool IsConnectClosed() const
		{
			return m_uchConnState == SL_CLOSED;
		}

		//获取连接状态
		int GetConnectStatus() const
		{
			return m_uchConnState;
		}

		//获取编码缓冲区
		CBuffer& GetEncodeBuffer()
		{
			return m_stCodeBuf;
		}

		//发送消息
		template<typename T>
		int SendData(CMsgHead& stHead, T& stData)
		{
			int iRet = 0;

			//服务器有数据需要发送
			CCodeStream s(m_stCodeBuf);

			if(m_stCodeBuf.GetFreeLen() - sizeof(unsigned int) <= 0)
			{
				SL_ERROR("Code Buff Free is use up!");
				return -1;
			}

			s.InitConvert();
			iRet = CodeConvert(s, stHead, NULL, bin_encode());
			if(iRet)
			{
				SL_ERROR("code data Head MsgID(%u) fails(%d)", stHead.m_shMsgID, iRet);
				return -2;
			}
			iRet = CodeConvert(s, stData, NULL, bin_encode());
			if(iRet)
			{
				SL_ERROR("code data fails(%d) MsgID(%u)", iRet, stHead.m_shMsgID);
				return -2;
			}

			iRet = m_stCodeBuf.Append(s.m_iPos);
			if(iRet)
			{
				SL_ERROR("code length(%d) is, but code free buffer(%d) is", s.m_iPos, m_stCodeBuf.GetFreeLen());
				return -3;
			}

			return Send(m_stCodeBuf.GetUsedBuf(), m_stCodeBuf.GetUsedLen());
		}

		///循环检查
		void CheckConnect()
		{
			ConnectSvr();
			CheckSend();
		}

		//用于出现收发异常时重新连接
		void SetReconnect();

	protected:
		///接受数据处理
		void Recv();

		//有数据读入处理这些数据
		int RecvdData();

		void HandleMsg(char* pstBuff, int iDataLen);

	protected:
		bool				m_bInited;
		CEpoll*				m_pstEpoll;				///< Epoll
		CBuffer				m_stRecvBuffer;			///< 接受数据缓冲区
		CBuffer				m_stSendBuffer;			///< 发送数据缓冲区
		CSocket				m_stSocket;				///< 连接对象
		unsigned char		m_uchConnState;			///< d当前连接状态
		unsigned char		m_uchOldConnState;		///< 之前连接状态
		sl::uint			m_iConnectCounts;		///< 连接计数
		int					m_iRequestConnectCount;	///< 请求连接次数
		CSvrConnectParam	m_SvrConnectParam;		///< 连接服务器配置对象
		CBuffer				m_stCodeBuf;			///< 缓存


	};
}
#endif