//��̬����ģ��

#ifndef _SL_ARRAY_H_
#define _SL_ARRAY_H_

#include "slassert.h"

namespace sl
{
	//����ģ����
	/*
		@code
			int t[] = {1,2,3,4}
			int t1[] = {1,2,3,4,5}

			CArray<int, 10> v1(t);  //������Ϊ�������
			CArray<int, 5> v2(v1);  //�Բ�ͬ��CArrayΪ�������
			CArray<int, 100> v3;	//Ĭ�Ϲ���
			v3.AddItemList(v2);		//׷��һ��CArray����
			v3.AddItemList(t);		//׷��һ������
			printArray(v1);
			printArray(v2);
			printArray(v3);
			v1 = v3;				//�Բ�ͬ��CArray���и�ֵ
			v2 = t1;				//��������и�ֵ
			printArray(v1);
			pirntArray(v2);
		@endcode
	*/

	template<typename T, int COUNT>
	class CArray
	{
	public:
		int		m_iUsedCount;			//< ��ʹ��������� 
		T       m_astData[COUNT];		//< ����
	
	public:
		CArray(): m_iUsedCount(0){}

		//ָ�����鹹��
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

		//����±�ĺϷ���
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


		///��������Ƿ�Ϊ��
		bool IsEmpty() const {return m_iUsedCount <= 0;}

		//�Ƿ�����
		bool IsFull() const {return m_iUsedCount >= COUNT;}

		//��������Ƿ�Ϊ��
		bool IsNotEmpty() const {return m_iUsedCount > 0;}

		//ȡָ���±������Ԫ���±�
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

		//ȡ�����������
		int GetCount() const {return COUNT;}

		//ȡ������Ч����ĸ���
		int GetLength() const {return m_iUsedCount;}
		int Size() const {return m_iUsedCount;}

		//ȡ�����ָ��
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
			������������ָ��������
			@param [in] tArray    ָ��������
			@param [in] N         ���鶨��ĸ���
			@return ���صĽ��
				-1   ��ʾ����ʧ��
				-2	 ��ʾN����Ч��
				>=0	 ��ʾ����Ԫ�صĿ�ʼ�±�
		*/
		template<int N>
		int AddItemList(T (&tArray)[N])
		{
			return  AddItemList(tArray, N);
		}

		/*
			�������������ָ���ĸ���Ԫ��
			@param [in] tData	Ҫ���ӵ�Ԫ�ص�ָ��
			@param [in]	iCount	Ҫ���ӵ�Ԫ�ظ���
			@return ���صĽ��
				-1	��ʾ����ʧ��
				-2	��ʾm_iUseCount����Ч��
				>=0	��ʾ����Ԫ�صĿ�ʼ�±�
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

		///���������һλ����Ԫ��
		/*
			@param [in]		tItem	���ӵ� Ԫ��
			@return ���ؽ��
				-1	��ʾ����ʧ��
				>=0	��ʾ������Ԫ�ص��±�
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

		//���������׷��һ��Ԫ��
		/*
			@return T* ����׷�ӵ�Ԫ��
				-NOT NULL	��ʾ׷�ӳɹ�
				-NULL		��ʾ׷��ʧ��
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

		//ɾ��������ָ���±��Ԫ��
		/*
			���ɾ���ķ����һ��Ԫ�أ��������һ��Ԫ�������λ
			@param [in] iIndex ɾ�����±�
			@return ɾ���Ľ��
				-1	��ʾɾ��ʧ��
				>=0 ��ʾ��ɾ����Ԫ���±�
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

		//ɾ��������ָ���±��Ԫ��
		/*
			��ɾ��Ԫ�غ����Ԫ��һ��ǰ��
			@return ɾ�����
				-1	��ʾɾ��ʧ��
				>=0 ��ʾ��ɾ��Ԫ�ص��±�
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

		//������ָ��λ�ò���һ��Ԫ��
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
			��ָ��λ�ò���һ��Ԫ��
			-1 ��ʾֱ�Ӳ��뵽��β
		*/
		int InsertOneItem(const T& tItem, int iIndex = -1)
		{
			if ((COUNT) == m_iUsedCount)
			{//�����Ѿ�����
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

		//��������Ԫ�أ�0�����ɹ�,-1��ʾʧ��,index�������鷶Χ
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