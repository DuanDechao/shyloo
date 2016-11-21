//多级ObjectMgr，每一级的对象大小递增
/********************************************************************
	created:	2015/12/05
	created:	5:12:2015   15:56
	filename: 	e:\myproject\shyloo\sllib\slmulti_object_mgr.h
	file path:	e:\myproject\shyloo\sllib
	file base:	slmulti_object_mgr
	file ext:	h
	author:		ddc
	
	purpose:	多级ObjectMgr
*********************************************************************/
#ifndef _SL_MULTI_OBJECT_MGR_H_
#define _SL_MULTI_OBJECT_MGR_H_
#include "slbase.h"
#include "slobject_mgr.h"
#include "slobj_reload_check.h"
namespace sl
{
	//多级ObjectMgr的下标信息
	class CMgrIndex
	{
	public:
		int		iMgrIndex;			///< 在CMultiObjectMgr中的哪个Mgr
		int		iObjIndex;			///< 在Mgr中的下标

		CMgrIndex(): iMgrIndex(-1), iObjIndex(-1) {}
		
		CMgrIndex(const CMgrIndex& obj)
		{
			iMgrIndex = obj.iMgrIndex;
			iObjIndex = obj.iObjIndex; 
		}

		CMgrIndex& operator = (const CMgrIndex& obj)
		{
			iMgrIndex = obj.iMgrIndex;
			iObjIndex = obj.iObjIndex;
			return *this;
		}

		bool operator == (const CMgrIndex& obj)
		{
			return iMgrIndex == obj.iMgrIndex &&
					iObjIndex == obj.iObjIndex;
		}
		bool operator != (const CMgrIndex& obj)
		{
			return ! operator == (obj);
		}

		void Clear()
		{
			iMgrIndex = -1;
			iObjIndex = -1;
		}

		bool IsNull() const
		{
			return (iMgrIndex == -1 || iObjIndex == -1);
		}
	}; /// class CMgrIndex

	class CMgrConfig
	{
	public:
		int		m_iObjectCount;
		int		m_iObjectSize;

		bool operator < (const CMgrConfig& obj) const
		{
			return m_iObjectSize < obj.m_iObjectSize;
		}
	}; // class CMgrConfig

	///多级的ObjectMgr
	class CMultiObjectMgr
	{
	protected:
		typedef CObjectMgr<char*> TObjectMgr;
		typedef vector<TObjectMgr> TObjectMgrVec;

		typedef vector<CMgrConfig> TMgrConfigVec;
		TObjectMgrVec		m_astObjectMgr;
	
	public:
		CMultiObjectMgr() {}
		virtual ~CMultiObjectMgr() {}

		/*
			初始化
			@param [in]  pszMgrConfig		不同大小的内存块和数量
				字符串格式是(内存块大小1,内存块数量)...(内存块大小n,内存块数量n)
			@param [in]  pszBuffer   	内存首地址(内存是外部分配的)
			@param [in]  iBufferSize	内存的大小，必须要等于CountSize()计算出来的大小
		*/
		int Init(const char* pszMgrConfig, const char* pszBuffer, int iBufferSize,
			int iListCount = 2, int iIndexCount = 1, bool bReload = true)
		{
			int iRet = 0;
			TMgrConfigVec ast;
			iRet = BuildMgrConfigVec(pszMgrConfig, ast);
			if(iRet)
			{
				return iRet;
			}
			return Init(ast, pszBuffer, iBufferSize, iListCount, iIndexCount, bReload);
		}


		/*
			计算astMgrConfig需要的内存块大小，方便申请内存/共享内存
			@param [in]  pszMgrConfig		不同大小的内存块和数量
			@return 返回所需内存的总大小，单位是字节
		*/
		static size_t CountSize(const char* pszMgrConfig, int iListCount = 2, int iIndexCount = 1)
		{
			int iRet = 0;
			TMgrConfigVec ast;
			iRet  = BuildMgrConfigVec(pszMgrConfig, ast);
			if(iRet)
			{
				return iRet;
			}
			return CountSize(ast, iListCount, iIndexCount);
		}

		/*
			如果要分配缓冲区，应该从哪个Mgr中分配
			计算方法是从开始遍历，找到最先满足大小的Mgr
			@return 返回Mgr的下标，如果都不满足就返回-1
		*/
		int CalcMgrIndex(int iSize) const
		{
			for (size_t i = 0; i < m_astObjectMgr.size(); ++i)
			{
				if(m_astObjectMgr[i].GetObjectSize() >= iSize)
				{
					return static_cast<int>(i);
				}
			}
			return -1;
		}

		/*
			分配缓冲区
			需要在外面placement new
			@param [in]  iSize		要分配的缓冲区大小
			@param [out] pstIndex	分配缓冲区的下标信息
		*/
		char* Alloc(int iMgrIndex, CMgrIndex* pstIndex)
		{
			if(pstIndex)
			{
				pstIndex->Clear();
			}
			if (iMgrIndex < 0)
			{
				return NULL;
			}

			//分配空间
			TObjectMgr& stMgr = m_astObjectMgr[iMgrIndex];
			int iIndex = stMgr.InfoListAlloc(0, 1);
			if(iIndex < 0)
			{
				SL_WARNING("Mgr[%d].Alloc failed %d", iMgrIndex, iIndex);
				return NULL;
			}
			if(pstIndex)
			{
				pstIndex->iMgrIndex = iMgrIndex;
				pstIndex->iObjIndex = iIndex;
			}
			return m_astObjectMgr[iMgrIndex][iIndex];
		}

		/*
			判断缓冲区是不是从这里分配的
			@param [in]   pOject	要检测的缓冲区
			@param [out]  pstIndex	保存缓冲区的下标信息
		*/
		bool IsValidObject(const char* pObject, CMgrIndex* pstIndex) const
		{
			if( !pObject)
			{
				return false;
			}
			for (size_t i = 0; i < m_astObjectMgr.size(); i++)
			{
				const TObjectMgr& stMgr = m_astObjectMgr[i];
				if (pObject >= stMgr[0] && pObject <= stMgr[stMgr.GetObjectCount() - 1])
				{
					if( (pObject - stMgr[0])  % stMgr.GetObjectSize() == 0 )
					{
						if(pstIndex)
						{
							pstIndex->iMgrIndex = static_cast<int>(i);
							pstIndex->iObjIndex = static_cast<int>( (pObject - stMgr[0]) / stMgr.GetObjectSize() );
						}
						return true;
					}
					else
					{
						return false;
					}
				}
			}
			return false;
		}


		/*
			判断下标信息是不是从这里分配的
			@param [in]   stIndex	要检测的下标信息
		*/
		bool IsValidIndex(const CMgrIndex& stIndex) const
		{
			if(stIndex.iMgrIndex < 0 || stIndex.iMgrIndex >= (int)m_astObjectMgr.size() ||
				stIndex.iObjIndex < 0 || stIndex.iObjIndex >= (int)m_astObjectMgr[stIndex.iMgrIndex].GetObjectCount())
			{
				return false;
			}
			return true;
		}

		/*
			释放缓冲区
			需要在外面调用placement new出来的对象的析构函数
		*/
		int Free(const char* pObject)
		{
			CMgrIndex stIndex;
			if(!IsValidObject(pObject, &stIndex))
			{
				return -1;
			}
			return Free(stIndex);
		}

		/*
			释放缓冲区
			以分配缓冲区时的下标信息做参数效率更高
		*/
		int Free(const CMgrIndex& stIndex)
		{
			if(!IsValidIndex(stIndex))
			{
				return -1;
			}
			m_astObjectMgr[stIndex.iMgrIndex].InfoListMove(stIndex.iObjIndex, 0, 0);
			return 0;
		}


		//返回下标信息对应的缓冲区
		char* Get(const CMgrIndex& stIndex)
		{
			return m_astObjectMgr[stIndex.iMgrIndex][stIndex.iObjIndex];
		}
		size_t GetMgrCount() const
		{
			return m_astObjectMgr.size();
		}
		int GetMgrObjectSize(int iMgrIndex) const
		{
			return m_astObjectMgr[iMgrIndex].GetObjectSize();
		}

		int GetMgrObjectCount(int iMgrIndex) const
		{
			return m_astObjectMgr[iMgrIndex].GetObjectCount();
		}

		int Size(int iMgrIndex, int iIndexID, int iListID)
		{
			return m_astObjectMgr[iMgrIndex].Size(iListID, iIndexID);
		}

		//获得某一个MgrIndex在索引iIndexID的那个链上
		int Flag(CMgrIndex& stIndex, int iIndexID)
		{
			if(stIndex.IsNull())
			{
				return -1;
			}
			return m_astObjectMgr[stIndex.iMgrIndex].Flag(stIndex.iObjIndex, iIndexID);
		}

		/*
			获得多级ObjectMgr某条链表的头部
			@param [out] stIndex		头部的下标信息，如果链表为空，则stIndex.IsNull
		*/
		void GetHead(CMgrIndex& stIndex, int iListID, int iIndexID) const
		{
			stIndex.Clear();
			for (size_t i = 0; i< m_astObjectMgr.size(); ++i)
			{
				stIndex.iMgrIndex = static_cast<int>(i);
				stIndex.iObjIndex = m_astObjectMgr[i].Head(iListID, iIndexID);
				if(!stIndex.IsNull())
				{
					return;
				}
			}
		}

		//获取多级ObjectMgr某条链表的下一个结点
		void GetNext(const CMgrIndex& stIndex, CMgrIndex& stNext, int iIndexID) const
		{
			stNext.iMgrIndex  = stIndex.iMgrIndex;
			stNext.iObjIndex  = m_astObjectMgr[stIndex.iMgrIndex].Next(stIndex.iObjIndex, iIndexID);
			if(!stNext.IsNull())
			{
				return;
			}

			//在这个级别的ObjectMgr找不到下一个结点了，就在下一个级别的ObjectMgr找
			int iListID = m_astObjectMgr[stIndex.iMgrIndex].Flag(stIndex.iObjIndex, iIndexID);
			for(size_t i = stIndex.iMgrIndex + 1; i < m_astObjectMgr.size(); ++i)
			{
				stNext.iMgrIndex  =  static_cast<int>(i);
				stNext.iObjIndex  = m_astObjectMgr[i].Head(iListID, iIndexID);
				if(!stNext.IsNull())
				{
					return;
				}
			}
		}

		//把结点移动到另一个链表
		int Move(const CMgrIndex& stIndex, int iIndexID, int iNewListID)
		{
			TObjectMgr& stMgr = m_astObjectMgr[stIndex.iMgrIndex];
			return stMgr.InfoListMove(stIndex.iObjIndex, iIndexID, iNewListID);
		}

	protected:
		/*
			初始化
			@param [in]  astMgrConfig	不同大小的内存块和数量
			@param [in]  pszBuffer		内存首地址(内存是外部分配的)
			@param [in]  iBufferSize	内存的大小，必须要等于CountSize()计算出来的大小
		*/
		int Init(const TMgrConfigVec& astMgrConfig, const char* pszBuffer,
			size_t iBufferSize, int iListCount = 2, int iIndexCount = 1, bool bResetShm = true)
		{
			const size_t iAllSize = CountSize(astMgrConfig, iListCount, iIndexCount);
			if(iAllSize != iBufferSize || iAllSize <= 0)
			{
				return -1;
			}
			size_t iPos = 0;
			int iRet  = 0;
			for (size_t i = 0; i < astMgrConfig.size(); ++i)
			{
				m_astObjectMgr.push_back(TObjectMgr());
				iRet  = m_astObjectMgr[i].Init(pszBuffer + iPos, astMgrConfig[i].m_iObjectCount,
					astMgrConfig[i].m_iObjectSize, iListCount, iIndexCount, bResetShm);
				if(iRet)
				{
					return iRet;
				}

				iPos += TObjectMgr::CountSize(astMgrConfig[i].m_iObjectCount, astMgrConfig[i].m_iObjectSize,
					iListCount, iIndexCount);
			}
			return 0;
		}

		/*
			计算astMgrConfig需要的内存块大小，方便申请内存/共享内存
			@param [in]  astMgrConfig		不同大小的内存块和数量
			@return 返回所需的内存的总大小， 单位字节
		*/
		static size_t CountSize(const TMgrConfigVec& astMgrConfig, int iListCount = 2, int iIndexCount = 1)
		{
			int iAllSize = 0;
			for (size_t i = 0; i < astMgrConfig.size(); ++i)
			{
				iAllSize += static_cast<int>(TObjectMgr::CountSize(astMgrConfig[i].m_iObjectCount, 
					astMgrConfig[i].m_iObjectSize, iListCount,iIndexCount));
			}
			return iAllSize;
		}
	public:
		/*
			从字符串解析出astMgrConfig
			字符串格式是(内存块大小1，内存块数量1)...(内存块大小2, 内存块数量2)...(内存块大小n,内存块数量n)
			内存块大小 的单位是byte
		*/
		static int BuildMgrConfigVec(const char* pszMgrConfig, TMgrConfigVec& astMgrConfig)
		{
			astMgrConfig.clear();
			
			string s(pszMgrConfig);
			CStringUtils::RemoveNot(s, "01234567890,)");
			
			vector<string> astKeyValue;
			CStringUtils::Split(s, ")", astKeyValue);
			
			string sSize, sCount;
			for (size_t i = 0; i < astKeyValue.size(); ++i)
			{
				if(!CStringUtils::SplitIni(astKeyValue[i], sSize, sCount, ','))
				{
					astMgrConfig.clear();
					return -1;
				}
				CMgrConfig stConfig;
				stConfig.m_iObjectSize   =   CStringUtils::StrToInt<int>(sSize.c_str());
				stConfig.m_iObjectCount  =   CStringUtils::StrToInt<int>(sCount.c_str());
				if( !(stConfig.m_iObjectSize > 0 && stConfig.m_iObjectCount > 0))
				{
					astMgrConfig.clear();
					return -2;
				}
				astMgrConfig.push_back(stConfig);
			}

			//排序
			sort(astMgrConfig.begin(), astMgrConfig.end());
			return 0;
		}
	};


} //namespace sl
#endif