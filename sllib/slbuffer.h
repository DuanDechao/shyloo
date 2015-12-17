//��������������

#ifndef _SL_BUFFER_H_
#define _SL_BUFFER_H_

#include <string.h>
#include "slbase.h"
namespace sl
{
	//��������������
	
	/*˵��: ������������Ҫ���ڹ���һ���ض����ڴ��ַ
		  ǰ������Ա������m_iPos, m_pszBuf
	*/
	
	class CBuffer
	{
	protected:
		int		m_iPos;              //< �������α�
		char *	m_pszBuf;			 //< ��������ʼ�ڴ��ַ
		int		m_iSize;             //< ��������С
		int     m_iAct;              //< ACTֵ
		bool    m_bNew;              //< �Դ�����ʶ�����Ϊtrue��ʾ�û��������Լ�����new�����ģ�false��ʾ�ڴ�ռ������洫��

	public:
		//���캯��
		CBuffer(): m_iPos(0), m_pszBuf(0), m_iSize(0), m_iAct(0), m_bNew(false){}

		//��������
		virtual ~CBuffer()
		{
			Detach();
		}

		//�ҽ��ڴ��ַ��Ϊ��ʱ���д���
		/*
			@param [in] pszBuf ָ���ҽӵ��ڴ��ַ
			@param [in] iSize  ��������С
			@param [in] iAct   ACTֵ
			@return �ɹ�����0�������ڴ�ʧ�ܷ���-1
		*/
		int Attach(char *pszBuf, int iSize, int iAct)
		{
			//���ԭ�����ڴ��ַ�Ĺҽӣ��ڴ���Ϊ�Խ��������ڲ��ͷ��ڴ�
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
		

		//������ڴ��ַ�Ĺҽӣ��ڴ���Ϊ�Դ��������ͷ��ڴ�
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
		
		//׷��һ��char������
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

		//׷��һ��ָ��������
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

		//���ָ����С������
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
		
		//���α���ǰ�ƶ�iLen���ֽڣ������洢����
		int Append(int iLen)
		{
			SL_ASSERT(iLen >= 0);
			if(iLen > GetFreeLen())
				return -1;
			m_iPos += iLen;
			return 0;
		}

		//�Ƴ���������ǰ��ָ�������ֽ�
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
		
		//���û����α���0�����൱��ˢ�»�����������ɾ��ԭ������
		int Clear()
		{
			m_iPos = 0;
			return 0;
		}

		operator char* ()
		{
			return m_pszBuf;
		}


	
	//CBuffer�ڲ���Ϣ��ȡ����
	public:

		//��ȡ��������ַ
		char* GetUsedBuf() const
		{
			return m_pszBuf;
		}

		//��ȡ��ʹ�õĻ���������
		int GetUsedLen() const
		{
			return m_iPos;
		}

		//��ȡ���еĻ�������ʼ��ַ
		char* GetFreeBuf() const
		{
			return m_pszBuf + m_iPos;
		}

		//��ȡ�ռ�Ļ�������С
		int GetFreeLen() const
		{
			return m_iSize - m_iPos;
		}

		//��ȡ��������С
		int GetBufferSize() const
		{
			return m_iSize;
		}

		//��ȡACTֵ
		const int Act() const
		{
			return m_iAct;
		}

		//��ȡACTֵ������
		int & Act()
		{
			return m_iAct;
		}

		//�û������Ƿ��ѹҽ��ڴ��ַ
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