///Object和Buffer管理器（对象池）
/********************************************************************
	created:	2015/12/05
	created:	5:12:2015   15:03
	filename: 	e:\myproject\shyloo\sllib\slobject_mgr.h
	file path:	e:\myproject\shyloo\sllib
	file base:	slobject_mgr
	file ext:	h
	author:		ddc
	
	purpose:	Object和Buffer管理器
*********************************************************************/
#ifndef _SL_OBJECT_MGR_H_
#define _SL_OBJECT_MGR_H_
#include <new>
#include "slbase.h"
namespace sl
{
	///Object和Buffer管理器的基类，核心逻辑
	/*
		每个Object都有iIndexCount个CList结构，这些结构可以组成若干个链表
		比如已分配的Object是一个链表，未分配的Object是在另外一个链表
		Clist共有iIndexCount*iObjectCount个
		CListInfo共有iListCount*iIndexCount个
		内存结构：
		CListInfo: [0][1]..[链表个数-1]..[链表个数 * 链表区个数-1];
		CList:	   [0][1]..[对象个数-1]..[对象个数 * 链表区个数-1];
		Object:	   [0][1]..[对象个数-1]
	*/
	class CObjectMgrBase
	{
	public:

		//Index链表的头部
		class CListInfo
		{
		public:
			int		iHead;		///< 链表头元素在数组中的下标
			int		iTail;		///< 链表尾元素在数组中的下标
			int		iSize;		///< 链表中共有多少个元素

			CListInfo() : iHead(-1), iTail(-1), iSize(0) {}

			void Init()
			{
				iHead = -1;
				iTail = -1;
				iSize = 0;
			}
		};

		///用来组成链表的结构，相当于链表的节点
		class CList
		{
		public:
			int		iPrev;		///< 链表中前一个元素在数组中的下标
			int		iNext;		///< 链表中后一个元素在数组中的下标
			int		iFlag;		///< 用来表示当前元素在哪个链表中，空闲链表？已分配链表？还是其他链表？
			
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
			初始化
			这个初始化函数是protected的，请在派生类中使用
			内存在外部分配好了传进来，需要多大空间用CountSize()计算
			@param [in] pBuferr			CObjectMgr所用的内存是从外部传进来的，这里不分配内存
			@param [in] iObjectCount	T的个数
			@param [in] iObjectSize		应该等于sizeof(T),如果T是char *，那么就是字符期望分配的大小
			@param [in] iListCount		链表的个数
			@param [in] iIndexCount		索引的个数
			@param [in] bResetShm		是否使用共享内存
						- true 默认值   表示使用共享内存
						- false 表示不使用共享内存
		*/
		virtual int Init(const char* pBuffer, int iObjectCount, int iObjectSize, int iListCount = 2, int iIndexCount = 1, bool bResetShm = true)
		{
			SLASSERT( !(!pBuffer || iObjectCount <= 0 || iObjectSize <= 0 || iListCount <= 1 || iIndexCount <= 0));
			if(!pBuffer || iObjectCount <= 0 || iObjectSize <=0 || iListCount < 2 || iIndexCount <= 0)
			{
				return -1;
			}
			//存储区的字节数
			const size_t iCountSize = CountSize(iObjectCount, iObjectSize, iListCount, iIndexCount);
			//对象个数
			m_iObjectCount  =	iObjectCount;
			//对象大小
			m_iObjectSize   =	iObjectSize;
			//每个索引区链表个数
			m_iListCount    =   iListCount;
			//索引区个数
			m_iIndexCount   =   iIndexCount;
			//链表头对象开始的位置
			m_pstInfo		=   (CListInfo*)(pBuffer);
			//链表节点对象开始的位置，每个节点是和存储的对象是一一对应的
			m_pstList		=   (CList*) (pBuffer + sizeof(CListInfo) * m_iIndexCount * m_iListCount);
			//存储的对象开始位置
			m_pDataBuffer   =   (char*)pBuffer + sizeof(CListInfo) * m_iIndexCount * m_iListCount
								+ sizeof(CList) * m_iIndexCount * m_iObjectCount;

			if(bResetShm)
			{
				memset((void*)pBuffer, 0, iCountSize);
				//默认的， 所有节点都在0号链表中，0号链表必须是空闲节点链表

				//每个索引区一次初始化
				for (int iIndexID = 0; iIndexID < m_iIndexCount; ++iIndexID)
				{
					for (int iListID = 0; iListID < m_iListCount; ++iListID) //每个索引区的链表头一次初始化
					{
						ListInfo(iListID, iIndexID).Init();
					}
					//将每个索引区所有节点都放到第0个链表中去
					BuildInfoList(ListInfo(0, iIndexID), 0, iIndexID);
				}
			}
			return 0;
		}



	public:
		/*
			计算所用内存的大小
			@param [in] iObjectCount	对象的个数
			@param [in] iObjectSize		对象的大小，单位byte
			@param [in] iListCount		每个链表区链表头的个数
			@param [in] iIndexCount     链表区的个数
		*/
		static size_t CountSize(int iObjectCount, int iObjectSize, int iListCount = 2, int iIndexCount = 1)
		{
			return sizeof(CListInfo) * iIndexCount * iListCount      /*链表头的个数*/
				+ sizeof(CList) * iIndexCount * iObjectCount         /*链表节点的个数*/
				+ iObjectCount * iObjectSize;                        /*对象所需的字节数*/
		}

		/*
			把iIndexID索引初始化成InfoList链表
			链表的ListID就是iListID
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
			简化的分配一个可用元素
			从0号链表中分配一个元素到iNewListID号链表
			这个函数要求ListInfo不能是外部的
			@param  [in] iIndexID    链表区
			@param  [in] iNewListID  分配到几号链表？ 必须大于0
			@return 成功就返回分配元素的数组下标，失败时返回-1
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
			简化的InfoList中将节点i移动到新链表
			这个函数要求ListInfo不能是外部的
			@note 如果已在新链表中，会加入到链表的最后
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
			从链表中删除的逻辑
			把节点i从stListInfo链表中删除
			@note stListInfo从外部传入
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
			加入到链表的逻辑
			把节点i加入到stListInfo链表中
			@note stListInfo 从外部传入
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
			InfoList中将节点i移动到新链表
			@note 链表内部移动，会将节点移动到链表尾部
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

			///检查操作后是否有错误出现
			if(Flag(i, iIndexID) != iNewListID)
			{
				SL_ERROR("InfoListMove error, flag changed");
			}
			if(Next(i, iIndexID) != -1)
			{
				SL_ERROR("InfoListMove error, next is not -1");
			}
			if(Prev(i, iIndexID) == -1)  //头结点
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
			分配一个可用的元素
			从0号链表中分配一个元素到iNewListID号链表
			@param [in] iNewListID 分配到几号链表? 必须大于0
			@return 成功就返回分配元素的数组下标，失败时就返回小于0的值
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
		///指定链表第一个节点
		int Head(int iListID, int iIndexID = 0) const
		{
			return ListInfo(iListID, iIndexID).iHead;
		}
		int& Head(int iListID, int iIndexID = 0)
		{
			return ListInfo(iListID, iIndexID).iHead;
		}

		//指定链表尾第一个节点
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
		
		///List 操作
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

		///ListInfo 操作
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
		int			m_iObjectCount;			///< 对象的个数
		int			m_iObjectSize;			///< 对象的大小(单位byte)
		int			m_iListCount;			///< 每个链表区链表的个数
		int			m_iIndexCount;			///< 链表区的个数

		CListInfo*  m_pstInfo;				///< 链表区链表头的开始指针
		CList*		m_pstList;				///< 链表区链表节点的开始指针
		char*		m_pDataBuffer;			///< 对象开始使用的开始指针

	};// class CObjectMgrBase

	//对象管理器（对象池）
	/*
		这个类主要是提供了对Object的访问的[]运算符
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

		//重载[]运算符
		T& operator[] (int i)
		{
			return m_astObject[i];
		}
		const T& operator[](int i) const
		{
			return m_astObject[i];
		}
		
		///指定下标访问对象
		/*
			提供一种非运算符[]访问对象的方式
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