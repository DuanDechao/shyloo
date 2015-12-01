//这里定义了异步命令框架
#ifndef _SL_ASYNC_CMD_H_
#define _SL_ASYNC_CMD_H_
#include "slasync_cmd_factory.h"
#include "slmsg_base.h"

namespace sl
{
	//异步命令基类
	class CAsyncCmd: public CAsyncCmdInf
	{

	public:
		CAsyncCmd()
			:m_pstParent(NULL),
			 m_pstChild(NULL),
			 m_iState(0),
			 m_iCmdCreateTime(0),
			 m_bParentEnableLogicDo(false),
			 m_dCmdExecTime(0),
			 m_iReverse1(0),
			 m_iReverse2(0)
		{
			m_stBeginTime.tv_sec  = 0;
			m_stBeginTime.tv_usec = 0;
			m_stEndTime.tv_sec	  = 0;
			m_stEndTime.tv_usec   = 0;
		}

		virtual ~CAsyncCmd()
		{
			//如果自己比子命令先释放(超时会出现这种情况)，那么把子命令的pParent设为0
			if(m_pstChild != NULL && m_pstChild->m_pstParent == this)
			{
				m_pstChild->m_pstParent = NULL;
			}
			if(m_pstParent != NULL && m_pstParent->m_pstChild == this)
			{
				m_pstParent->m_pstChild = NULL;
			}
			m_bParentEnableLogicDo = false;
		}

		//在调用子命令Do之前先RegChild
		void RegisterChild(CAsyncCmd* pChildCmd, bool bParentEnableLogicDo = false)
		{
			m_iState = pChildCmd->GetCmdID();
			pChildCmd->m_pstParent = this;
			this->m_pstChild = pChildCmd;
			pChildCmd->m_bParentEnableLogicDo = bParentEnableLogicDo;
			IncInf();
		}
		//命令执行完毕后调用Done函数，以便自动释放或回调父命令
		virtual int Done(int iRet)
		{
			if(m_pstParent)
			{
				m_pstParent->DecInf();
				m_pstParent->LoginDo(iRet);
				m_pstParent = NULL;
				m_bParentEnableLogicDo = false;
			}

			//释放自己
			int i = SL_CMDFACTORY->FreeCmd(this);
			if(i)
			{

			}
			return iRet;
		}

		virtual bool HasChild()
		{
			return (m_pstChild != NULL && GetRef() > 0);
		}

		//设置命令开始时间
		virtual void SetCmdCreateTime(int iNow = -1)
		{
			if(iNow == -1)
			{
				m_iCmdCreateTime = static_cast<int>(time(NULL));
			}
			else
			{
				m_iCmdCreateTime = iNow;
			}
		}
		virtual int GetCmdCreateTime()
		{
			return m_iCmdCreateTime;
		}
	public:
		//默认调用callback
		virtual int LoginDo(int iRet)
		{
			return Done(iRet);
		}

		/*
			增加该命令的执行时间
		*/
		virtual int AddExecTime(double dExecTime)
		{
			if(dExecTime > 0)
			{
				//增加自己的执行时间
				m_dCmdExecTime += dExecTime;

				//如果自己有父命令，也增加父命令的执行时间
				if(m_pstParent)
				{
					m_pstParent->AddExecTime(dExecTime);
				}
			}
		}

		/*
			设置开始执行的时间，这个时间为命令释放前分析自己使用了多少时间准备的
		*/
		virtual int SetBeginExecTime(timeval stBeginExecTime)
		{
			m_stBeginExecTime = stBeginExecTime;
			//如果自己有父命令，也设置父命令的开始执行时间，以便父命令在Done的时候结算自己
			if(m_pstParent)
			{
				m_pstParent->SetBeginExecTime(stBeginExecTime);
			}
			return 0;
		}
	protected:
		CAsyncCmd*			m_pstParent;
		CAsyncCmd*			m_pstChild;
		int					m_iState;
		int					m_iCmdCreateTime;			//命令生成时间
		bool				m_bParentEnableLogicDo;

		timeval				m_stBeginTime;
		timeval				m_stEndTime;

		timeval				m_stBeginExecTime;
		double				m_dCmdExecTime;				//命令的执行时长，以秒为单位
	public:
		int					m_iReverse1;				//保留字段
		int					m_iReverse2;				//保留字段

	}; //class CAsyncCmd

	//==========================================================

	//异步命令的Do参数
	class CCmdDoParam
	{
	public:
		CNetHead&			m_stHead;			///网络传输头部
		CMsgHead&			m_stMsgHead;		///消息头部
		CCodeStream&		m_stCode;			///数据区
		bool				m_bIsAdmin;			///是否来自管理端口

		CCmdDoParam(CNetHead& stHead, CMsgHead& stMsgHead, CCodeStream& stCode, bool bIsAdmin)
			:m_stHead(stHead), m_stMsgHead(stMsgHead), m_stCode(stCode), m_bIsAdmin(bIsAdmin) {}

		virtual ~CCmdDoParam() {}
	
	private:
		//禁止赋值
		CCmdDoParam& operator = (const CCmdDoParam& obj) {return *this;}

	}; //class CCmdDoParam

	//异步命令的OnAnswer参数
	class CCmdOnAnswerParam
	{
	public:
		CMsgHead&			m_stMsgHead;
		CCodeStream&		m_stCode;

		CCmdOnAnswerParam(CMsgHead& stMsgHead, CCodeStream& stCode)
			:m_stMsgHead(stMsgHead), m_stCode(stCode) {}

		virtual ~CCmdOnAnswerParam() {}
	
	private:
		//禁止赋值
		CCmdOnAnswerParam& operator = (const CCmdOnAnswerParam& obj) { return *this;}


	}; //class CCmdOnAnswerParam

} //namespace sl
#endif