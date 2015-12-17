//异步命令工厂类
/********************************************************************
	created:	2015/12/15
	created:	15:12:2015   16:29
	filename: 	d:\workspace\shyloo\sllib\slasync_cmd_factory.h
	file path:	d:\workspace\shyloo\sllib
	file base:	slasync_cmd_factory
	file ext:	h
	author:		ddc
	
	purpose:	异步命令工厂类
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
	//异步命令基类的接口
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
			生成命令后执行Do函数
			@param [in] pData  用void*做参数
		*/
		virtual int Do(void* pData)
		{
			return 0;
		}

		/*
			如果没有子命令，外部收到响应后调用命令的OnAnswer函数
			@param [in] pData  用void* 做参数
		*/
		virtual int OnAnswer(void* pData)
		{
			return 0;
		}

		//命令完成
		virtual int Done(int iRet)
		{
			return 0;
		}

		/*
			该命令用于自定义子命令返回处理函数
			@param [in] iRet	子命令执行的返回值
		*/
		virtual int LogicDo(int iRet)
		{
			return 0;
		}

		//设置命令开始时间
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
		//返回引用计数
		short GetRef() const
		{
			return m_shRef;
		}
		//引用计数加1
		short IncInf()
		{
			return ++m_shRef;
		}

		//引用计数减1
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
		unsigned int	ulOwnerThread;		///< 线程号
		bool			m_bufSty;			///< 记录分配内存的方式，默认false，true表示new

	}; //class CAsyncCmdInf

	//异步命令的工厂注册基类
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

	//异步命令工厂
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
			
			MAX_DOQUEUE		=	1024,  ///< 默认一次最多执行的队列命令
			MAX_CMD_TIMEOUT	=	15,	   ///< 命令超时时间（秒）

			BUF_NEW_COUNT	=	300,   ///< 内存池不足，new内存块对象数量
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
		//异步命令的注册/创建/释放
		/*
			注册异步命令
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
			根据iCmdID创建异步命令
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
			释放异步命令
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

		///-1表示按照全局测试判断，其他ID
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
		// 命令队列的操作

		///把命令放到队列中
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
			把队列中的命令拉出来执行
			@param [in] iMaxDo 最多执行多少个队列命令
			@return 返回执行了多少个队列命令
		*/
		int DoQueue(int iMaxDo = MAX_DOQUEUE)
		{
			int iCount = 0;
			CMgrIndex stFirst, stIndex, stNext;
			m_stMgr.GetHead(stFirst, QUEUE_LIST, QUEUE_INDEX);

			//如果队列中的某个命令执行后还会继续放到队列中，需要
			//用!(iCount >0 && stIndex != stFirst) 来避免这种情况造成的无效循环
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

		//检查超时的命令
		int CheckTimeoutCmd()
		{
			int iCount = 0;
			const int FREE_CMD_PER_CHECK = 100;
			CArray<CAsyncCmdInf*, FREE_CMD_PER_CHECK> arpCmdNeedFree;

			CMgrIndex stFirst, stIndex, stNext;
			m_stMgr.GetHead(stFirst, USED_LIST, ALLOC_INDEX);

			int iNow  = static_cast<int>(time(NULL));
			///如果队列中的某个命令执行后还会继续放入队列中
			for (iCount = 0, stIndex = stFirst; !stIndex.IsNull() && iCount < FREE_CMD_PER_CHECK; stIndex = stNext)
			{
				m_stMgr.GetNext(stIndex, stNext, ALLOC_INDEX);

				CAsyncCmdInf* pstCmd = (CAsyncCmdInf*) m_stMgr.Get(stIndex);
				if(!pstCmd->HasChild() && iNow - pstCmd->GetCmdCreateTime() > MAX_CMD_TIMEOUT)
				{
					arpCmdNeedFree[iCount] = pstCmd;			//由于命令有关联性，所以将待释放的命令先存起来，之后统一释放
					arpCmdNeedFree.m_iUsedCount = iCount + 1;
					iCount++;
				}
			}

			//释放命令
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

	//异步命令的工厂注册类
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

	//简化注册的宏
	#define SL_REGISTER_ASYNCCMD(CmdID, Class) \
	static const CAsyncCmdRegister<Class> CAsyncCmdRegister##Class(CmdID, #Class)


}// namespace sl
#endif