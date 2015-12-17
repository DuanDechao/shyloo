//缓冲区数据类型

#ifndef _SL_BUFFER_H_
#define _SL_BUFFER_H_

#include <string.h>
#include "slbase.h"
namespace sl
{
	//缓冲区数据类型
	
	/*说明: 该数据类型主要用于管理一块特定的内存地址
		  前两个成员必须是m_iPos, m_pszBuf
	*/
	
	class CBuffer
	{
	protected:
		int		m_iPos;              //< 缓冲区游标
		char *	m_pszBuf;			 //< 缓冲区起始内存地址
		int		m_iSize;             //< 缓冲区大小
		int     m_iAct;              //< ACT值
		bool    m_bNew;              //< 自创建标识，如果为true表示该缓冲区是自己创建new出来的，false表示内存空间是外面传入

	public:
		//构造函数
		CBuffer(): m_iPos(0), m_pszBuf(0), m_iSize(0), m_iAct(0), m_bNew(false){}

		//析构函数
		virtual ~CBuffer()
		{
			Detach();
		}

		//挂接内存地址，为空时自行创建
		/*
			@param [in] pszBuf 指定挂接的内存地址
			@param [in] iSize  缓冲区大小
			@param [in] iAct   ACT值
			@return 成功返回0，创建内存失败返回-1
		*/
		int Attach(char *pszBuf, int iSize, int iAct)
		{
			//解除原来与内存地址的挂接，内存若为自建，则由内部释放内存
			Detach();

			if (pszBuf == NULL)
			{
				m_bNew = true;
				m_pszBuf = new char[iSize];
				if(m_pszBuf == NULL)
				{
					return -1;
				}
			}else
			{
				m_bNew = false;
				m_pszBuf = pszBuf;
			}
			m_iPos = 0;
			m_iSize = iSize;
			m_iAct = iAct;

			return 0;

		}
		

		//解除与内存地址的挂接，内存若为自创建，则释放内存
		void Detach()
		{
			if(m_pszBuf != NULL && m_bNew)
			{
				delete [] m_pszBuf;
			}

			m_pszBuf  =  0;
			m_iSize   =  0;
			m_iPos    =  0;
			m_iAct    =  0;
			m_bNew    =  false;

		}
		
		//追加一个char型数据
		int AppendChar(char a_Value)
		{
			const int CHAR_SIZE  =  (int)sizeof(char);
			if(GetFreeLen() < CHAR_SIZE)
			{
				return -1;
			}
			memcpy(m_pszBuf + m_iPos, (const void *)&a_Value, CHAR_SIZE);
			m_iPos += CHAR_SIZE;
			return 0;
		}

		//追加一个指定的数据
		template<class T>
		int AppendData(const T& a_Data)
		{
			const int T_SIZE = (int) sizeof(T);
			if(GetFreeLen() < T_SIZE)
			{
				return -1;
			}
			memcpy(m_pszBuf + m_iPos, (const void *)&a_Data, T_SIZE);
			m_iPos += T_SIZE;
			return 0;

		}

		//添加指定大小的数据
		int Append(const char *pszBuf, int iLen)
		{
			SL_ASSERT(pszBuf && iLen >= 0);
			if(GetFreeLen() < iLen)
			{
				return -1;
			}
			memcpy(m_pszBuf + m_iPos, (const void *)pszBuf, iLen);
			m_iPos += iLen;
			return 0;
		}
		
		//将游标向前移动iLen个字节，并不存储数据
		int Append(int iLen)
		{
			SL_ASSERT(iLen >= 0);
			if(iLen > GetFreeLen())
				return -1;
			m_iPos += iLen;
			return 0;
		}

		//移除缓冲区中前面指定长度字节
		int Remove(int iLen)
		{
			if(iLen > m_iPos)
			{
				return -1;
			}
			else if(iLen < m_iPos)
			{
				memmove(m_pszBuf, m_pszBuf+iLen, m_iPos-iLen);
			}
			m_iPos -= iLen;
			return 0;
		}
		
		//重置缓存游标至0处，相当于刷新缓冲区，而不删除原有数据
		int Clear()
		{
			m_iPos = 0;
			return 0;
		}

		operator char* ()
		{
			return m_pszBuf;
		}


	
	//CBuffer内部信息获取函数
	public:

		//获取缓冲区地址
		char* GetUsedBuf() const
		{
			return m_pszBuf;
		}

		//获取已使用的缓冲区长度
		int GetUsedLen() const
		{
			return m_iPos;
		}

		//获取空闲的缓冲区起始地址
		char* GetFreeBuf() const
		{
			return m_pszBuf + m_iPos;
		}

		//获取空间的缓冲区大小
		int GetFreeLen() const
		{
			return m_iSize - m_iPos;
		}

		//获取缓冲区大小
		int GetBufferSize() const
		{
			return m_iSize;
		}

		//获取ACT值
		const int Act() const
		{
			return m_iAct;
		}

		//获取ACT值的引用
		int & Act()
		{
			return m_iAct;
		}

		//该缓冲区是否已挂接内存地址
		bool IsAttached() const
		{
			return m_pszBuf != 0;
		}

		bool IsNewMemory() const
		{
			return m_bNew;
		}

	}; //class Buffer


} // namespace sl

#endif