//静态数组模板

#ifndef _SL_ARRAY_H_
#define _SL_ARRAY_H_

#include "slassert.h"

namespace sl
{
	//数组模板类
	/*
		@code
			int t[] = {1,2,3,4}
			int t1[] = {1,2,3,4,5}

			CArray<int, 10> v1(t);  //以数组为构造参数
			CArray<int, 5> v2(v1);  //以不同的CArray为构造参数
			CArray<int, 100> v3;	//默认构造
			v3.AddItemList(v2);		//追加一个CArray对象
			v3.AddItemList(t);		//追加一个数组
			printArray(v1);
			printArray(v2);
			printArray(v3);
			v1 = v3;				//以不同的CArray进行赋值
			v2 = t1;				//以数组进行赋值
			printArray(v1);
			pirntArray(v2);
		@endcode
	*/

	template<typename T, int COUNT>
	class CArray
	{
	public:
		int		m_iUsedCount;			//< 已使用数组个数 
		T       m_astData[COUNT];		//< 数组
	
	public:
		CArray(): m_iUsedCount(0){}

		//指定数组构造
		/*
			@code
			int t[] = {1,2,3,4,5}
			CArray<int, 100> a(t)
			@endcode
		*/
		template<int N>
		CArray(T (&tArray)[N])
			:m_iUsedCount(0)
		{
			AddItemList(tArray, N);
		}

		template<int C>
		CArray(const CArray<T, C>& tArray)
			:m_iUsedCount(0)
		{
			SL_ASSERT(tArray.GetLength() <= COUNT);
			if(tArray.IsNotEmpty())
			{
				AddItemList(tArray.GetData(), tArray.GetLength());
			}
		}

		//检查下标的合法性
		int CheckIndex(int i) const
		{
			int iIndex = 0;
			if( i < 0)
			{
				iIndex = 0;
			}
			else
			{
				int iMaxIndex = COUNT -1;
				iIndex = i > iMaxIndex ? iMaxIndex : i;
			}
			return iIndex;
		}


		///检查数组是否为空
		bool IsEmpty() const {return m_iUsedCount <= 0;}

		//是否已满
		bool IsFull() const {return m_iUsedCount >= COUNT;}

		//检查数组是否不为空
		bool IsNotEmpty() const {return m_iUsedCount > 0;}

		//取指定下标的数组元素下标
		const T& operator[] (int i) const
		{
			SL_ASSERT(i < COUNT && i>= 0);
			
			int iIndex = CheckIndex(i);
			
			SL_ASSERT(iIndex >= 0 && iIndex < COUNT);
			
			return m_astData[iIndex];
		}
		T& operator[] (int i)
		{
			SL_ASSERT(i < COUNT && i>= 0);

			int iIndex = CheckIndex(i);

			SL_ASSERT(iIndex >= 0 && iIndex < COUNT);

			return m_astData[iIndex];
		}

		//取数组的最大个数
		int GetCount() const {return COUNT;}

		//取数组有效数组的个数
		int GetLength() const {return m_iUsedCount;}
		int Size() const {return m_iUsedCount;}

		//取数组的指针
		const T* GetData() const {return m_astData;}
		T* GetData() {return m_astData;}



		void Clear()
		{
			m_iUsedCount = 0;
		}

		int ReSize(int iNewLen)
		{
			SL_ASSERT(iNewLen <= COUNT);
			m_iUsedCount = iNewLen > COUNT ? COUNT : iNewLen;
			return m_iUsedCount;
 		}

		template<int C>
		int AddItemList(const CArray<T,C>& tArray)
		{
			return AddItemList(tArray.GetData(), tArray.GetLength());
		}
		/*
			在数组最后添加指定的数组
			@param [in] tArray    指定的数组
			@param [in] N         数组定义的个数
			@return 返回的结果
				-1   表示增加失败
				-2	 表示N是无效的
				>=0	 表示增加元素的开始下标
		*/
		template<int N>
		int AddItemList(T (&tArray)[N])
		{
			return  AddItemList(tArray, N);
		}

		/*
			在数组最后增加指定的个数元素
			@param [in] tData	要增加的元素的指针
			@param [in]	iCount	要增加的元素个数
			@return 返回的结果
				-1	表示增加失败
				-2	表示m_iUseCount是无效的
				>=0	表示增加元素的开始下标
		*/
		int	AddItemList(const T* tData, int iCount)
		{
			if(iCount <= 0|| (m_iUsedCount + iCount > COUNT))
			{
				return -1;
			}
			if(m_iUsedCount < 0 || m_iUsedCount > COUNT)
			{
				return -2;
			}
			int iIndex = m_iUsedCount;
			for (int i = 0; i < iCount; i++)
			{
				m_astData[iIndex + i] = tData[i];
			}
			m_iUsedCount += iCount;
			return iIndex;
		}

		///在数组最后一位增加元素
		/*
			@param [in]		tItem	增加的 元素
			@return 返回结果
				-1	表示增加失败
				>=0	表示新增加元素的下标
		*/
		int AddOneItem(const T& tItem)
		{
			if(m_iUsedCount == COUNT)
			{
				return -1;
			}
			if(m_iUsedCount < 0 || m_iUsedCount > COUNT)
			{
				return -1;
			}
			int iIndex = m_iUsedCount;
			m_astData[m_iUsedCount] = tItem;
			++m_iUsedCount;
			return iIndex;
		}

		int Append(const T& tItem)
		{
			return AddOneItem(tItem);
		}

		//在数组最后追加一个元素
		/*
			@return T* 返回追加的元素
				-NOT NULL	表示追加成功
				-NULL		表示追加失败
		*/
		T* Append()
		{
			if(m_iUsedCount == COUNT)
			{
				return NULL;
			}
			if(m_iUsedCount < 0 || m_iUsedCount >= COUNT)
			{
				return NULL;
			}
			int iIndex = m_iUsedCount;
			++m_iUsedCount;
			new ((char*)(m_astData + iIndex)) T();
			return m_astData + iIndex;
		}

		//删除数组中指定下标的元素
		/*
			如果删除的非最后一个元素，则用最后一个元素来填补空位
			@param [in] iIndex 删除的下标
			@return 删除的结果
				-1	表示删除失败
				>=0 表示被删除的元素下标
		*/
		int DelOneItem(int iIndex)
		{
			if(iIndex < 0 || iIndex >= m_iUsedCount)
			{
				return -1;
			}
			if(m_iUsedCount <= 0)
			{
				return -1;
			}
			--m_iUsedCount;
			m_astData[iIndex] = m_astData[m_iUsedCount];
			return iIndex;
		}

		//删除数组中指定下标的元素
		/*
			被删除元素后面的元素一次前移
			@return 删除结果
				-1	表示删除失败
				>=0 表示被删除元素的下标
		*/
		int ShiftDelOneItem(int iIndex)
		{
			if(iIndex < 0 || iIndex >= m_iUsedCount)
			{
				return -1;
			}
			if(m_iUsedCount <= 0)
			{
				return -1;
			}
			--m_iUsedCount;
			int i = 0;
			for (i = iIndex; i < m_iUsedCount; ++i)
			{
				m_astData[i] = m_astData[i+1];
			}
			return iIndex;
		}

		//在数组指定位置插入一个元素
		int AddOneItemAt(const T& tData, int iIndex)
		{
			if(Size() >= GetCount())
			{
				return -1;
			}
			int iPos = iIndex;
			if(iPos < 0)
			{
				iPos = 0;
			}
			else if( iPos >= Size())
			{
				iPos = Size();
			}
			for (int i = Size(); i > iPos; i--)
			{
				m_astData[i] = m_astData[i-1];
			}
			m_astData[iPos] = tData;
			m_iUsedCount++;

			return 0;
		}


		/*
			向指定位置插入一个元素
			-1 表示直接插入到队尾
		*/
		int InsertOneItem(const T& tItem, int iIndex = -1)
		{
			if ((COUNT) == m_iUsedCount)
			{//数组已经满了
				return -1;
			}

			if (iIndex == -1 || iIndex >= m_iUsedCount)
			{///-1
				return AddOneItem(tItem);
			}

			for (int i = m_iUsedCount; i > iIndex; i--)
			{
				m_astData[i] = m_astData[ i - 1];
			}
			m_astData[iIndex] = tItem;
			m_iUsedCount++;
			return iIndex;
		}

		int UpdateOneItem(const T& tItem, int iIndex)
		{
			if(iIndex == -1 || iIndex >= m_iUsedCount)
			{
				return -1;
			}
			m_astData[iIndex] = tItem;
			return iIndex;
		}

		int FindOneItem(const T& tItem)
		{
			for (int index = 0; index < m_iUsedCount; ++index)
			{
				if(m_astData[index] == tItem)
				{
					return index;
				}
			}
			return -1;
		}

		//交换数组元素，0交换成功,-1表示失败,index超过数组范围
		int SwapItems(int iIndex1, int iIndex2)
		{
			if(iIndex1 >= m_iUsedCount || iIndex2 >= m_iUsedCount)
			{
				return -1;
			}
			T tmp = m_astData[iIndex1];
			m_astData[iIndex1] = m_astData[iIndex2];
			m_astData[iIndex2] = tmp;
			return 0;
		}

		typedef CArray<T, COUNT> THIS_ARRAT_TYPE;

	};

	typedef CArray<int, 2> TIntArray2;
	typedef CArray<int, 3> TIntArray3;
	typedef CArray<TIntArray2, 12> TIntArray12_2;
	typedef CArray<TIntArray2, 5> TIntArray5_2;



}// namespace sl
#endif