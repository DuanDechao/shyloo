//统计类
/********************************************************************
	created:	2015/12/14
	created:	14:12:2015   23:17
	filename: 	e:\workspace\shyloo\sllib\slstat.h
	file path:	e:\workspace\shyloo\sllib
	file base:	slstat
	file ext:	h
	author:		ddc
	
	purpose:	状态统计类
*********************************************************************/

#ifndef _SL_STAT_H_
#define _SL_STAT_H_

#include "slbase.h"

namespace sl
{
	class CStatItem
	{
	public:
		enum
		{
			SL_DESC_MAX = 128,	
		};

		char		m_szDesc[SL_DESC_MAX];
		int64		m_iCount;
		int64		m_iZeroCount;
		int64		m_iMax;
		int64		m_iMin;
		int64		m_llAll;
		
	public:
		CStatItem()
		{
			m_szDesc[0] = '\0';
			Reset();
		}
		
		~CStatItem(){}
		
		void Put(int i = 1)
		{
			m_llAll += i;
			m_iMax = SL_MAX(i, m_iMax);
			m_iMin = SL_MIN(i, m_iMin);

			if(m_iCount == 0)
			{
				m_iMax = m_llAll;
				m_iMin = m_llAll;
			}
			++m_iCount;
			if(i == 0)
			{
				++ m_iZeroCount;
			}
		}
		
		void Dump(CLog* plog, ELogFilter filter)
		{
#ifndef WIN32
			plog->Log(filter, "%s: Count=%lld Max=%lld Min=%lld Avg=%lld All=%lld Zero=%lld",
#else
			plog->Log(filter, "%s: Count=%I64d Max=%I64d Min=%I64d Avg=%I64d All=%I64d Zero=%I64d",
#endif
				m_szDesc, m_iCount, m_iMax, m_iMin, 
				(m_iCount == 0 ? 0 : m_llAll / m_iCount),
				m_llAll, m_iZeroCount);
		}

		void Reset()
		{
			m_iZeroCount = 0;
			m_iCount     = 0;
			m_iMax       = 0;
			m_iMin       = 0;
			m_llAll		 = 0;
		}

	};

	class CStat
	{
	public:
		CStat()
		{
			m_pItems	=	NULL;
			m_iCount	=	0;
		}

		CStat(int iItemCount, const char pszItemDesc[][CStatItem::SL_DESC_MAX])
		{
			m_pItems	=	NULL;
			m_iCount	=	0;
			Init(iItemCount, pszItemDesc);
		}

		virtual ~CStat()
		{
			if(m_pItems)
			{
				delete [] m_pItems;
				m_pItems = 0;
				m_iCount = 0;
			}
		}

		int Init(int iItemCount, const char pszItemDesc[][CStatItem::SL_DESC_MAX])
		{
			if(!pszItemDesc || iItemCount <= 0)
			{
				return -1;
			}
			m_iCount = iItemCount;
			m_pItems = new CStatItem[m_iCount];
			if(!m_pItems)
			{
				SL_ERROR("new CStatItem[%d] failed", m_iCount);
				return -2;
			}
			for (int i = 0; i < m_iCount; ++i)
			{
				sl_snprintf(SL_STRSIZE(m_pItems[i].m_szDesc), "%s", pszItemDesc[i]);
			}
			return 0;
		}

		int Put(int iIndex, int i = 1)
		{
			if(iIndex < 0 || iIndex >= m_iCount)
			{
				SL_ERROR("When put (%d, %d) to stat, index overflow %d", iIndex, i, m_iCount);
				return -1;
			}
			m_pItems[iIndex].Put(i);
			return 0;
		}

		void Dump(CLog* plog, ELogFilter filter = EInfo)
		{
			for (int i =0; i< m_iCount; ++i)
			{
				m_pItems[i].Dump(plog, filter);
				m_pItems[i].Reset();
			}
		}

		int GetCount() const
		{
			return m_iCount;
		}

		CStatItem& operator[] (int i)
		{
			return m_pItems[i];
		}

	private:
		CStatItem*		m_pItems;
		int				m_iCount;
	};

} // namespace sl
#endif