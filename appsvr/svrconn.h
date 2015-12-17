//���������������ӵ���
/********************************************************************
	created:	2015/12/17
	created:	17:12:2015   10:28
	filename: 	d:\workspace\shyloo\appsvr\svrconn.h
	file path:	d:\workspace\shyloo\appsvr
	file base:	svrconn
	file ext:	h
	author:		ddc
	
	purpose:	���������������ӵ���
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
		SL_CLOSED = 0,      ///�����ѹر�
		SL_CONNECTING = 1,  ///���ڳ��Խ�������,���ӻ�û����
		SL_CONNECTED = 2,   ///�����Ѿ��ɹ�����
	};

	//����������Ϣ
	class CSvrConnectParam
	{
	public:
		int				Index;				///< ���ӵ���ţ������0��ʼ���
		string			ServerIP;			///< ������IP��ַ
		ushort			ServerPort;			///< �������˿ں�
		int				RecvBufferSize;		///< ������Buff��С
		int				SendBufferSize;		///< ������Buff��С
		string			Desc;				///< ������������
		CSvrConnectParam(): Index(-1), ServerPort(0), RecvBufferSize(0), SendBufferSize(0){}
	};

	//���������������
	class CSvrConnect: public CEpollObject<CSvrConnect>
	{
	public:
		//���캯��
		CSvrConnect()
			:m_bInited(false),
			 m_stSocket(PF_INET, SOCK_STREAM, false),
			 m_uchConnState(SL_CLOSED),
			 m_uchOldConnState(0),
			 m_iConnectCounts(0),
			 m_iRequestConnectCount(0)
		{}

	public:
		//��ʼ������
		int Init(CEpoll& stEpoll, const CSvrConnectParam& stSvrConnectParam);

		bool IsInited() {return m_bInited;}

		//�������¼�������
		void OnSvrEvent(CEpollObject<CSvrConnect>* pstObject, SOCKET iSocket, int iEvent);

		//���ӷ�����
		int ConnectSvr();

		void CloseConnect()
		{
			SL_TRACE("CloseConnect!");
			//if()
		}

		//���Ͳ���������
		void CheckSend();

		//��������
		int Send(const char* pszBuf, int iBufLen);

		//��ȡ�������󲻳ɹ�����
		int GetRequestConnectCount() const
		{
			return m_iRequestConnectCount;
		}

		///�����Ƿ�ر�
		bool IsConnectClosed() const
		{
			return m_uchConnState == SL_CLOSED;
		}

		//��ȡ����״̬
		int GetConnectStatus() const
		{
			return m_uchConnState;
		}

		//��ȡ���뻺����
		CBuffer& GetEncodeBuffer()
		{
			return m_stCodeBuf;
		}

		//������Ϣ
		template<typename T>
		int SendData(CMsgHead& stHead, T& stData)
		{
			int iRet = 0;

			//��������������Ҫ����
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

		///ѭ�����
		void CheckConnect()
		{
			ConnectSvr();
			CheckSend();
		}

		//���ڳ����շ��쳣ʱ��������
		void SetReconnect();

	protected:
		///�������ݴ���
		void Recv();

		//�����ݶ��봦����Щ����
		int RecvdData();

		void HandleMsg(char* pstBuff, int iDataLen);

	protected:
		bool				m_bInited;
		CEpoll*				m_pstEpoll;				///< Epoll
		CBuffer				m_stRecvBuffer;			///< �������ݻ�����
		CBuffer				m_stSendBuffer;			///< �������ݻ�����
		CSocket				m_stSocket;				///< ���Ӷ���
		unsigned char		m_uchConnState;			///< d��ǰ����״̬
		unsigned char		m_uchOldConnState;		///< ֮ǰ����״̬
		sl::uint			m_iConnectCounts;		///< ���Ӽ���
		int					m_iRequestConnectCount;	///< �������Ӵ���
		CSvrConnectParam	m_SvrConnectParam;		///< ���ӷ��������ö���
		CBuffer				m_stCodeBuf;			///< ����


	};
}
#endif