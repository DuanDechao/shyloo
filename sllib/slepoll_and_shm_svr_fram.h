//����epoll��shm�����ڴ���Ϊ����ͨ·��
#ifndef _LZ_EPOLL_AND_SHM_SVR_FRAM_H_
#define _LZ_EPOLL_AND_SHM_SVR_FRAM_H_
#include "slsvr_base_frame.h"
#include "slsize_string.h"
#include "slbase_define.h"
#include "net/slepoll.h"
#include "slshm_queue.h"
#include "slarray.h"
namespace sl
{
	enum enumEpollDef
	{
		EPOLL_SIZE				=		1024,
		EPOLL_WAIT_SIZE			=		1024,
		EPOLL_DEF_SLEEP_TIMER	=		10,      ///< Ĭ��epoll�ȴ�ʱ��Ϊ10ms
		EPOLL_SHM_MAX			=		20,
	};

	typedef CSizeString<SL_PATH_MAX> CDPParaStr;

	class CEpollAndShmSvr;
	typedef CEpollObject<CEpollAndShmSvr> CEpollObj;
	typedef CShmQueue<CEpollAndShmSvr> CShmQ;
	typedef CShmQ* PCShmQ;

	typedef void (*PDATA_EVENT)(unsigned int uiPathKey, int iEvent);

	class CShmData;

	class CDataPathPara
	{
	public:
		CDataPathPara()
			:m_uiPathKey(0),
			 m_uiStreamSize(0),
			 m_bIsInit(false),
			 m_bIsShmHead(false),
			 m_stEventFunc(NULL)
		{}

		CDataPathPara(unsigned int uiPathKey,
			bool bIsHead,
			const char* szStreamKey,
			unsigned int uiStreamSize,
			const char* szFrontSock,
			const char* szBackSock,
			PDATA_EVENT pEventFunc)
			:m_uiPathKey(uiPathKey),
			 m_uiStreamSize(uiStreamSize),
			 m_bIsInit(true),
			 m_bIsShmHead(bIsHead),
			 m_stEventFunc(pEventFunc)
		{
			m_szStreamKey.Set(szStreamKey);
			m_szFrontSock.Set(szFrontSock);
			m_szBackSock.Set(szBackSock);
		}

		~CDataPathPara(){}

		//�ǲ����ڹ����ڴ�ͷ��
		bool IsHead() {return m_bIsShmHead;}

		//�Ƿ��ʼ����
		bool IsInited() {return m_bIsInit;}

		//��ȡ��������Key
		const char* GetStreamKey() {return m_szStreamKey();}

		unsigned int GetStreamSize() {return m_uiStreamSize;}

		const char* GetFrontSock() {return m_szFrontSock();}
		
		const char* GetBackSock() {return m_szBackSock();}

		unsigned int GetPathKey() {return m_uiPathKey;}

		void DoEvent(int iEvent)
		{
			if(m_stEventFunc)
			{
				(*m_stEventFunc)(m_uiPathKey, iEvent);
			}
		}

		friend class CShmData;
	private:
		unsigned int		m_uiPathKey;		///< ����·����ʶ��
		CDPParaStr			m_szStreamKey;		///< �ܵ�����Key�ļ�
		unsigned int		m_uiStreamSize;		///< �ڴ����ݹܵ���С
		bool				m_bIsInit;			///< �Ƿ񾭹���ʼ��
		bool				m_bIsShmHead;		///< �Ƿ��ڹܵ���ͷ��
		CDPParaStr			m_szFrontSock;		///< ǰ��socket
		CDPParaStr			m_szBackSock;		///< ���socket
		PDATA_EVENT			m_stEventFunc;		///< �¼�����

	}; // class CDataPathPara

	class CShmData
	{
	private:
		CDataPathPara m_stPara;
		PCShmQ m_stShmq;			///< �����ڴ����

	public:
		CShmData()
		{
			m_stShmq = NULL;
		}
		~CShmData()
		{
			if(m_stShmq)
			{
				delete m_stShmq;
			}
		}

		PCShmQ GetShmQue()
		{
			return m_stShmq;
		}

		void SetPara(const CDataPathPara& stPar)
		{
			m_stPara = stPar;
		}

		unsigned int GetPathKey()
		{
			return m_stPara.m_uiPathKey;
		}

		int DoEvent(SOCKET iSocket, int iEvent)
		{
			if(m_stPara.IsInited() == false || m_stShmq == NULL)
			{
				return -1;
			}

			//������Ϣ
			m_stShmq->CleaeNotify();

			m_stPara.DoEvent(iEvent);

			return 0;
		}

		int MakeShm(CEpollAndShmSvr& stSvr, CEpoll& stEpoll, CEpollObj::PF_EPOLL_EVENT pfEvent)
		{
			if(m_stPara.IsInited() == false)
			{
				return -1;
			}

			if(m_stShmq)
			{
				delete m_stShmq;
				m_stShmq = NULL;
			}

			m_stShmq = new CShmQ();
			if(m_stShmq == NULL || m_stPara.IsInited() == false)
			{
				return -1;
			}

			int iRet = m_stShmq->Init(m_stPara.IsHead(),
				m_stPara.GetStreamKey(), 
				m_stPara.GetStreamSize(),
				m_stPara.GetFrontSock(),
				m_stPara.GetBackSock(),
				stEpoll, stSvr, pfEvent);
			
			return iRet; 
		}

	}; /// class CShmData

	typedef CArray<CShmData, EPOLL_SHM_MAX> CShmList; ///< ���ݹܵ�����

	class CEpollAndShmSvr: public CSvrBaseFrame
	{
	private:
		CShmList			m_stShmList;		///< ����ͨ·
		CEpoll				m_stEpoll;			///< epoll������Ϊ�¼�������

		int FindShm(CEpollObj* pstEpoll)
		{
			PCShmQ pstShm = NULL;
			for (int i = 0; i < m_stShmList.Size(); ++i)
			{
				pstShm = m_stShmList[i].GetShmQue();
				if(pstShm == NULL)
				{
					continue;
				}

				if(pstShm->IsMyEpollObj(pstEpoll))
				{
					return i;
				}
			}

			return -1;
		}

	public:
		CEpollAndShmSvr(const char* pszAppName): CSvrBaseFrame(pszAppName) {}
		virtual ~CEpollAndShmSvr() {}

		void EpollEvent(CEpollObj* pstObject, SOCKET iSocket, int iEvent)
		{
			//���¼��ַ�����Ӧ�Ĵ�����
			//���ҵ�����ʱ���shm

			int iIndex = FindShm(pstObject);
			if(iIndex <= -1 || iIndex >= m_stShmList.Size())
			{
				return;
			}

			//�ص���ʵ���¼�������
			m_stShmList[iIndex].DoEvent(iSocket, iEvent);

		}

		int InsertDataPath(CDataPathPara& stPara)
		{
			//������2��Key��ͬ������ͨ��
			//���߹ܵ�Key=0
			CShmData stSData;
			stSData.SetPara(stPara);
			int iIndex = m_stShmList.AddOneItem(stSData);

			return iIndex;
		}
	protected:
		CEpoll& GetEpoll() {return m_stEpoll;}

		void WaitAndEvent()
		{
			m_stEpoll.WaitAndEvent(GetSleepTime());
		}

		//�趨����һ�����ߵ����ʱ������λms
		virtual unsigned int GetSleepTime()
		{
			return EPOLL_DEF_SLEEP_TIMER;
		}

		virtual unsigned int GetEpollSize()
		{
			return EPOLL_SIZE;
		}

		virtual unsigned int GetEpollWaitTime()
		{
			return EPOLL_WAIT_SIZE;
		}
		
		//PCShmQ
		PCShmQ GetShm(unsigned int uiPathKey)
		{
			if(uiPathKey == 0)
			{
				return NULL;
			}

			for (int i = 0; i < m_stShmList.Size(); ++i)
			{
				if(m_stShmList[i].GetPathKey() == uiPathKey)
				{
					return m_stShmList[i].GetShmQue();
				}
			}
			return NULL;

		}

		int InitDataStream()
		{
			int iRet = 0;
			iRet = m_stEpoll.Init(GetEpollSize(), GetEpollWaitTime());

			CEpollObj::PF_EPOLL_EVENT pFuncEvent = &CEpollAndShmSvr::EpollEvent;
			//��ע�������ͨ��ע���epoll����
			for (int i = 0; i < m_stShmList.Size(); ++i)
			{
				iRet = m_stShmList[i].MakeShm(*this, m_stEpoll, pFuncEvent);
			}
		}
		
	};
	

} //namespace sl
#endif