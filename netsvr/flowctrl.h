//流量控制类
/********************************************************************
	created:	2015/12/08
	created:	8:12:2015   15:18
	filename: 	e:\workspace\shyloo\netsvr\flowctrl.h
	file path:	e:\workspace\shyloo\netsvr
	file base:	flowctrl
	file ext:	h
	author:		ddc
	
	purpose:	流量控制类
*********************************************************************/
#ifndef _FLOW_CTRL_H_
#define _FLOW_CTRL_H_
#include "../sllib/slsize_string.h"
#include "netdef.h"

namespace sl
{
	class CTimeIntervalCount
	{
	public:
		CTimeIntervalCount(): m_iNowCount(0)
		{
			SetBeginTime();
		}

		void SetInterval(int iSecInterval, int iUsecInterval)
		{
			m_iSecInterval   = iSecInterval;
			m_iUsecInterval  = iUsecInterval;
		}

		void Add(int iCount = 1)
		{
			if(IsCountExpired())
			{
				SetBeginTime();
				m_iNowCount	=	iCount;
			}
			else
			{
				m_iNowCount += iCount;
			}
		}

		int GetCount()
		{
			Add(0);
			return m_iNowCount;
		}
	private:
		int				m_iSecInterval;
		int				m_iUsecInterval;

		int				m_iNowCount;
		struct timeval  m_stBeginStartTime;
		struct timeval  m_stNowTime;

		bool IsCountExpired()
		{
#ifndef SL_OS_WINDOWS
			gettimeofday(&m_stNowTime, NULL);
			return ((m_stNowTime.tv_sec - m_stBeginStartTime.tv_sec) * 1000000 + (m_stNowTime.tv_usec - m_stBeginStartTime.tv_usec)
				- m_iSecInterval * 1000000 - m_iUsecInterval) > 0;
#else
			return	false;
#endif
		}

		void SetBeginTime()
		{
#ifndef SL_OS_WINDOWS
			gettimeofday(&m_stBeginStartTime, NULL);
#else
			m_stBeginStartTime.tv_sec = 0;
			m_stBeginStartTime.tv_usec = 0;
#endif
		}

	}; ///class CTimeIntervalCount

	class CFlowCtrl;

	class CFlowCtrlPolicy
	{
	public:
		friend class CFlowCtrl;
		CFlowCtrlPolicy(): m_bIsOverflow(false), m_pstNextCtrl(NULL)
		{
		}

		virtual ~CFlowCtrlPolicy() {}

		virtual bool IsOverflow()
		{
			if(m_bIsOverflow)
			{
				return m_bIsOverflow;
			}
			if(m_pstNextCtrl == NULL)
			{
				return m_bIsOverflow;
			}
			return m_pstNextCtrl->IsOverflow();
		}

		void RegistOtherCtrl(CFlowCtrlPolicy* pstCFlowCtrl)
		{
			if(m_pstNextCtrl != NULL)
			{
				m_pstNextCtrl->RegistOtherCtrl(pstCFlowCtrl);
			}
			else
			{
				m_pstNextCtrl = pstCFlowCtrl;
			}
		}
	protected:
		bool m_bIsOverflow;
	private:
		CFlowCtrlPolicy* m_pstNextCtrl;
	
	}; /// class CFlowCtrlPolicy

	class CStopIntervalFlowCtrl: public CFlowCtrlPolicy
	{
	public:
		friend class CFlowCtrl;
		CStopIntervalFlowCtrl(): m_iStopBeginTime(0), m_iStopInterval(0) {}
		void SetStopInterval(int iSec)
		{
			m_iStopInterval = iSec;
		}

		void Stop()
		{
			m_iStopBeginTime = (int)time(NULL);
			m_bIsOverflow	 =	true;
		}

		bool IsOverflow()
		{
			if(time(NULL) - m_iStopBeginTime > m_iStopInterval)
			{
				m_bIsOverflow = false;
			}
			return CFlowCtrlPolicy::IsOverflow();
		}
	private:
		int		m_iStopBeginTime;
		int		m_iStopInterval;

	}; //class CStopIntervalFlowCtrl


	class CAcceptNumFlowCtrl: public CFlowCtrlPolicy
	{
	public:
		friend class CFlowCtrl;
		CAcceptNumFlowCtrl(): m_iAcceptCount(0), m_iMaxAcceptCount(1000){}

		void SetMaxAcceptCount(int iMaxAcceptCount)
		{
			if(iMaxAcceptCount < 0)
			{
				return;
			}
			m_iMaxAcceptCount = iMaxAcceptCount;
		}

		void DelAcceptCount()
		{
			if(m_iAcceptCount > 0)
			{
				m_iAcceptCount--;
			}
			if(m_iAcceptCount < m_iMaxAcceptCount)
			{
				m_bIsOverflow = false;
			}
		}

		void AddAcceptCount()
		{
			if(m_iAcceptCount < m_iMaxAcceptCount)
			{
				m_iAcceptCount++;
			}

			if(m_iAcceptCount >= m_iMaxAcceptCount)
			{
				m_bIsOverflow = true;
			}
		}

	private:
		int		m_iAcceptCount;
		int     m_iMaxAcceptCount;

	};  /// class CAcceptNumFlowCtrl

	class CAcceptRateFlowCtrl: public CFlowCtrlPolicy
	{
	public:
		friend class CFlowCtrl;
		CAcceptRateFlowCtrl(): m_iMaxConnect(1000){}

		void AcceptNewConnect()
		{
			stTimeIntervalCount.Add();
		}

		void SetAcceptRate(int iIntervalMsec, int iMaxConnect)
		{
			stTimeIntervalCount.SetInterval(iIntervalMsec / 1000, iIntervalMsec % 1000);
			m_iMaxConnect = iMaxConnect;
		}

		bool IsOverflow()
		{
			if(stTimeIntervalCount.GetCount() >= m_iMaxConnect)
			{
				m_bIsOverflow	=	true;
			}
			else
			{
				m_bIsOverflow	=	false;
			}

			return CFlowCtrlPolicy::IsOverflow();
		}
	private:
		int		m_iMaxConnect;
		CTimeIntervalCount  stTimeIntervalCount;
	};

	class CSendMsgUpFlowCtrl: public CFlowCtrlPolicy
	{
	public:
		friend class CFlowCtrl;
		CSendMsgUpFlowCtrl(): m_iMaxSendPkg(1000){}
		
		void SendOneMsg()
		{
			stTimeIntervalCount.Add();
		}
		void SetSendRate(int iIntervalMsec, int iMaxSendPkg)
		{
			stTimeIntervalCount.SetInterval(iIntervalMsec / 1000, iIntervalMsec % 1000);
			m_iMaxSendPkg = iMaxSendPkg;
		}

		bool IsOverflow()
		{
			if(stTimeIntervalCount.GetCount() >= m_iMaxSendPkg)
			{	
				m_bIsOverflow = true;
			}	
			else
			{
				m_bIsOverflow = false;
			}

			return CFlowCtrlPolicy::IsOverflow();
		}

	private:
		int						m_iMaxSendPkg;
		CTimeIntervalCount		stTimeIntervalCount;
	};

	class CFlowCtrl: public CFlowCtrlPolicy
	{
	public:
		CFlowCtrl()
		{
			RegistOtherCtrl(&m_stAcceptNumFlowCtrl);
			RegistOtherCtrl(&m_stAcceptRateFlowCtrl);
			RegistOtherCtrl(&m_stStopIntervalFlowCtrl);
			RegistOtherCtrl(&m_stSendMsgUpFlowCtrl);
		}
		~CFlowCtrl()
		{

		}

		int Init(int iMaxConnect, int iNewConnCheckInterval, int iNewConnMax, int iSendUpCheckInterval, int iSendUpMax, int iStopAcceptInterval)
		{
			m_stAcceptNumFlowCtrl.SetMaxAcceptCount(iMaxConnect);
			m_stAcceptRateFlowCtrl.SetAcceptRate(iNewConnCheckInterval, iNewConnMax);
			m_stStopIntervalFlowCtrl.SetStopInterval(iStopAcceptInterval);
			m_stSendMsgUpFlowCtrl.SetSendRate(iSendUpCheckInterval, iSendUpMax);

			return 0;
		}

		//接收一个连接
		void AcceptConnect()
		{
			m_stAcceptNumFlowCtrl.AddAcceptCount();
			m_stAcceptRateFlowCtrl.AcceptNewConnect();
		}

		//关闭一个连接
		void ClientDisconnect()
		{
			m_stAcceptNumFlowCtrl.DelAcceptCount();
		}

		//停止接收一段时间
		void StopAccept()
		{
			m_stStopIntervalFlowCtrl.Stop();
		}

		///向上层发送了一个请求
		void SendOneMsgUp()
		{
			m_stSendMsgUpFlowCtrl.SendOneMsg();
		}

		///获取当前状态信息
		const char* GetStat()
		{
			m_stStatBuf.Set("connected %d/%d, accept rate %d/%d, sendup rate %d/%d, stop stat %d",
				m_stAcceptNumFlowCtrl.m_iAcceptCount, m_stAcceptNumFlowCtrl.m_iMaxAcceptCount,
				m_stAcceptRateFlowCtrl.stTimeIntervalCount.GetCount(), m_stAcceptRateFlowCtrl.m_iMaxConnect,
				m_stSendMsgUpFlowCtrl.stTimeIntervalCount.GetCount(), m_stSendMsgUpFlowCtrl.m_iMaxSendPkg,
				m_stStopIntervalFlowCtrl.m_bIsOverflow);

			return m_stStatBuf.Get();
		}
	private:
		CAcceptNumFlowCtrl		m_stAcceptNumFlowCtrl;
		CAcceptRateFlowCtrl		m_stAcceptRateFlowCtrl;
		CSendMsgUpFlowCtrl		m_stSendMsgUpFlowCtrl;
		CStopIntervalFlowCtrl	m_stStopIntervalFlowCtrl;

		CSizeString<2048> m_stStatBuf;		///存下当前流量信息
	};

}
#endif