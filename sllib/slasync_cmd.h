//���ﶨ�����첽������
/********************************************************************
	created:	2015/12/15
	created:	15:12:2015   19:42
	filename: 	d:\workspace\shyloo\sllib\slasync_cmd.h
	file path:	d:\workspace\shyloo\sllib
	file base:	slasync_cmd
	file ext:	h
	author:		ddc
	
	purpose:	�첽������
*********************************************************************/
#ifndef _SL_ASYNC_CMD_H_
#define _SL_ASYNC_CMD_H_
#include "slasync_cmd_factory.h"
#include "slmsg_base.h"
#include "slbase.h"

namespace sl
{
	//�첽�������
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
			SL_TRACE("AsyncCmd(%p) free. parent=%p, child=%p, seq=%d", this, m_pstParent, m_pstChild, GetCmdSeq());
			//����Լ������������ͷ�(��ʱ������������)����ô���������pParent��Ϊ0
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

		//�ڵ���������Do֮ǰ��RegChild
		void RegisterChild(CAsyncCmd* pChildCmd, bool bParentEnableLogicDo = false)
		{
			SL_ASSERT(pChildCmd);
			SL_TRACE("AsyncCmd(%d, %p) register child(%d, %p)",
				GetCmdID(), this, pChildCmd->GetCmdID(), pChildCmd);
			m_iState = pChildCmd->GetCmdID();
			pChildCmd->m_pstParent = this;
			this->m_pstChild = pChildCmd;
			pChildCmd->m_bParentEnableLogicDo = bParentEnableLogicDo;
			IncInf();
		}
		//����ִ����Ϻ����Done�������Ա��Զ��ͷŻ�ص�������
		virtual int Done(int iRet)
		{
			SL_TRACE("AsyncCmd(%d, %p) done, ret = %d", GetCmdID(), this, iRet);
			if(m_pstParent)
			{
				SL_TRACE("AsyncCmd(%d, %p) done, it's parent is (%d, %p)", GetCmdID(), this, m_pstParent->GetCmdID(), m_pstParent);
				m_pstParent->DecInf();
				m_pstParent->LoginDo(iRet);
				m_pstParent = NULL;
				m_bParentEnableLogicDo = false;
			}

			//�ͷ��Լ�
			int _iRet = SL_CMDFACTORY->FreeCmd(this);
			if(_iRet)
			{
				SL_WARNING("freecmd(%d, %p) failed, ret=%d",GetCmdID(),this, _iRet);
			}
			return iRet;
		}

		virtual bool HasChild()
		{
			return (m_pstChild != NULL && GetRef() > 0);
		}

		//���Լ��ŵ�CAsyncCmdQueue��
		int PushQueue(int iRet)
		{
			m_iQueueRet = iRet;
			m_iState = GetCmdID();
			IncInf();
			return SL_CMDFACTORY->PushQueue(this);
		}

		//�������ʼʱ��
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
		//Ĭ�ϵ���callback
		virtual int LoginDo(int iRet)
		{
			return Done(iRet);
		}

		/*
			���Ӹ������ִ��ʱ��
		*/
		virtual int AddExecTime(double dExecTime)
		{
			if(dExecTime > 0)
			{
				//�����Լ���ִ��ʱ��
				m_dCmdExecTime += dExecTime;

				//����Լ��и����Ҳ���Ӹ������ִ��ʱ��
				if(m_pstParent)
				{
					m_pstParent->AddExecTime(dExecTime);
				}
			}

			//���һ�ν�����ɣ����ÿ�ʼռ��cpu��ʱ��
			m_stBeginExecTime.tv_sec = 0;
			m_stBeginExecTime.tv_usec = 0;
			return 0;
		}

		/*
			���ÿ�ʼִ�е�ʱ�䣬���ʱ��Ϊ�����ͷ�ǰ�����Լ�ʹ���˶���ʱ��׼����
		*/
		virtual int SetBeginExecTime(timeval stBeginExecTime)
		{
			m_stBeginExecTime = stBeginExecTime;
			//����Լ��и����Ҳ���ø�����Ŀ�ʼִ��ʱ�䣬�Ա㸸������Done��ʱ������Լ�
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
		int					m_iCmdCreateTime;			//��������ʱ��
		bool				m_bParentEnableLogicDo;

		timeval				m_stBeginTime;
		timeval				m_stEndTime;

		timeval				m_stBeginExecTime;
		double				m_dCmdExecTime;				//�����ִ��ʱ��������Ϊ��λ
	public:
		int					m_iReverse1;				//�����ֶ�
		int					m_iReverse2;				//�����ֶ�

	}; //class CAsyncCmd

	//==========================================================

	//�첽�����Do����
	class CCmdDoParam
	{
	public:
		CNetHead&			m_stHead;			///���紫��ͷ��
		CMsgHead&			m_stMsgHead;		///��Ϣͷ��
		CCodeStream&		m_stCode;			///������
		bool				m_bIsAdmin;			///�Ƿ����Թ���˿�

		CCmdDoParam(CNetHead& stHead, CMsgHead& stMsgHead, CCodeStream& stCode, bool bIsAdmin)
			:m_stHead(stHead), m_stMsgHead(stMsgHead), m_stCode(stCode), m_bIsAdmin(bIsAdmin) {}

		virtual ~CCmdDoParam() {}
	
	private:
		//��ֹ��ֵ
		CCmdDoParam& operator = (const CCmdDoParam& obj) {return *this;}

	}; //class CCmdDoParam

	//�첽�����OnAnswer����
	class CCmdOnAnswerParam
	{
	public:
		CMsgHead&			m_stMsgHead;
		CCodeStream&		m_stCode;

		CCmdOnAnswerParam(CMsgHead& stMsgHead, CCodeStream& stCode)
			:m_stMsgHead(stMsgHead), m_stCode(stCode) {}

		virtual ~CCmdOnAnswerParam() {}
	
	private:
		//��ֹ��ֵ
		CCmdOnAnswerParam& operator = (const CCmdOnAnswerParam& obj) { return *this;}


	}; //class CCmdOnAnswerParam

	///�������ݵ�״̬
	enum EnumLoadDataResult
	{
		ELDR_NEED_LOAD		=	0,			///< ��δ����
		ELDR_LOADING		=	1,			///< ������
		ELDR_LOAD_SUCCESS	=	2,			///< ���سɹ�
		ELDR_LOAD_FAILED	=	3,			///< ����ʧ��
		ELDR_LOAD_EMPTY		=	4,			///< ����Ϊ��
	};

	//��������״̬
	class CLoadDataStat
	{
	public:
		CLoadDataStat(): m_Stat(ELDR_NEED_LOAD){}

		inline void SetLoadStat(EnumLoadDataResult stStat) {m_Stat = stStat;}
		inline EnumLoadDataResult GetLoadStat() const {return m_Stat;}
		inline bool IsNeedLoad() const {return m_Stat == ELDR_NEED_LOAD;}
		inline bool IsLoading() const {return m_Stat == ELDR_LOADING;}
		inline bool IsLoadSuccess() const {return m_Stat == ELDR_LOAD_SUCCESS;}
		inline bool IsLoadFailed() const {return m_Stat == ELDR_LOAD_FAILED;}
		inline bool IsLoadEmpty() const {return m_Stat == ELDR_LOAD_EMPTY;}
	
	protected:
		short	m_Stat;			///< ����״̬
		
	};

} //namespace sl
#endif