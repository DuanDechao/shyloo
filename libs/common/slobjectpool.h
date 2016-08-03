/********************************************************************
	created:	2016/08/03
	created:	3:8:2016   13:42
	filename: 	d:\Projects\shyloo\libs\common\slobjectpool.h
	file path:	d:\Projects\shyloo\libs\common
	file base:	slobjectpool
	file ext:	h
	author:		ddc
	
	purpose:	
******************************************************************/
#ifndef _SL_OBJECT_POOL_H_
#define _SL_OBJECT_POOL_H_
#include <set>
#include <string>
#include <queue>
namespace sl
{
	class IObjectPool
	{
	public:
		virtual ~IObjectPool(){}
		virtual size_t GetFreeCount(void) = 0;
		virtual size_t GetAllCount(void)  = 0;
		virtual size_t GetMallocCount(void) = 0;
		virtual const char* GetName(void) = 0;
		virtual size_t GetObjectSize(void) = 0;
		virtual size_t AllocFreq(void) = 0;
	};

	class CObjectPoolMgr
	{
	public:
		typedef std::set<IObjectPool*>	SetObjectPool;
		typedef SetObjectPool::iterator SetObjectPoolIter;
		
		typedef std::map<std::string, SetObjectPool>	MapIndexObjectPool;
		typedef MapIndexObjectPool::iterator MapIndexObjectPoolIter;

	public:
		static void Add(IObjectPool* poObjectPool)
		{
			MapIndexObjectPoolIter mapIter = GetIndexObjectPoolMap().find(poObjectPool->GetName());
			if(mapIter != GetIndexObjectPoolMap().end())
			{
				SetObjectPool& setPool = mapIter->second;

				SetObjectPoolIter setIter = setPool.find(poObjectPool);
				if(setIter != setPool.end())
				{
					SL_ASSERT(0);
					return;
				}
			}
			GetIndexObjectPoolMap()[poObjectPool->GetName()].insert(poObjectPool);
		}

		static void Remove(IObjectPool* poPool)
		{
			MapIndexObjectPoolIter mapIt = GetIndexObjectPoolMap().find(poPool->GetName());
			if(mapIt == GetIndexObjectPoolMap().end())
			{
				SL_ASSERT(0);
			}
			else
			{
				SetObjectPool& setPool = mapIt->second;
				SetObjectPoolIter setIter = setPool.find(poPool);
				if(setIter == setPool.end())
				{
					SL_ASSERT(0);
				}
				else
				{
					setPool.erase(setIter);
					if(setPool.empty())
					{
						GetIndexObjectPoolMap().erase(mapIt);
					}
				}
			}
		}

		static MapIndexObjectPool& GetIndexObjectPoolMap()
		{
			static MapIndexObjectPool	s_mapIndexObjectPool;
			return s_mapIndexObjectPool;
		}
	};

	template<class T, class Lock>
	class CObjectPool: public IObjectPool
	{
	public:
		explicit CObjectPool(void)
			:m_dwInitCount(0),
			 m_dwGrowCount(0),
			 m_dwAllocFreq(0)
		{
			CObjectPoolMgr::Add(this);
		}

		~CObjectPool(void)
		{
			m_ListFree.Clear();
			std::for_each(m_ListAll.begin(), m_ListAll.end(), Skiller1());
			m_ListAll.clear();

			CObjectPoolMgr::Remove(this);
		}

		bool Init(UINT32 dwInitCount, UINT32 dwGrowCount)
		{
			m_dwInitCount = dwInitCount;
			m_dwGrowCount = dwGrowCount;
			return _AllocT(m_dwInitCount);
		}

		T* FetchObj(void)
		{
			CShellT* poShellT = NULL;
			if(m_ListFree.empty())
			{
				if(!_AllocT(m_dwGrowCount))
				{
					return NULL;
				}
			}

			poShellT = m_ListFree.front();
			if( NULL == poShellT)
			{
				return NULL;
			}

			if( 0 != poShellT->m_byRef)
			{
				//已经在使用
				return NULL;
			}

			new (poShellT)CShellT;
			poShellT->m_byRef = 1;

			m_ListFree.pop();
			m_dwAllocFreq++;
			return poShellT;
		}

		void ReleaseObj(T* pObj)
		{
			if(NULL == pObj)
			{
				return;
			}

			CShellT* poShellT = static_cast<CShellT*>(pObj);
			if( NULL == poShellT)
			{
				return;
			}

			if(0 != poShellT->m_byRef - 1)
			{
				return;
			}
			poShellT->m_byRef--;
			poShellT->~CShellT();
			m_ListFree.push(poShellT);
		}

		size_t GetFreeCount(void)
		{
			return m_ListFree.size();
		}

		size_t GetAllCount(void)
		{
			size_t count = 0;
			if(m_dwInitCount > 0)
			{
				if(GetMallocCount() > 0)
				{
					count = m_dwInitCount + (GetMallocCount() - 1) * m_dwGrowCount;  
				}
				else
				{
					SL_ASSERT(0);
				}
			}
			else
			{
				count = GetMallocCount() * m_dwGrowCount;
			}

			return count;
		}

		size_t GetMallocCount(void)
		{
			return m_ListAll.size();
		}

		const char* GetTName(void)
		{
			return typeid(T).name();
		}

		size_t GetObjectSize(void)
		{
			return sizeof(T);
		}

		virtual size_t AllocFreq(void)
		{
			size_t ret = m_dwAllocFreq;
			m_dwAllocFreq = 0;
			return ret;
		}
		
	protected:
		bool _AllocT(UINT32 dwCount)
		{
			if(0 == dwCount)
			{
				return false;
			}
			if(!m_ListFree.empty())
			{
				return true;
			}

			CShellT* poShellT = (CShellT*)malloc(sizeof(CShellT) * dwCount);
			if( NULL == poShellT)
			{
				return false;
			}

			for (UINT32 i = 0; i < dwCount; ++i)
			{
				poShellT[i].m_byRef = 0;
				m_ListFree.push(&poShellT[i]);
			}
			m_ListAll.push_back(poShellT);
			return true;
		}

	private:
		class CShellT: public T
		{
		public:
			CShellT(): m_byRef(0) {}
			~CShellT(){}
			UINT8			m_byRef;
		};

		struct Skiller1
		{
			void operator()(CShellT* poShellT)
			{
				free(poShellT);
			}
		};

		class ClearableQueue: public std::queue<CShellT*>
		{
		public:
			void Clear()
			{
				c.clear();
			}
		} m_ListFree;

		std::list<CShellT*>		m_ListAll;
		UINT32			m_dwInitCount;
		UINT32			m_dwGrowCount;
		UINT32			m_dwAllocFreq;
	};
} //namespace sl
#endif
