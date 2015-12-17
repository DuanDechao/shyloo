//�첽�������
/********************************************************************
	created:	2015/12/15
	created:	15:12:2015   16:29
	filename: 	d:\workspace\shyloo\sllib\slasync_cmd_factory.h
	file path:	d:\workspace\shyloo\sllib
	file base:	slasync_cmd_factory
	file ext:	h
	author:		ddc
	
	purpose:	�첽�������
*********************************************************************/
#ifndef _SL_ASYNC_CMD_FACTORY_H_
#define _SL_ASYNC_CMD_FACTORY_H_
#include "slmulti_object_mgr.h"
#include "slsingleton.h"
#include "slmsg_base.h"
#include "slarray.h"
#include "slexception.h"
#include <map>

namespace sl
{
	//�첽�������Ľӿ�
	class CAsyncCmdInf
	{
		friend class CAsyncCmdFactory;
	
	public:
		CAsyncCmdInf()
			:m_CmdID(0),
			 m_iCmdSeq(0),
			 m_shRef(0),
			 m_iQueueRet(0),
			 ulOwnerThread(0),
			 m_bufSty(false)
		{}

		virtual ~CAsyncCmdInf() {}

		MsgID_t GetCmdID() const
		{
			return m_CmdID;
		}
		int GetCmdSeq() const
		{
			return m_iCmdSeq;
		}
		const CMgrIndex& GetMgrIndex() const
		{
			return m_stIndex;
		}

	public:
		/*
			���������ִ��Do����
			@param [in] pData  ��void*������
		*/
		virtual int Do(void* pData)
		{
			return 0;
		}

		/*
			���û��������ⲿ�յ���Ӧ����������OnAnswer����
			@param [in] pData  ��void* ������
		*/
		virtual int OnAnswer(void* pData)
		{
			return 0;
		}

		//�������
		virtual int Done(int iRet)
		{
			return 0;
		}

		/*
			�����������Զ���������ش�����
			@param [in] iRet	������ִ�еķ���ֵ
		*/
		virtual int LogicDo(int iRet)
		{
			return 0;
		}

		//�������ʼʱ��
		virtual void SetCmdCreateTime(int iNow = -1)
		{

		}
		virtual int GetCmdCreateTime()
		{
			return 0;
		}

		virtual bool HasChild()
		{
			return false;
		}

		virtual int AddExecTime(double dExecTime)
		{
			return 0;
		}

		virtual int SetBeginExecTime(timeval stBeginExecTime)
		{
			return 0;
		}
	
	protected:
		//�������ü���
		short GetRef() const
		{
			return m_shRef;
		}
		//���ü�����1
		short IncInf()
		{
			return ++m_shRef;
		}

		//���ü�����1
		short DecInf()
		{
			return --m_shRef;
		}

	private:
		MsgID_t			m_CmdID;
		int				m_iCmdSeq;
		short			m_shRef;
		CMgrIndex		m_stIndex;
	
	protected:
		int				m_iQueueRet;

	public:
		unsigned int	ulOwnerThread;		///< �̺߳�
		bool			m_bufSty;			///< ��¼�����ڴ�ķ�ʽ��Ĭ��false��true��ʾnew

	}; //class CAsyncCmdInf

	//�첽����Ĺ���ע�����
	class CAsyncCmdRegisterInf
	{
	public:
		CAsyncCmdRegisterInf() : m_iMgrIndex(-1) {}
		virtual ~CAsyncCmdRegisterInf() {}

		virtual int GetObjectSize() const  = 0;
		virtual CAsyncCmdInf* CreateObject(char* pszBuffer) const = 0;
	public:
		int m_iMgrIndex;
	};

	class CBufNewInfo
	{
	public:
		CBufNewInfo():	m_pBuf(NULL), m_iCmdID(0) {}
	public:
		char*		m_pBuf;
		int			m_iCmdID;
	};

	//�첽�����
	class CAsyncCmdFactory
	{
	public:
		typedef map<int, CAsyncCmdRegisterInf*> TRegisterMap;
	protected:
		enum
		{
			LIST_COUNT		=	2,
			INDEX_COUNT		=	3,

			ALLOC_INDEX		=	0,
			FREE_LIST		=	0,
			USED_LIST		=   1,

			QUEUE_INDEX		=   1,
			QUEUE_LIST		=	1,
			
			MAX_DOQUEUE		=	1024,  ///< Ĭ��һ�����ִ�еĶ�������
			MAX_CMD_TIMEOUT	=	15,	   ///< ���ʱʱ�䣨�룩

			BUF_NEW_COUNT	=	300,   ///< �ڴ�ز��㣬new�ڴ���������
		};
		int					m_iCmdSeq;
		TRegisterMap		m_stMap;
		CMultiObjectMgr		m_stMgr;

		CArray<CBufNewInfo, BUF_NEW_COUNT> m_cBufMgr;

	public:
		CAsyncCmdFactory(): m_iCmdSeq(0) {}
		virtual ~CAsyncCmdFactory() {}

		int Init(const char* pszMgrConfig, const char* pszBuffer, int iBufferSize)
		{
			int iRet = 0;
			m_cBufMgr.Clear();
			iRet = m_stMgr.Init(pszMgrConfig, pszBuffer, iBufferSize, LIST_COUNT, INDEX_COUNT);
			if(iRet)
			{
				return iRet;
			}
			
			TRegisterMap::iterator iter;
			int iMgrIndex = 0;
			for (iter = m_stMap.begin(); iter != m_stMap.end(); ++iter)
			{
				iMgrIndex	=	m_stMgr.CalcMgrIndex(iter->second->GetObjectSize());
				if(iMgrIndex < 0)
				{
					SL_ERROR("RegisterCmd fail, cmdid(%d)'s size(%d) is too large", iter->first, iter->second->GetObjectSize());
					return -1;
				}
				else
				{
					SL_INFO("Register Cmdid = %d, size = %d, mgr index = %d", iter->first, iter->second->GetObjectSize(), iMgrIndex);
				}
				iter->second->m_iMgrIndex = iMgrIndex;
			}
			return 0;
		}

		static size_t CountSize(const char* pszMgrConfig)
		{
			return CMultiObjectMgr::CountSize(pszMgrConfig, LIST_COUNT, INDEX_COUNT);
		}

		///===============================================
		//�첽�����ע��/����/�ͷ�
		/*
			ע���첽����
		*/
		void RegisterCmd(int iCmdID, CAsyncCmdRegisterInf* p)
		{
			TRegisterMap::iterator it = m_stMap.find(iCmdID);
			if(it != m_stMap.end())
			{
				SL_THROW("cmdid(%d) already exists", iCmdID);
				//return;
			}
			m_stMap[iCmdID] = p;

			SL_INFO("CmdID = %d, cmd size = %d, mgr index = %d", iCmdID, p->GetObjectSize(), m_stMap[iCmdID]->m_iMgrIndex);
		}

		/*
			����iCmdID�����첽����
		*/
		CAsyncCmdInf* CreateCmd(MsgID_t iCmdID)
		{
			TRegisterMap::iterator iter = m_stMap.find(iCmdID);
			if(iter != m_stMap.end())
			{
				CAsyncCmdRegisterInf* p = iter->second;
				SL_ASSERT(p && p->m_iMgrIndex >= 0 && p->m_iMgrIndex <(int)m_stMgr.GetMgrCount());

				CMgrIndex stIndex;
				char* pszBuffer = m_stMgr.Alloc(p->m_iMgrIndex, &stIndex);

				CBufNewInfo	bufModul;
				if(!pszBuffer)
				{
					if(m_cBufMgr.m_iUsedCount == BUF_NEW_COUNT)
					{
						SL_ERROR("cmd factory buf mgr usecount full, alloc cmd %d failed", iCmdID);
						return NULL;
					}
					SL_WARNING("alloc buffer fail, mgrindex = %d, cmdid = %d, alloc buf buy new", p->m_iMgrIndex, iCmdID);
					pszBuffer = new char[p->GetObjectSize()];
					if(!pszBuffer)
					{
						SL_WARNING("alloc buf by new for cmd %d failed", iCmdID);
						return NULL;
					}
					bufModul.m_pBuf    =  pszBuffer;
					bufModul.m_iCmdID  =  iCmdID;
					m_cBufMgr.AddOneItem(bufModul);
				}

				CAsyncCmdInf* pstCmd  =  p->CreateObject(pszBuffer);
				pstCmd->m_CmdID		  =  iCmdID;
				pstCmd->m_iCmdSeq	  =  GetCmdSeq();
				pstCmd->m_stIndex	  =  stIndex;
				pstCmd->SetCmdCreateTime();
				if(bufModul.m_pBuf)  pstCmd->m_bufSty  =  true;
				SL_TRACE("Create AsybcCmd %d(%p)", iCmdID, pstCmd);
				return pstCmd;
			}
			return NULL;
		}

		/*
			�ͷ��첽����
		*/
		int FreeCmd(CAsyncCmdInf* pstCmd)
		{
			if(pstCmd->m_bufSty == true)
			{
				for (int i = 0; i < m_cBufMgr.m_iUsedCount; i++)
				{
					if(m_cBufMgr[i].m_iCmdID == pstCmd->GetCmdID())
					{
						delete m_cBufMgr[i].m_pBuf;
						m_cBufMgr.DelOneItem(i);
						SL_TRACE("delete buf in m_cBufMgr index %d cmdid %d", i, pstCmd->GetCmdID());
						return 0;
					}
				}
				
			}
			CMgrIndex stIndex;
			if(!pstCmd || !m_stMgr.IsValidObject((const char*)pstCmd, &stIndex))
			{
				return -1;
			}
			pstCmd->~CAsyncCmdInf();
			m_stMgr.Free(stIndex);
			return 0;
		}

		bool IsValidCmd(CAsyncCmdInf* pstCmd, CMgrIndex* pstIndex = NULL)
		{
			return m_stMgr.IsValidObject((char*)pstCmd, pstIndex);
		}

		///-1��ʾ����ȫ�ֲ����жϣ�����ID
		bool IsBusy(int iMaxPercent)
		{
			if(iMaxPercent <= 0)
			{
				return true;
			}
			for (unsigned int i = 0; i < m_stMgr.GetMgrCount(); ++i)
			{
				int iTotalCount = m_stMgr.GetMgrObjectCount(i);
				if(iTotalCount <= 0)
				{
					return true;
				}

				int64 i64Temp = m_stMgr.Size(i, ALLOC_INDEX, USED_LIST) * 100;
				if(i64Temp / iTotalCount >= iMaxPercent)
				{
					return true;
				}

			}
			return false;
		}

		void DumpStatInfo(CLog* pstLog)
		{
			for(unsigned int i = 0; i< m_stMgr.GetMgrCount(); ++i)
			{
				pstLog->Log(EInfo, "Cmds_Lv%d: All=%d Used=%d Free=%d Queue=%d", i+1,
					m_stMgr.GetMgrObjectCount(i),
					m_stMgr.Size(i, ALLOC_INDEX, USED_LIST),
					m_stMgr.Size(i, ALLOC_INDEX, FREE_LIST),
					m_stMgr.Size(i, QUEUE_INDEX, QUEUE_LIST));
			}
		}

		void DumpStatDetail(CLog* pstLog)
		{
			pstLog->Log(EInfo, "======================== Async Cmds Detail ===================");
			CMgrIndex stFirst, stIndex, stNext;
			m_stMgr.GetHead(stFirst, USED_LIST, ALLOC_INDEX);

			if(stFirst.IsNull())
			{
				pstLog->Log(EInfo, "No Used Cmd");
				return;
			}

			stIndex = stFirst;
			for (; !stIndex.IsNull(); stIndex = stNext)
			{
				m_stMgr.GetNext(stIndex, stNext, ALLOC_INDEX);
				pstLog->Log(EInfo, "Cmd MsgId %d", ((CAsyncCmdInf*)(m_stMgr.Get(stIndex)))->GetCmdID());
			}
		}

		//=================================
		// ������еĲ���

		///������ŵ�������
		int PushQueue(CAsyncCmdInf* pstCmd)
		{
			CMgrIndex stIndex;
			if(!m_stMgr.IsValidObject((const char*)pstCmd, &stIndex))
			{
				return -1;
			}
			return m_stMgr.Move(stIndex, QUEUE_INDEX, QUEUE_LIST);
		}

		/*
			�Ѷ����е�����������ִ��
			@param [in] iMaxDo ���ִ�ж��ٸ���������
			@return ����ִ���˶��ٸ���������
		*/
		int DoQueue(int iMaxDo = MAX_DOQUEUE)
		{
			int iCount = 0;
			CMgrIndex stFirst, stIndex, stNext;
			m_stMgr.GetHead(stFirst, QUEUE_LIST, QUEUE_INDEX);

			//��������е�ĳ������ִ�к󻹻�����ŵ������У���Ҫ
			//��!(iCount >0 && stIndex != stFirst) ���������������ɵ���Чѭ��
			for(iCount = 0, stIndex = stFirst;
				iCount < iMaxDo && !stIndex.IsNull() && !(iCount >0 && stIndex != stFirst);
				++iCount, stIndex = stNext)
			{
				m_stMgr.GetNext(stIndex, stNext, QUEUE_INDEX);

				CAsyncCmdInf* pstCmd = (CAsyncCmdInf*) m_stMgr.Get(stIndex);
				SL_TRACE("exec queue AsyncCmd(%d, %p)(%d)", pstCmd->GetCmdID(), pstCmd, pstCmd->m_iQueueRet);

				pstCmd->DecInf();
				pstCmd->LogicDo(pstCmd->m_iQueueRet);
				if(pstCmd->GetRef() <= 0)
				{
					FreeCmd(pstCmd);
				}
			}

			return iCount;
		}

		//��鳬ʱ������
		int CheckTimeoutCmd()
		{
			int iCount = 0;
			const int FREE_CMD_PER_CHECK = 100;
			CArray<CAsyncCmdInf*, FREE_CMD_PER_CHECK> arpCmdNeedFree;

			CMgrIndex stFirst, stIndex, stNext;
			m_stMgr.GetHead(stFirst, USED_LIST, ALLOC_INDEX);

			int iNow  = static_cast<int>(time(NULL));
			///��������е�ĳ������ִ�к󻹻�������������
			for (iCount = 0, stIndex = stFirst; !stIndex.IsNull() && iCount < FREE_CMD_PER_CHECK; stIndex = stNext)
			{
				m_stMgr.GetNext(stIndex, stNext, ALLOC_INDEX);

				CAsyncCmdInf* pstCmd = (CAsyncCmdInf*) m_stMgr.Get(stIndex);
				if(!pstCmd->HasChild() && iNow - pstCmd->GetCmdCreateTime() > MAX_CMD_TIMEOUT)
				{
					arpCmdNeedFree[iCount] = pstCmd;			//���������й����ԣ����Խ����ͷŵ������ȴ�������֮��ͳһ�ͷ�
					arpCmdNeedFree.m_iUsedCount = iCount + 1;
					iCount++;
				}
			}

			//�ͷ�����
			for (int i = 0; i< arpCmdNeedFree.m_iUsedCount; i++)
			{
				SL_WARNING("find cmd (%d, %p) timeout, do Done()", arpCmdNeedFree[i]->GetCmdID(), arpCmdNeedFree[i]);
				arpCmdNeedFree[i]->Done(10);
			}

			arpCmdNeedFree.m_iUsedCount = 0;
			return iCount;
		}

	protected:
		int GetCmdSeq()
		{
			if(m_iCmdSeq >= SL_INT_MAX || m_iCmdSeq < 0)
			{
				m_iCmdSeq = 0;
			}
			return ++m_iCmdSeq;
		}
	}; ///class CAsyncCmdFactory

	#define SL_CMDFACTORY  (sl::CSingleton<CAsyncCmdFactory>::Instance())

	//�첽����Ĺ���ע����
	template<typename T>
	class CAsyncCmdRegister: public CAsyncCmdRegisterInf
	{
	public:
		CAsyncCmdRegister(int iCmdID, const char* pszClassName)
		{
			SL_TRACE("register cmd id(%d), class(%s)!", iCmdID, pszClassName);
			SL_CMDFACTORY->RegisterCmd(iCmdID, this);
		}
		int GetObjectSize() const
		{
			return sizeof(T);
		}
		CAsyncCmdInf* CreateObject(char* pszBuffer) const
		{
			return new (pszBuffer) T;
		}
	};

	//��ע��ĺ�
	#define SL_REGISTER_ASYNCCMD(CmdID, Class) \
	static const CAsyncCmdRegister<Class> CAsyncCmdRegister##Class(CmdID, #Class)


}// namespace sl
#endif