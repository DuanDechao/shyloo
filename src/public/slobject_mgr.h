///Object��Buffer������������أ�
/********************************************************************
	created:	2015/12/05
	created:	5:12:2015   15:03
	filename: 	e:\myproject\shyloo\sllib\slobject_mgr.h
	file path:	e:\myproject\shyloo\sllib
	file base:	slobject_mgr
	file ext:	h
	author:		ddc
	
	purpose:	Object��Buffer������
*********************************************************************/
#ifndef _SL_OBJECT_MGR_H_
#define _SL_OBJECT_MGR_H_
#include <new>
#include "slbase.h"
namespace sl
{
	///Object��Buffer�������Ļ��࣬�����߼�
	/*
		ÿ��Object����iIndexCount��CList�ṹ����Щ�ṹ����������ɸ�����
		�����ѷ����Object��һ������δ�����Object��������һ������
		Clist����iIndexCount*iObjectCount��
		CListInfo����iListCount*iIndexCount��
		�ڴ�ṹ��
		CListInfo: [0][1]..[�������-1]..[������� * ����������-1];
		CList:	   [0][1]..[�������-1]..[������� * ����������-1];
		Object:	   [0][1]..[�������-1]
	*/
	class CObjectMgrBase
	{
	public:

		//Index�����ͷ��
		class CListInfo
		{
		public:
			int		iHead;		///< ����ͷԪ���������е��±�
			int		iTail;		///< ����βԪ���������е��±�
			int		iSize;		///< �����й��ж��ٸ�Ԫ��

			CListInfo() : iHead(-1), iTail(-1), iSize(0) {}

			void Init()
			{
				iHead = -1;
				iTail = -1;
				iSize = 0;
			}
		};

		///�����������Ľṹ���൱������Ľڵ�
		class CList
		{
		public:
			int		iPrev;		///< ������ǰһ��Ԫ���������е��±�
			int		iNext;		///< �����к�һ��Ԫ���������е��±�
			int		iFlag;		///< ������ʾ��ǰԪ�����ĸ������У����������ѷ�������������������
			
			CList(): iPrev(-1), iNext(-1), iFlag(0) {}

			void Init()
			{
				iPrev = -1;
				iNext = -1;
				iFlag = 0;
			}
		};

	public:
		CObjectMgrBase() 
			:m_iObjectCount(0),
			 m_iObjectSize(0),
			 m_iListCount(0),
			 m_iIndexCount(0),
			 m_pstInfo(0),
			 m_pstList(0),
			 m_pDataBuffer(0)
		{}

		virtual ~CObjectMgrBase(){}

	protected:
		/*
			��ʼ��
			�����ʼ��������protected�ģ�������������ʹ��
			�ڴ����ⲿ������˴���������Ҫ���ռ���CountSize()����
			@param [in] pBuferr			CObjectMgr���õ��ڴ��Ǵ��ⲿ�������ģ����ﲻ�����ڴ�
			@param [in] iObjectCount	T�ĸ���
			@param [in] iObjectSize		Ӧ�õ���sizeof(T),���T��char *����ô�����ַ���������Ĵ�С
			@param [in] iListCount		����ĸ���
			@param [in] iIndexCount		�����ĸ���
			@param [in] bResetShm		�Ƿ�ʹ�ù����ڴ�
						- true Ĭ��ֵ   ��ʾʹ�ù����ڴ�
						- false ��ʾ��ʹ�ù����ڴ�
		*/
		virtual int Init(const char* pBuffer, int iObjectCount, int iObjectSize, int iListCount = 2, int iIndexCount = 1, bool bResetShm = true)
		{
			SLASSERT( !(!pBuffer || iObjectCount <= 0 || iObjectSize <= 0 || iListCount <= 1 || iIndexCount <= 0));
			if(!pBuffer || iObjectCount <= 0 || iObjectSize <=0 || iListCount < 2 || iIndexCount <= 0)
			{
				return -1;
			}
			//�洢�����ֽ���
			const size_t iCountSize = CountSize(iObjectCount, iObjectSize, iListCount, iIndexCount);
			//�������
			m_iObjectCount  =	iObjectCount;
			//�����С
			m_iObjectSize   =	iObjectSize;
			//ÿ���������������
			m_iListCount    =   iListCount;
			//����������
			m_iIndexCount   =   iIndexCount;
			//����ͷ����ʼ��λ��
			m_pstInfo		=   (CListInfo*)(pBuffer);
			//����ڵ����ʼ��λ�ã�ÿ���ڵ��Ǻʹ洢�Ķ�����һһ��Ӧ��
			m_pstList		=   (CList*) (pBuffer + sizeof(CListInfo) * m_iIndexCount * m_iListCount);
			//�洢�Ķ���ʼλ��
			m_pDataBuffer   =   (char*)pBuffer + sizeof(CListInfo) * m_iIndexCount * m_iListCount
								+ sizeof(CList) * m_iIndexCount * m_iObjectCount;

			if(bResetShm)
			{
				memset((void*)pBuffer, 0, iCountSize);
				//Ĭ�ϵģ� ���нڵ㶼��0�������У�0����������ǿ��нڵ�����

				//ÿ��������һ�γ�ʼ��
				for (int iIndexID = 0; iIndexID < m_iIndexCount; ++iIndexID)
				{
					for (int iListID = 0; iListID < m_iListCount; ++iListID) //ÿ��������������ͷһ�γ�ʼ��
					{
						ListInfo(iListID, iIndexID).Init();
					}
					//��ÿ�����������нڵ㶼�ŵ���0��������ȥ
					BuildInfoList(ListInfo(0, iIndexID), 0, iIndexID);
				}
			}
			return 0;
		}



	public:
		/*
			���������ڴ�Ĵ�С
			@param [in] iObjectCount	����ĸ���
			@param [in] iObjectSize		����Ĵ�С����λbyte
			@param [in] iListCount		ÿ������������ͷ�ĸ���
			@param [in] iIndexCount     �������ĸ���
		*/
		static size_t CountSize(int iObjectCount, int iObjectSize, int iListCount = 2, int iIndexCount = 1)
		{
			return sizeof(CListInfo) * iIndexCount * iListCount      /*����ͷ�ĸ���*/
				+ sizeof(CList) * iIndexCount * iObjectCount         /*����ڵ�ĸ���*/
				+ iObjectCount * iObjectSize;                        /*����������ֽ���*/
		}

		/*
			��iIndexID������ʼ����InfoList����
			�����ListID����iListID
		*/
		void BuildInfoList(CListInfo& stListInfo, int iListID, int iIndexID = 0)
		{
			for (int i = 0; i < m_iObjectCount; i++)
			{
				Prev(i, iIndexID) = i - 1;
				Next(i, iIndexID) = i + 1;
				Flag(i, iIndexID) = iListID;
			}
			Next(m_iObjectCount - 1, iIndexID) = -1;
			stListInfo.iHead = 0;
			stListInfo.iTail = m_iObjectCount - 1;
			stListInfo.iSize = m_iObjectCount;
		}

		/*
			�򻯵ķ���һ������Ԫ��
			��0�������з���һ��Ԫ�ص�iNewListID������
			�������Ҫ��ListInfo�������ⲿ��
			@param  [in] iIndexID    ������
			@param  [in] iNewListID  ���䵽�������� �������0
			@return �ɹ��ͷ��ط���Ԫ�ص������±꣬ʧ��ʱ����-1
		*/
		int InfoListAlloc(int iIndexID, int iNewListID)
		{
			if(iIndexID < 0 || iIndexID >= m_iIndexCount || iNewListID < 0 || iNewListID >= m_iListCount)
			{
				SL_ERROR("InfoListAlloc error: invalid index id(%d) or new list id(%d)", iIndexID, iNewListID);
				return -1;
			}
			return InfoListAlloc(iIndexID, iNewListID, ListInfo(0, iIndexID), ListInfo(iNewListID, iIndexID));
		}

		/*
			�򻯵�InfoList�н��ڵ�i�ƶ���������
			�������Ҫ��ListInfo�������ⲿ��
			@note ��������������У�����뵽��������
		*/
		int InfoListMove(int i, int iIndexID, int iNewListID)
		{
			if(iIndexID < 0 || iIndexID >= m_iIndexCount || iNewListID < 0 || iNewListID >= m_iListCount || i < 0 || i >= m_iObjectCount)
			{
				SL_ERROR("InfoListMove error: invalid index id(%d) or new list id(%d)", iIndexID, iNewListID);
				return -1;
			}
			return InfoListMove(i, iIndexID, iNewListID, ListInfo(Flag(i, iIndexID), iIndexID), ListInfo(iNewListID, iIndexID));
		}

	private:
		/*
			��������ɾ�����߼�
			�ѽڵ�i��stListInfo������ɾ��
			@note stListInfo���ⲿ����
		*/
		void InfoListRemove(CListInfo& stListInfo, int i, int iIndexID)
		{
			if(Prev(i, iIndexID) == -1)
			{
				if(Next(i, iIndexID) == -1)
				{
					stListInfo.iHead = -1;
					stListInfo.iTail = -1;
				}else
				{
					stListInfo.iHead = Next(i, iIndexID);
					Prev(Next(i, iIndexID), iIndexID) = -1;
				}
			}
			else
			{
				if(Next(i, iIndexID) == -1)
				{
					stListInfo.iTail = Prev(i, iIndexID);
					Next(Prev(i, iIndexID), iIndexID) = -1;
				}
				else
				{
					Prev(Next(i, iIndexID), iIndexID) = Prev(i, iIndexID);
					Next(Prev(i, iIndexID), iIndexID) = Next(i, iIndexID);
				}
			}

			stListInfo.iSize--;

			if(stListInfo.iSize > 0)
			{
				SLASSERT(stListInfo.iHead > -1);
			}
			else
			{
				SLASSERT(stListInfo.iHead == -1);
			}
		}

		/*
			���뵽������߼�
			�ѽڵ�i���뵽stListInfo������
			@note stListInfo ���ⲿ����
		*/
		void InfoListAppend(CListInfo& stListInfo, int i, int iIndexID)
		{
			if(stListInfo.iHead == -1 || stListInfo.iTail == -1)
			{
				Prev(i, iIndexID) = -1;
				Next(i, iIndexID) = -1;
				stListInfo.iHead = i;
				stListInfo.iTail = i;
			}
			else
			{
				Next(stListInfo.iTail, iIndexID) = i;
				Next(i, iIndexID) = -1;
				Prev(i, iIndexID) = stListInfo.iTail;
				stListInfo.iTail = i;
			}

			++stListInfo.iSize;
			if(stListInfo.iSize > 0)
			{
				SLASSERT(stListInfo.iHead >  -1);
			}
		}

		/*
			InfoList�н��ڵ�i�ƶ���������
			@note �����ڲ��ƶ����Ὣ�ڵ��ƶ�������β��
		*/
		int InfoListMove(int i, int iIndexID, int iNewListID, CListInfo& stOldList, CListInfo& stNewList)
		{
			if(iIndexID == 0 && iNewListID == 1)
			{
				SL_TRACE("InfoListMove i(hashkey) %d indexID %d newListId %d oldList(head %d, tail %d, size %d) newList(head %d, tail %d, size %d)",
					i, iIndexID, iNewListID, stOldList.iHead, stOldList.iTail, stOldList.iSize, stNewList.iHead, stNewList.iTail, stNewList.iSize);
			}
			if(stOldList.iSize <= 0)
			{
				return -1;
			}

			if(i < 0 || i >= m_iObjectCount)
			{
				return -2;
			}
			if(iIndexID < 0 || iIndexID >= m_iIndexCount)
			{
				return -3;
			}
			if(&m_pstInfo[Flag(i, iIndexID) + iIndexID * m_iListCount] != &stOldList)
			{
				return -4;
			}
			Flag(i, iIndexID) = iNewListID;

			InfoListRemove(stOldList, i, iIndexID);
			InfoListAppend(stNewList, i, iIndexID);

			///���������Ƿ��д������
			if(Flag(i, iIndexID) != iNewListID)
			{
				SL_ERROR("InfoListMove error, flag changed");
			}
			if(Next(i, iIndexID) != -1)
			{
				SL_ERROR("InfoListMove error, next is not -1");
			}
			if(Prev(i, iIndexID) == -1)  //ͷ���
			{
				if(Head(iNewListID, iIndexID) != i)
				{
					SL_ERROR("InfoListMove error, head error");
				}
			}
			else
			{
				if(Flag(Prev(i, iIndexID), iIndexID) != iNewListID)
				{
					SL_ERROR("InfoListMove error, prev is not in this list");
				}
			}
			return 0;
		}

	public:
		/*
			����һ�����õ�Ԫ��
			��0�������з���һ��Ԫ�ص�iNewListID������
			@param [in] iNewListID ���䵽��������? �������0
			@return �ɹ��ͷ��ط���Ԫ�ص������±꣬ʧ��ʱ�ͷ���С��0��ֵ
		*/
		int InfoListAlloc(int iIndexID, int iNewListID, CListInfo& stOldList, CListInfo& stNewList)
		{
			if(stOldList.iSize <= 0)
			{
				SL_ERROR("InfoListAlloc failed %d iIndexID %d iNewListID %d stOldList(%d %d %d), stNewList(%d %d %d)",
					stOldList.iSize, iIndexID, iNewListID, stOldList.iHead, stOldList.iTail, stOldList.iSize, stNewList.iHead, stNewList.iTail, stNewList.iSize);
				return -1;
			}
			int i = stOldList.iHead;
			int iRet = InfoListMove(i, iIndexID, iNewListID, stOldList, stNewList);
			if(iRet)
			{
				return iRet;
			}
			return i;
		}


	public:
		const char* GetBuffer() const
		{
			return (const char*)m_pstInfo;
		}
		int GetObjectCount() const
		{
			return m_iObjectCount;
		}
		int GetObjectSize() const
		{
			return m_iObjectSize;
		}
		int GetListCount() const
		{
			return m_iListCount;
		}
		int GetIndexCount() const
		{
			return m_iIndexCount;
		}
	
	public:
		///ָ�������һ���ڵ�
		int Head(int iListID, int iIndexID = 0) const
		{
			return ListInfo(iListID, iIndexID).iHead;
		}
		int& Head(int iListID, int iIndexID = 0)
		{
			return ListInfo(iListID, iIndexID).iHead;
		}

		//ָ������β��һ���ڵ�
		int Tail(int iListID, int iIndexID = 0) const
		{
			return ListInfo(iListID, iIndexID).iTail;
		}
		int& Tail(int iListID, int iIndexID = 0)
		{
			return ListInfo(iListID, iIndexID).iTail;
		}
		int Size(int iListID, int iIndexID = 0) const
		{
			return ListInfo(iListID, iIndexID).iSize;
		}
		int& Size(int iListID, int iIndexID = 0)
		{
			return ListInfo(iListID, iIndexID).iSize;
		}
		
		///List ����
		int Prev(int i, int iIndexID = 0) const
		{
			return List(i, iIndexID).iPrev;
		}
		int& Prev(int i, int iIndexID = 0)
		{
			return List(i, iIndexID).iPrev;
		}
		int Next(int i, int iIndexID = 0) const
		{
			return List(i, iIndexID).iNext;
		}
		int& Next(int i, int iIndexID = 0)
		{
			return List(i, iIndexID).iNext;
		}
		int Flag(int i, int iIndexID = 0) const
		{
			return List(i, iIndexID).iFlag;
		}
		int& Flag(int i, int iIndexID = 0)
		{
			return List(i, iIndexID).iFlag;
		}

		///ListInfo ����
		const CListInfo& ListInfo(int iListID, int iIndexID = 0) const
		{
			return m_pstInfo[iIndexID * m_iListCount + iListID];
		}
		CListInfo& ListInfo(int iListID, int iIndexID = 0)
		{
			SLASSERT( ((iIndexID * m_iListCount + iListID) < (m_iIndexCount * m_iListCount)));
			return m_pstInfo[iIndexID * m_iListCount + iListID];
		}
		const CList& List(int i, int iIndexID = 0) const
		{
			return m_pstList[iIndexID * m_iObjectCount + i];
		}
		CList& List(int i, int iIndexID = 0)
		{
			return m_pstList[iIndexID * m_iObjectCount + i];
		}

	protected:
		int			m_iObjectCount;			///< ����ĸ���
		int			m_iObjectSize;			///< ����Ĵ�С(��λbyte)
		int			m_iListCount;			///< ÿ������������ĸ���
		int			m_iIndexCount;			///< �������ĸ���

		CListInfo*  m_pstInfo;				///< ����������ͷ�Ŀ�ʼָ��
		CList*		m_pstList;				///< ����������ڵ�Ŀ�ʼָ��
		char*		m_pDataBuffer;			///< ����ʼʹ�õĿ�ʼָ��

	};// class CObjectMgrBase

	//���������������أ�
	/*
		�������Ҫ���ṩ�˶�Object�ķ��ʵ�[]�����
	*/
	template<typename T>
	class CObjectMgr: public CObjectMgrBase
	{
	public:
		virtual ~CObjectMgr(){}

		virtual int Init(const char* pBuffer, int iObjectCount, int iObjectSize = sizeof(T), int iListCount = 2, int iIndexCount = 1, bool bResetShm = true)
		{
			int iRet = CObjectMgrBase::Init(pBuffer, iObjectCount, iObjectSize, iListCount, iIndexCount, bResetShm);
			if(iRet)
			{
				return iRet;
			}
			if(bResetShm)
			{
				m_astObject = new (m_pDataBuffer) T[iObjectCount]; 
			}
			else
			{
				//OBJ_STAT->SetNotReset();
				m_astObject = new (m_pDataBuffer) T[iObjectCount];
				//OBJ_STAT->SetReset();
			}
			return 0;
		}

		//����[]�����
		T& operator[] (int i)
		{
			return m_astObject[i];
		}
		const T& operator[](int i) const
		{
			return m_astObject[i];
		}
		
		///ָ���±���ʶ���
		/*
			�ṩһ�ַ������[]���ʶ���ķ�ʽ
		*/
		T& getObjectByIndex(int iIndex)
		{
			return m_astObject[iIndex];
		}
		const T& getObjectByIndex(int iIndex) const
		{
			return m_astObject[iIndex];
		}
	protected:
		T*		m_astObject;

	}; //class CObjectMgr

	template<>
	class CObjectMgr<char*>: public CObjectMgrBase
	{
	public:
		virtual ~CObjectMgr() {}
		
		virtual int Init(const char* pBuffer, int iObjectCount, int iObjectSize, int iListCount = 2, int iIndexCount = 1, bool bResetShm = true)
		{
			int iRet = CObjectMgrBase::Init(pBuffer, iObjectCount, iObjectSize, iListCount, iIndexCount, bResetShm);
			if(iRet)
			{
				return iRet;
			}
			return 0;
		}

		char* operator[] (int i)
		{
			return m_pDataBuffer + i * m_iObjectSize;
		}
		const char* operator[] (int i) const
		{
			return m_pDataBuffer + i * m_iObjectSize;
		}
	};

}// namespace sl

#endif