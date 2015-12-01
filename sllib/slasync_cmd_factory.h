//�첽�������
#ifndef _SL_ASYNC_CMD_FACTORY_H_
#define _SL_ASYNC_CMD_FACTORY_H_
#include "slmulti_object_mgr.h"
#include "slsingleton.h"
#include "slmsg_base.h"
#include "slarray.h"
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
					return -1;
				}
				else
				{

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
				//return;
			}
			m_stMap[iCmdID] = p;
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
						return NULL;
					}
					pszBuffer = new char[p->GetObjectSize()];
					if(!pszBuffer)
					{
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
					if(m_cBufMgr[i].m_iCmdID == pstCmd->m_CmdID)
					{
						delete m_cBufMgr[i].m_pBuf;
						m_cBufMgr.DelOneItem(i);
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